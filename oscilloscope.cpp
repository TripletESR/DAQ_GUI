#include "oscilloscope.h"

Oscilloscope::Oscilloscope(ViRsrc name, bool init):
    SCPI(name, init)
{

}

Oscilloscope::~Oscilloscope(){
}

void Oscilloscope::Initialize(int ch)
{
    if( sta != VI_SUCCESS) return;
        SetAcqMode(0);
        SetTime(200E-6, 40E-6);
        SetVoltage(1, 1, 4, 0, 1);
        SetTrigger(1, 0);
}

void Oscilloscope::SetTouch(bool touch)
{
    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":SYSTem:TOUCh? %d\n", touch);SendCmd(cmd);
}

void Oscilloscope::SetPreset()
{
    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":SYSTem:preset\n");SendCmd(cmd);
}

void Oscilloscope::SetTime(double range, double delay){
    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":timebase:range %f\n", range);SendCmd(cmd); // time for 10 div
    sprintf(cmd,":timebase:position %f\n", delay);SendCmd(cmd); //time delay
}

void Oscilloscope::SetTrigger(int ch, double level)
{
    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":trigger:level %f Channel%d\n", level, ch);SendCmd(cmd); // in Voltage
    sprintf(cmd,":trigger:slope positive\n"); SendCmd(cmd);

}

void Oscilloscope::SetVoltage(int ch, bool display, double range, double offset, bool ohm50){
    //:CHANnel<n>:DISPlay?
    //:CHANnel<n>:OFFSet?
    //:CHANnel<n>:IMPedance? ONEMeg | FIFTy
    //:CHANnel<n>:RANGe? //full range
    //:CHANnel<n>:SCALe? // div

    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":channel%d:display %d\n", ch, display);SendCmd(cmd);
    sprintf(cmd,":channel%d:range %f\n", ch, range);SendCmd(cmd); // range for 8 div
    sprintf(cmd,":channel%d:offset %f\n", ch, offset);SendCmd(cmd);

    if( ohm50 ) {
        sprintf(cmd,":channel%d:impedance FIFTY\n", ch);SendCmd(cmd);
    }else{
        sprintf(cmd,":channel%d:impedance ONEMEG\n", ch);SendCmd(cmd);
    }
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
            SendCmd(cmd);
            //SendCmd(":acquire:count?\n");
            ReadRespond();
            this->count = QString(buf).toInt();
            break;
    }
}

void Oscilloscope::SetAverage(int count)
{
    if( sta != VI_SUCCESS) return;
    sprintf(cmd,":acquire:type average\n"); SendCmd(cmd);
    sprintf(cmd,":acquire:count %d\n", count); SendCmd(cmd);
    sprintf(cmd,":acquire:srate?\n"); SendCmd(cmd);
    ReadRespond();
    //viScanf(device, "%t", buf);
    //*std::remove(buf, buf+strlen(buf), '\n') = '\0';
    qDebug() << "sample rate : " << buf << " samples/s";
    double srate = atof(buf);
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
    //sprintf(cmd,":trigger:level? Channel%d\n", ch);SendCmd(cmd); // in Voltage
    //sprintf(cmd,":trigger:slope positive\n"); SendCmd(cmd);
}

void Oscilloscope::GetData(int ch, const int points)
{
    if( sta != VI_SUCCESS) return;
    xData = QVector<double>(points);
    yData = QVector<double>(points);

    //qDebug() << xData.size();

    sprintf(cmd,":digitize channel%d\n", ch); SendCmd(cmd);
    sprintf(cmd,":waveform:source channel%d\n", ch); SendCmd(cmd);
    sprintf(cmd,":waveform:format ASCii\n"); SendCmd(cmd);
    sprintf(cmd,":waveform:points %d\n", points); SendCmd(cmd);

    sprintf(cmd,":waveform:data?\n"); SendCmd(cmd);
    viScanf(device, "%t", rawData);
    QString raw = rawData;
    QStringList data = raw.mid(10).split(',');
    //qDebug() << data;
    qDebug() << "number of data : " << data.length();

    sprintf(cmd,":waveform:xorigin?\n"); SendCmd(cmd);
    viScanf(device, "%t", buf);
    //qDebug() << buf;
    double xOrigin = QString(buf).toDouble() * 1e+6 ; //time in us
    qDebug() << "x origin : "<< xOrigin;

    sprintf(cmd,":waveform:xincrement?\n"); SendCmd(cmd);
    viScanf(device, "%t", buf);
    double xInc = QString(buf).toDouble() * 1e+6; //time in us
    qDebug() << "x inc : "<< xInc;

    for( int i = 0 ; i < data.length(); i++){
        //qDebug() << (data[i]).toDouble();
        xData[i] = xOrigin + i * xInc;
        yData[i] = (data[i]).toDouble();
        //qDebug() << i << "," << xData[i] << "," << yData[i];
    }

    sprintf(cmd,":RUN\n"); SendCmd(cmd);

    xMax = GetMax(xData);
    xMin = GetMin(xData);
    yMax = GetMax(yData);
    yMin = GetMin(yData);

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
