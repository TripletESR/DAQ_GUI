#include "oscilloscope.h"

Oscilloscope::Oscilloscope(ViRsrc name):
    SCPI(name),
    xData(NULL),
    yData(NULL)
{
    xData = new double[500];
    yData = new double[500];
}

Oscilloscope::~Oscilloscope(){
    delete xData;
    delete yData;
}

void Oscilloscope::Initialize(int ch)
{
    sprintf(cmd,"timebase:range 200E-6\n");SendCmd(cmd); // time for 10 div
    sprintf(cmd,"timebase:range;location 0.3\n");SendCmd(cmd);
    sprintf(cmd,"channel%d:Range 4\n",ch); SendCmd(cmd); //vertical full range
    sprintf(cmd,":trigger:sweep normal\n"); SendCmd(cmd);
    sprintf(cmd,":trigger:level 0.1\n"); SendCmd(cmd);
    sprintf(cmd,":trigger:slope positive\n"); SendCmd(cmd);

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
