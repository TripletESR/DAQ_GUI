#include "oscilloscope.h"

Oscilloscope::Oscilloscope(ViRsrc name): QSCPI(name)
{

    int SBR = StatusByteRegister();
    qDebug("%#x, %#x, %s", SBR, EventStatusRegister() ,GetErrorMsg().toStdString().c_str());

    //if( !(SBR == 161 || SBR == 129 || SBR == 1) ) {
    //    sta = VI_ERROR_ABORT;
    //    qDebug() << "Try to change trigger level & try again. Exact reason unknown.";
    //}

    if( sta == VI_SUCCESS){
        this->name = GetName();
        qDebug() << "Instrument identification string:\n \t" <<  this->name;
        openFlag = 1;
    }else{
        qDebug() << "Cannot open " << name;
        openFlag = 0;
        this->name = "Oscilloscope";
        return;
    }

    //trgCh = 5;

    SetRemoteLog(1);
}

Oscilloscope::~Oscilloscope(){
    SystemLock(0);
    SetRemoteLog(0);
    Clear();
}

void Oscilloscope::Initialize()
{
    if( sta != VI_SUCCESS) return;

    SendMsg("======= Initialize Oscilloscope");
    SetAcqMode(0);
    SetTime(500E-6, 160E-6);
    SetVoltage(1, 8, 0, 1);
    SetVoltage(2, 0.4, 0, 0);
    OpenCh(1,1);
    OpenCh(2,1);
    SetTriggerLevel(2, 0.02);
    SetTrigger(2);
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

void Oscilloscope::SetVoltage(int ch, double range, double offset, bool ohm1M){

    //:CHANnel<n>:OFFSet?
    //:CHANnel<n>:IMPedance? ONEMeg | FIFTy
    //:CHANnel<n>:RANGe? //full range
    //:CHANnel<n>:SCALe? // div

    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":channel%d:range %f\n", ch, range);SendCmd(cmd); // range for 8 div
    sprintf(cmd,":channel%d:offset %f\n", ch, offset);SendCmd(cmd);

    if( ohm1M ) {
        sprintf(cmd,":channel%d:impedance ONEMEG\n", ch);SendCmd(cmd);
    }else{
        sprintf(cmd,":channel%d:impedance FIFTY\n", ch);SendCmd(cmd);
    }
}

void Oscilloscope::SetTrigger(int ch)
{
    if( sta != VI_SUCCESS) return;
    if( ch <= 4 && ch >=1 ){
        sprintf(cmd,":trigger:source channel%d\n", ch); SendCmd(cmd);
    }else if(ch == 0){
        sprintf(cmd,":trigger:source external\n"); SendCmd(cmd);
    }
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
    scpi_Msg.sprintf(" ============== Setting DVM. ch:%d, IO:%d", ch, IO);
    SendMsg(scpi_Msg);
    sprintf(cmd,":DVM:enable %d\n", IO); SendCmd(cmd);
    sprintf(cmd,":DVM:source channel%d\n", ch); SendCmd(cmd);
    DVMIO = IO;
    DVMCh = ch;
    DVMMode = mode;

    sprintf(cmd, ":dvm:arange 1\n"); // set auto range
    SendCmd(cmd);

    switch(mode){
        case 0 : sprintf(cmd,":DVM:mode DC\n"); SendCmd(cmd); break;
        case 1 : sprintf(cmd,":DVM:mode DCRMS\n"); SendCmd(cmd); break;
        case 2 : sprintf(cmd,":DVM:mode ACRMS\n"); SendCmd(cmd); break;
    }

    SetVoltage(ch, vRange[ch], vOffset[ch], 1); // set resistanc eot be 1M.
    ohm[ch] = 1;
}

void Oscilloscope::GetChannelData(int ch)
{
    if( sta != VI_SUCCESS) return;

    scpi_Msg.sprintf("==== Getting Channal Setting, CH %d", ch);
    SendMsg(scpi_Msg);

    sprintf(cmd,":channel%d:display?\n", ch);
    IO[ch] = Ask(cmd).toInt();

    sprintf(cmd,":trigger:level? Channel%d\n", ch);
    trgLevel[ch] = Ask(cmd).toDouble();

    sprintf(cmd,":channel%d:range?\n", ch);
    vRange[ch] = Ask(cmd).toDouble();

    sprintf(cmd,":channel%d:offset?\n", ch);
    vOffset[ch] = Ask(cmd).toDouble();

    sprintf(cmd,":channel%d:impedance?\n", ch);
    QString ans = Ask(cmd);
    if( ans == "FIFT") ohm[ch] = 0; //need to match the checkbox index
    if( ans == "ONEM") ohm[ch] = 1;

    //sprintf(cmd,":trigger:source?\n", ch);
    //trgCh = Ask(cmd).right(1).toInt();
    //qDebug() << "Trg Channel : " <<trgCh;

}

double Oscilloscope::GetDVM()
{
    sprintf(cmd,":dvm:current?\n");
    return Ask(cmd).toDouble();
}

void Oscilloscope::GetTime()
{
    if( sta != VI_SUCCESS) return;

    SendMsg("==== Getting Time Setting");

    sprintf(cmd,":timebase:range?\n");
    tRange = Ask(cmd).toDouble() * 1e6; // time in us

    sprintf(cmd,":timebase:position?\n");
    tDelay = Ask(cmd).toDouble() * 1e6; //time delay
}

double Oscilloscope::GetTriggerRate()
{
    if( sta != VI_SUCCESS) return 0;
    sprintf(cmd,":measure:frequency? channel%d\n", trgCh);
    trgRate = Ask(cmd).toInt();
    return trgRate;

}

int Oscilloscope::GetAcquireCount()
{
    if( sta != VI_SUCCESS) return 0;
    sprintf(cmd,":acquire:count?\n");
    acqCount = Ask(cmd).toInt();
    return acqCount;
}

void Oscilloscope::GetSystemStatus(){
    if( sta != VI_SUCCESS) return;

    SendMsg("==== Getting System Setting");

    sprintf(cmd,":system:lock?\n");
    lockFlag = Ask(cmd).toInt();

    sprintf(cmd,":system:touch?\n");
    touchFlag = Ask(cmd).toInt();

    sprintf(cmd,":acquire:type?\n");
    QString mode = Ask(cmd);

    GetAcquireCount();

    if(mode == "AVER"){
        acqFlag = 1;
    }else{
        acqFlag = 0;
    }
    //qDebug() << " acquire:Type : " << mode << ", " << acqFlag;

    sprintf(cmd,":trigger:source?\n");
    trgCh = Ask(cmd).right(1).toInt();

    GetTriggerRate();

    sprintf(cmd,":system:rlogger:state?\n");
    logFlag = Ask(cmd).toInt();

    sprintf(cmd,":dvm:enable?\n");
    DVMIO = Ask(cmd).toInt();

    sprintf(cmd,":dvm:source?\n");
    DVMCh = Ask(cmd).right(1).toInt();

    sprintf(cmd,":dvm:mode?\n");
    mode = Ask(cmd);
    if( mode == "DC") DVMMode = 0;
    if( mode == "DCRM") DVMMode = 1;
    if( mode == "ACRM") DVMMode = 2;

}

void Oscilloscope::GetData(int ch, const int points, bool Save2BG)
{
    if( sta != VI_SUCCESS) return;
    xData[ch].clear();
    yData[ch].clear();

    GetTime();
    GetSystemStatus();
    GetChannelData(ch);

    xData[ch] = QVector<double>(points);
    yData[ch] = QVector<double>(points);
    if(Save2BG) BGData = QVector<double>(points);
    if(Save2BG) SendMsg("Getting Background signals.");

    if( this->acqFlag == 0){
        //======= block method
        //sprintf(cmd,":digitize channel%d\n", ch); SendCmd(cmd);

        //======= Polling method
        SendMsg("===== Polling Data.");
        sprintf(cmd,":STOP\n"); SendCmd(cmd);
        sprintf(cmd,"*OPC?\n");
        bool opc = Ask(cmd).toInt();
        qDebug() << "Operation completed? " << opc;
        if( !opc) {
            qDebug() << "Operation did not complete ";
            return;
        }

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
            TranslateRawData(ch, points, Save2BG);
        }else{
            SendMsg("===== 10 sec timeout for single-shot trigger.");
        }

        Resume(ch);

    }

    //TODO given to signal is 60 Hz, with known # of avergae, estimated the time.
    if( this->acqFlag == 1){
        SendMsg("===== Synchronizing in averaging acquisition mode.");
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

        double timeExpect = acqCount / trgRate; // sec
        timeElapsed = 0;
        double waitsec = 1.0;//wait for # sec
        //if(timeExpect > 10) {
        //    waitsec = 5.0;
        //}
        maxWaitTime = qCeil(timeExpect)+5;
        SyncOSC(ch, points, waitsec, Save2BG);
        //QProgressDialog progBox("Waiting for the device....", "Abort.", 0, maxWaitSec);
        //progBox.setWindowModality(Qt::WindowModal);
        //bool breakFlag = 0;
        //do{
        //    //scpi_Msg.sprintf("Waiting for the device: %5.1f sec. %#x =? %#x (Expect %d sec).", lngElapsed, SBR, 161, maxWaitSec);
        //    //SendMsg(scpi_Msg);
        //    //progBox.setLabelText(scpi_Msg);
        //    Sleep(waitsec * 1000);
        //    lngElapsed += waitsec;
        //    //progBox.setValue(lngElapsed);
        //    SBR = StatusByteRegister(); //161 is system "good" status SBR
        //    //if( progBox.wasCanceled() ) {
        //    //    breakFlag = 1;
        //    //    break;
        //    //}
        //}while((SBR & 32) == 0); // 32 is the ESR registor bit

        //if( breakFlag ) return;

        // Clear ESR and restore previously saved *ESE mask.
        //EventStatusRegister();
        //sprintf(cmd,"*ESE 255\n"); SendCmd(cmd);
        //
        ////Get Result
        //sprintf(cmd,":waveform:source channel%d\n", ch); SendCmd(cmd);
        //sprintf(cmd,":waveform:format ASCii\n");      SendCmd(cmd);
        //sprintf(cmd,":waveform:points %d\n", points+1); SendCmd(cmd);
        //sprintf(cmd,":waveform:data?\n");             SendCmd(cmd);
        //char rawData[900000];
        //viScanf(device, "%t", rawData);
        //QString raw = rawData;
        //QStringList data = raw.mid(10).split(',');
        ////qDebug() << data;
        ////qDebug() << "number of data : " << data.length();
        //
        //double xOrigin = -(tRange/2-tDelay);
        //double xStep = tRange/points;
        //
        //for( int i = 0 ; i < points; i++){
        //    //qDebug() << (data[i]).toDouble();
        //    xData[ch][i] = xOrigin + i * xStep;
        //    yData[ch][i] = (data[i]).toDouble();
        //    if( Save2BG ) BGData[i] = yData[ch][i];
        //    //qDebug() << i << "," << xData[i] << "," << yData[i];
        //}

    }

}

void Oscilloscope::SyncOSC(int ch, int points, double waitsec, bool Save2BG){
    QTimer timer;
    int SBR = StatusByteRegister();

    if( SBR & 32 == 0 ) {
        NotFinished(timeElapsed);
        timer.singleShot(waitsec*1000, this, SLOT(SyncOSC(double,bool)));
        timeElapsed += waitsec*1000;
    }else{

        // Clear ESR and restore previously saved *ESE mask.
        EventStatusRegister();
        sprintf(cmd,"*ESE 255\n"); SendCmd(cmd);

        TranslateRawData(ch, points, Save2BG);

        Resume(ch);
    }
}

void Oscilloscope::TranslateRawData(int ch, int points, bool Save2BG){
    //Get Result
    sprintf(cmd,":waveform:source channel%d\n", ch); SendCmd(cmd);
    sprintf(cmd,":waveform:format ASCii\n");      SendCmd(cmd);
    sprintf(cmd,":waveform:points %d\n", points+1); SendCmd(cmd);
    sprintf(cmd,":waveform:data?\n");             SendCmd(cmd);
    char rawData[900000];
    viScanf(device, "%t", rawData);
    QString raw = rawData;
    QStringList data = raw.mid(10).split(',');
    //qDebug() << data;
    //qDebug() << "number of data : " << data.length();

    double xOrigin = -(tRange/2-tDelay);
    double xStep = tRange/points;

    for( int i = 0 ; i < points; i++){
        //qDebug() << (data[i]).toDouble();
        xData[ch][i] = xOrigin + i * xStep;
        yData[ch][i] = (data[i]).toDouble();
        if( Save2BG ) BGData[i] = yData[ch][i];
        //qDebug() << i << "," << xData[i] << "," << yData[i];
    }
}

void Oscilloscope::Resume(int ch){

    //resume
    SendMsg("===== Resume RUN.");
    sprintf(cmd,":RUN\n"); SendCmd(cmd);
    for( int i = 1; i <=4 ; i++){
        OpenCh(i, IO[i]);
    }

    xMax = GetMax(xData[ch]);
    xMin = GetMin(xData[ch]);
    yMax = GetMax(yData[ch]);
    yMin = GetMin(yData[ch]);

    scpi_Msg.sprintf("X :(%7f, %7f)", xMin, xMax); SendMsg(scpi_Msg);
    scpi_Msg.sprintf("Y :(%7f, %7f)", yMin, yMax); SendMsg(scpi_Msg);
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


