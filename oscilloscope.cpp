#include "oscilloscope.h"

Oscilloscope::Oscilloscope(ViRsrc name, bool init):
    SCPI(name, init)
{

}

Oscilloscope::~Oscilloscope(){
    SystemLock(0);
    SetRemoteLog(0);
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
            break;
        case 1:
            sprintf(cmd,":acquire:type average\n");
            SendCmd(cmd);
            sprintf(cmd,":acquire:count?\n");
            this->count = Ask(cmd).toInt();
            break;
    }
}
void Oscilloscope::SetAverage(int count)
{
    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":acquire:type average\n"); SendCmd(cmd);
    sprintf(cmd,":acquire:count %d\n", count); SendCmd(cmd);
    sprintf(cmd,":acquire:srate?\n");
    double srate = Ask(cmd).toInt();
    qDebug() << "sample rate : " << srate << " samples/s";
    qDebug() << "estimated time for complete cycle : " << count / srate * 1e3 << " ms";
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

void Oscilloscope::GetData(int ch, const int points)
{
    if( sta != VI_SUCCESS) return;
    xData[ch].clear();
    yData[ch].clear();

    GetTime();

    xData[ch] = QVector<double>(points);
    yData[ch] = QVector<double>(points);

    //qDebug() << xData.size();

    //sprintf(cmd,":digitize channel%d\n", ch); SendCmd(cmd);
    sprintf(cmd,":waveform:source channel%d\n", ch); SendCmd(cmd);
    sprintf(cmd,":waveform:format ASCii\n"); SendCmd(cmd);
    sprintf(cmd,":waveform:points %d\n", points); SendCmd(cmd);

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

    //sprintf(cmd,":RUN\n"); SendCmd(cmd);

    xMax = GetMax(xData[ch]);
    xMin = GetMin(xData[ch]);
    yMax = GetMax(yData[ch]);
    yMin = GetMin(yData[ch]);

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

void SaveData(QString filename){



}
