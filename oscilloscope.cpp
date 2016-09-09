#include "oscilloscope.h"

Oscilloscope::Oscilloscope(ViRsrc name, bool init):
    SCPI(name)
{
    if( sta != VI_SUCCESS) {
        qDebug() << "Cannot open " << name;
        return;
    }

    int SBR = StatusByteRegister();
    qDebug("%#x, %#x, %s", SBR, EventStatusRegister() ,GetErrorMsg().toStdString().c_str());

    if( !(SBR == 161 || SBR == 129 || SBR == 1) ) {
        sta = VI_ERROR_ABORT;
        qDebug() << "Try to change trigger level & try again. Exact reason unknown.";
    }

    if( sta == VI_SUCCESS){
        if ( init ) viPrintf(device, "*RST\n");
        this->name = GetName();
        qDebug() << "Instrument identification string:\n \t" <<  this->name;
    }else{
        qDebug() << "Cannot open " << name;
    }
}

Oscilloscope::~Oscilloscope(){
    SystemLock(0);
    SetRemoteLog(0);
    Clear();
}

void Oscilloscope::Initialize(int ch)
{
    if( sta != VI_SUCCESS) return;
    SetAcqMode(0);
    SetTime(200E-6, 40E-6);
    SetVoltage(1, 4, 0, 1);
    OpenCh(1,1);
    SetTriggerLevel(1, 0);
    SetTrigger(1);
}

void Oscilloscope::SetRemoteLog(bool log)
{
    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":system:rlogger %d\n", log);SendCmd(cmd);
    sprintf(cmd,":system:rlogger:display %d\n", log);SendCmd(cmd);
    logFlag =1;
}

void Oscilloscope::SetTouch(bool touch)
{
    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":SYSTem:TOUCh %d\n", touch);SendCmd(cmd);
    touchFlag =1;
}

void Oscilloscope::SetPreset()
{
    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":SYSTem:preset\n");SendCmd(cmd);
}

void Oscilloscope::SystemLock(bool lock){
    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":SYSTem:Lock %d\n", lock);SendCmd(cmd);
    lockFlag = 1;
}

void Oscilloscope::SetTime(double range, double delay){
    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":timebase:range %f\n", range);SendCmd(cmd); // time for 10 div
    sprintf(cmd,":timebase:position %f\n", delay);SendCmd(cmd); //time delay
}

void Oscilloscope::SetTriggerLevel(int ch, double level)
{
    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":trigger:level %f, Channel%d\n", level, ch);SendCmd(cmd); // in Voltage
    sprintf(cmd,":trigger:slope positive\n"); SendCmd(cmd);

}

void Oscilloscope::OpenCh(int ch, bool display){
    if( sta != VI_SUCCESS) return;
    //:CHANnel<n>:DISPlay?
    sprintf(cmd,":channel%d:display %d\n", ch, display);SendCmd(cmd);
}

void Oscilloscope::SetVoltage(int ch, double range, double offset, bool ohm50){

    //:CHANnel<n>:OFFSet?
    //:CHANnel<n>:IMPedance? ONEMeg | FIFTy
    //:CHANnel<n>:RANGe? //full range
    //:CHANnel<n>:SCALe? // div

    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":channel%d:range %f\n", ch, range);SendCmd(cmd); // range for 8 div
    sprintf(cmd,":channel%d:offset %f\n", ch, offset);SendCmd(cmd);

    if( ohm50 ) {
        sprintf(cmd,":channel%d:impedance FIFTY\n", ch);SendCmd(cmd);
    }else{
        sprintf(cmd,":channel%d:impedance ONEMEG\n", ch);SendCmd(cmd);
    }
}

void Oscilloscope::SetTrigger(int ch)
{
    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":trigger:source channel%d\n", ch); SendCmd(cmd);
}

void Oscilloscope::SetAcqMode(int mode){
    if( sta != VI_SUCCESS) return;
    switch (mode) {
        case 0:
            sprintf(cmd,":acquire:type normal\n");
            SendCmd(cmd);
            acqFlag = 0;
            break;
        case 1:
            sprintf(cmd,":acquire:type average\n");
            SendCmd(cmd);
            sprintf(cmd,":acquire:count?\n");
            this->acqFlag = Ask(cmd).toInt();
            break;
    }
}
void Oscilloscope::SetAverage(int count)
{
    if( sta != VI_SUCCESS) return;
    acqCount = count;
    sprintf(cmd,":acquire:type average\n"); SendCmd(cmd);
    sprintf(cmd,":acquire:count %d\n", count); SendCmd(cmd);
    //sprintf(cmd,":acquire:srate?\n");
    //double srate = Ask(cmd).toInt();
    //qDebug() << "sample rate : " << srate << " samples/s";
    //qDebug() << "estimated time for complete cycle : " << count / srate * 1e3 << " ms";
}

void Oscilloscope::SetDVM(bool IO, int ch, int mode)
{
    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":DVM:enable %d\n", IO); SendCmd(cmd);
    sprintf(cmd,":DVM:source channel%d\n", ch); SendCmd(cmd);

    switch(mode){
        case 0 : sprintf(cmd,":DVM:mode DC\n"); SendCmd(cmd); break;
        case 1 : sprintf(cmd,":DVM:mode DCRMS\n"); SendCmd(cmd); break;
        case 2 : sprintf(cmd,":DVM:mode ACRMS\n"); SendCmd(cmd); break;
    }
}

void Oscilloscope::GetChannelData(int ch)
{
    if( sta != VI_SUCCESS) return;

    sprintf(cmd,":trigger:level? Channel%d\n", ch);
    trgLevel[ch] = Ask(cmd).toDouble();

    sprintf(cmd,":channel%d:display?\n", ch);
    IO[ch] = Ask(cmd).toInt();

    sprintf(cmd,":channel%d:range?\n", ch);
    xRange[ch] = Ask(cmd).toDouble();

    sprintf(cmd,":channel%d:offset?\n", ch);
    xOffset[ch] = Ask(cmd).toDouble();

    sprintf(cmd,":channel%d:impedance?\n", ch);
    QString ans = Ask(cmd);
    if( ans == "FITT") ohm[ch] = 0; //need to match the checkbox index
    if( ans == "ONEM") ohm[ch] = 1;

    sprintf(cmd,":trigger:source?\n", ch);
    trgCh = Ask(cmd).right(1).toInt();
    qDebug() << "Trg Channel : " <<trgCh;

}

void Oscilloscope::GetTime()
{
    if( sta != VI_SUCCESS) return;

    sprintf(cmd,":timebase:range?\n");
    tRange = Ask(cmd).toDouble() * 1e6; // time in us

    sprintf(cmd,":timebase:position?\n");
    tDelay = Ask(cmd).toDouble() * 1e6; //time delay
}

void Oscilloscope::GetSystemStatus(){
    if( sta != VI_SUCCESS) return;

    sprintf(cmd,":system:lock?\n");
    lockFlag = Ask(cmd).toInt();

    sprintf(cmd,":system:touch?\n");
    touchFlag = Ask(cmd).toInt();

    sprintf(cmd,":acquire:type?\n");
    QString mode = Ask(cmd);

    sprintf(cmd,":acquire:count?\n");
    acqCount = Ask(cmd).toInt();

    if(mode == "AVER"){
        acqFlag = 1;
    }else{
        acqFlag = 0;
    }
    //qDebug() << " acquire:Type : " << mode << ", " << acqFlag;

    sprintf(cmd,":trigger:source?\n");
    trgCh = Ask(cmd).right(1).toInt();

    sprintf(cmd,":system:rlogger:state?\n");
    logFlag = Ask(cmd).toInt();

}

void Oscilloscope::GetData(int ch, const int points, int GetMethod = 0)
{
    if( sta != VI_SUCCESS) return;
    xData[ch].clear();
    yData[ch].clear();

    GetTime();

    xData[ch] = QVector<double>(points);
    yData[ch] = QVector<double>(points);

    if( GetMethod == 0){
        //======= block method
        //sprintf(cmd,":digitize channel%d\n", ch); SendCmd(cmd);

        //======= Polling method
        sprintf(cmd,":STOP\n"); SendCmd(cmd);
        sprintf(cmd,"*OPC?\n");
        qDebug() << "Ready : " << Ask(cmd);
        // Single Acquistion; cannot be average measurement
        sprintf(cmd,":Single\n"); SendCmd(cmd);

        const long lngTimeout = 10000; //10 sec
        long lngElapsed = 0;

        while (lngElapsed < lngTimeout) {
            sprintf(cmd,":operegister:condition?\n");
            int varQueryResult = Ask(cmd).toInt();
            qDebug() << varQueryResult << ", " << (varQueryResult & 8);
            if( (varQueryResult & 8) == 0){
                break;
            }else{
                Sleep(100);
                lngElapsed += 100;
            }
        }

        //------- Get Data
        if( lngElapsed < lngTimeout){
            sprintf(cmd,":waveform:source channel%d\n", ch); SendCmd(cmd);
            sprintf(cmd,":waveform:format ASCii\n"); SendCmd(cmd);
            sprintf(cmd,":waveform:points %d\n", points+1); SendCmd(cmd);

            sprintf(cmd,":waveform:data?\n"); SendCmd(cmd);
            char rawData[90000];
            viScanf(device, "%t", rawData);
            QString raw = rawData;
            QStringList data = raw.mid(10).split(',');
            //qDebug() << data;
            qDebug() << "number of data : " << data.length();

            double xOrigin = -(tRange/2-tDelay);
            double xStep = tRange/points;

            for( int i = 0 ; i < data.length(); i++){
                //qDebug() << (data[i]).toDouble();
                xData[ch][i] = xOrigin + i * xStep;
                yData[ch][i] = (data[i]).toDouble();
                //qDebug() << i << "," << xData[i] << "," << yData[i];
            }
        }else{
            qDebug() << "Timeout waiting for single-shot trigger.";
        }

    }

    if( GetMethod == 1){
        //Synchronizing in averaging acquisition mode.
        sprintf(cmd,":STOP\n"); SendCmd(cmd);
        sprintf(cmd,"*OPC?\n");
        bool opc = Ask(cmd).toInt();
        qDebug() << "Operation completed? " << opc;
        if( !opc) {
            qDebug() << "Operation did not complete ";
            return;
        }

        //Clear ESR (Standard Event Status register)
        int ESR = 0;
        do{
            ESR = EventStatusRegister();
        }while(ESR !=0);

        //Set *ESE mask to allow only OPC (Operation Complete) bit.
        sprintf(cmd,"*ESE 1\n"); SendCmd(cmd);

        //Acquire using :DIGitize. Set up OPC bit to be set when the operation is complete.
        sprintf(cmd,":DIGITIZE channel%d\n", ch); SendCmd(cmd);
        sprintf(cmd,"*OPC\n"); SendCmd(cmd);

        int SBR;
        double lngElapsed = 0;
        double waitsec = 0.5;//wait for # sec
        do{
            Sleep(waitsec * 1000);
            lngElapsed += waitsec;
            SBR = StatusByteRegister();
            qDebug("Waiting for the device: %5.1f sec. %#x =? %#x", lngElapsed, SBR, 161); //161 is system "good" status SBR
        }while((SBR & 32) == 0); // 32 is the ESR registor bit

        // Clear ESR and restore previously saved *ESE mask.
        EventStatusRegister();
        sprintf(cmd,"*ESE 255\n"); SendCmd(cmd);

        //Get Result
        sprintf(cmd,":waveform:source channel%d\n", ch); SendCmd(cmd);
        sprintf(cmd,":waveform:format ASCii\n");      SendCmd(cmd);
        sprintf(cmd,":waveform:points %d\n", points+1); SendCmd(cmd);
        sprintf(cmd,":waveform:data?\n");             SendCmd(cmd);
        char rawData[90000];
        viScanf(device, "%t", rawData);
        QString raw = rawData;
        QStringList data = raw.mid(10).split(',');
        //qDebug() << data;
        qDebug() << "number of data : " << data.length();

        double xOrigin = -(tRange/2-tDelay);
        double xStep = tRange/points;

        for( int i = 0 ; i < points; i++){
            //qDebug() << (data[i]).toDouble();
            xData[ch][i] = xOrigin + i * xStep;
            yData[ch][i] = (data[i]).toDouble();
            //qDebug() << i << "," << xData[i] << "," << yData[i];
        }

    }

    //resume
    sprintf(cmd,":RUN\n"); SendCmd(cmd);
    for( int i = 1; i <=4 ; i++){
        OpenCh(i, IO[i]);
    }

    xMax = GetMax(xData[ch]);
    xMin = GetMin(xData[ch]);
    yMax = GetMax(yData[ch]);
    yMin = GetMin(yData[ch]);

    qDebug("X :(%7f, %7f)", xMin, xMax);
    qDebug("Y :(%7f, %7f)", yMin, yMax);

}

double Oscilloscope::GetMax(QVector<double> vec){
    double max = vec[0];
    int size = vec.size();
    for(int i = 1 ; i < size ; i++){
        if( vec[i] > max) max = vec[i];
    }
    return max;
}

double Oscilloscope::GetMin(QVector<double> vec)
{
    double min = vec[0];
    int size = vec.size();
    for(int i = 1 ; i < size ; i++){
        if( vec[i] < min) min = vec[i];
    }
    return min;
}


