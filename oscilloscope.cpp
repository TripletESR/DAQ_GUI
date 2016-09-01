#include "oscilloscope.h"

Oscilloscope::Oscilloscope(ViRsrc name, bool init):
    SCPI(name, init),
    xData(NULL),
    yData(NULL)
{
    //xData = new double[500];
    //yData = new double[500];
}

Oscilloscope::~Oscilloscope(){
    delete xData;
    delete yData;
}

void Oscilloscope::Initialize(int ch)
{
    SetAcqMode(0);
    sprintf(cmd,"timebase:range 200E-6\n");SendCmd(cmd); // time for 10 div
    sprintf(cmd,"timebase:position 40E-6\n");SendCmd(cmd);
    sprintf(cmd,"channel%d:Range 4\n",ch); SendCmd(cmd); //vertical full range
    sprintf(cmd,":trigger:sweep normal\n"); SendCmd(cmd);
    sprintf(cmd,":trigger:level 0.1\n"); SendCmd(cmd);
    sprintf(cmd,":trigger:slope positive\n"); SendCmd(cmd);

}

void Oscilloscope::SetAcqMode(int mode){
    switch (mode) {
        case 0: sprintf(cmd,":acquire:type normal\n"); SendCmd(cmd); break;
        case 1: sprintf(cmd,":acquire:type average\n"); SendCmd(cmd); break;
    }
}

void Oscilloscope::SetAverage(int count)
{
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

void Oscilloscope::GetData(int ch, const int points)
{


    xData = new double[points];
    yData = new double[points];

    if ( sta == VI_SUCCESS){
        sprintf(cmd,":digitize channel%d\n", ch); SendCmd(cmd);
        sprintf(cmd,":waveform:source channel%d\n", ch); SendCmd(cmd);
        sprintf(cmd,":waveform:format ASCii\n"); SendCmd(cmd);
        sprintf(cmd,":waveform:points %d\n", points); SendCmd(cmd);

        sprintf(cmd,":waveform:data?\n"); SendCmd(cmd);
        viScanf(device, "%t", rawData);
        QString raw = rawData;
        QStringList data = raw.mid(10).split(',');
        qDebug() << data;
        qDebug() << "number of data : " << data.length();

        sprintf(cmd,":waveform:xorigin?\n"); SendCmd(cmd);
        viScanf(device, "%t", buf);
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
            qDebug() << i << "," << xData[i] << "," << yData[i];
        }

        sprintf(cmd,":RUN\n"); SendCmd(cmd);
    }
}
