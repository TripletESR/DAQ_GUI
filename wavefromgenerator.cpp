#include "wavefromgenerator.h"

WaveFromGenerator::WaveFromGenerator(ViRsrc name)
{
    viOpenDefaultRM(&defaultRM);
    viOpen(defaultRM, name, VI_NULL, VI_NULL, &device);

    // Initialize device
    viPrintf(device, "*RST\n");
    // Send an *IDN? string to the device, ask the devices name
    viPrintf(device, "*IDN?\n");
    // Read respond
    viScanf(device, "%t", buf);
    // Print results
    //printf("Instrument identification string:\n     %s\n", buf);
    qDebug() << "Instrument identification string:\n \t" <<  buf;

}

WaveFromGenerator::~WaveFromGenerator(){
    viClose(device);
    viClose(defaultRM);
    //printf("Closing VIAS session for Wave From Generator \n");
    qDebug() << "Closing VIAS session for Wave From Generator \n";
}

void WaveFromGenerator::Reset(){
    viPrintf(device, "*RST\n");
}

void WaveFromGenerator::OpenCh(int ch){
    sprintf(cmd, "output%d ON\n", ch);
    //printf("%s\n", cmd);
    qDebug() << cmd;
    viPrintf(device, cmd);
}

void WaveFromGenerator::SetSinWave(int ch,
                                   double freq,
                                   double amp,
                                   double offset,
                                   double phase,
                                   char unit[]){

    sprintf(cmd, "source%d:function sin\n", ch);
    //printf("%s", cmd);
    qDebug() << cmd;
    viPrintf(device, cmd);

    sprintf(cmd, "source%d:frequency %f\n", ch, freq);
    //printf("%s", cmd);
    qDebug() << cmd;
    viPrintf(device, cmd);

    sprintf(cmd, "source%d:voltage:unit %s\n", ch, unit);
    //printf("%s", cmd);
    qDebug() << cmd;
    viPrintf(device, cmd);

    sprintf(cmd, "source%d:voltage %f\n", ch, amp/2.);
    //printf("%s", cmd);
    qDebug() << cmd;
    viPrintf(device, cmd);

    sprintf(cmd, "source%d:voltage:offset %f\n", ch, offset);
    //printf("%s", cmd);
    qDebug() << cmd;
    viPrintf(device, cmd);

    sprintf(cmd, "source%d:phase %f\n", ch, phase); // angle:unit = degree
    //printf("%s", cmd);
    qDebug() << cmd;
    viPrintf(device, cmd);

}


void WaveFromGenerator::SetDC(int ch, double offset)
{
    if( std::abs(offset) > 5) {
        //printf("DC offset must be smaller than +- 5 V !\n");
        qDebug() << "DC offset must be smaller than +- 5 V !\n";
    }else{
        sprintf(cmd, "source%d:function DC\n", ch);
        printf("%s", cmd);
        viPrintf(device, cmd);

        sprintf(cmd, "source%d:voltage:offset %f\n", ch, offset);
        printf("%s", cmd);
        viPrintf(device, cmd);
    }
}

void WaveFromGenerator::GetSetting(int ch)
{

}

double WaveFromGenerator::GetFreq(int ch)
{
    sprintf(cmd, "source%d:frequency?\n", ch);
    viPrintf(device, cmd);
    viScanf(device, "%t", buf);
    freq = atof(buf);
    return freq;
}

