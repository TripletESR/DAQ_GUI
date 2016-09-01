#include "wavefromgenerator.h"

WaveFromGenerator::WaveFromGenerator(ViRsrc name):
    SCPI(name)
{
}

WaveFromGenerator::~WaveFromGenerator(){
}

void WaveFromGenerator::OpenCh(int ch){
    sprintf(cmd, "output%d ON\n", ch);
    //printf("%s\n", cmd);
    SendCmd(cmd);
}

void WaveFromGenerator::CloseCh(int ch)
{
    sprintf(cmd, "output%d Off\n", ch);
    //printf("%s\n", cmd);
    SendCmd(cmd);
}

void WaveFromGenerator::SetWaveForm(int ch, int wf_id) {

    if (ch != 1 && ch != 2) {
        printf(" ch must be either 1 or 2 !! \n");
    }

    switch (wf_id) {
        case 1: sprintf(cmd,"source%d:function sin\n", ch); break;
        case 2: sprintf(cmd,"source%d:function square\n", ch); break;
        case 3: sprintf(cmd,"source%d:function triangle\n", ch); break;
        case 4: sprintf(cmd,"source%d:function ramp\n", ch); break;
        case 5: sprintf(cmd,"source%d:function pulse\n", ch); break;
        case 6: sprintf(cmd,"source%d:function PRBS\n", ch); break; // pseudo-random binary sequence
        case 7: sprintf(cmd,"source%d:function noise\n", ch); break;
        case 8: sprintf(cmd,"source%d:function DC\n", ch); break;
    }

    SendCmd(cmd);

}


void WaveFromGenerator::SetAmp(int ch, double amp)
{
    this->amp = amp;
    sprintf(cmd, "source%d:voltage %f\n", ch, amp);
    SendCmd(cmd);
}

void WaveFromGenerator::SetFreq(int ch, double freq)
{
    this->freq = freq;
    sprintf(cmd, "source%d:frequency %f\n", ch, freq);
    SendCmd(cmd);
}

void WaveFromGenerator::SetOffset(int ch, double offset)
{

    if( std::abs(offset) > 5) {
        //printf("DC offset must be smaller than +- 5 V !\n");
        qDebug() << "DC offset must be smaller than +- 5 V !\n";
    }else{
        this->offset = offset;
        sprintf(cmd, "source%d:voltage:offset %f\n", ch, offset);
        SendCmd(cmd);
     }

}

void WaveFromGenerator::SetPhase(int ch, double phase)
{
    this->phase = phase;
    sprintf(cmd, "source%d:phase %f\n", ch, phase); // angle:unit = degree
    SendCmd(cmd);
}

int WaveFromGenerator::GetWaveForm(int ch)
{
    sprintf(cmd, "source%d:function?\n", ch);
    viPrintf(device, cmd);
    viScanf(device, "%t", buf);

    //qDebug() << QString(buf);

    if( QString(buf) == "SIN\n") wf = 0;
    if( QString(buf) == "DC\n") wf = 1;
    if( QString(buf) == "NOIS\n") wf = 2;

    return wf;
}

int WaveFromGenerator::GetChIO(int ch)
{
    sprintf(cmd, "output%d?\n", ch);
    viPrintf(device, cmd);
    viScanf(device, "%t", buf);
    this->IO = atoi(buf);
    qDebug() << "On/OFF : " << this->IO;
    return this->IO;
}

double WaveFromGenerator::GetFreq(int ch)
{
    sprintf(cmd, "source%d:frequency?\n", ch);
    viPrintf(device, cmd);
    viScanf(device, "%t", buf);
    this->freq = atof(buf);
    qDebug() << "Freq : " << this->freq;
    return this->freq;
}

double WaveFromGenerator::GetOffset(int ch)
{
    sprintf(cmd, "source%d:offset?\n", ch);
    viPrintf(device, cmd);
    viScanf(device, "%t", buf);
    offset = atof(buf);
    qDebug() << "Offset : " << offset;
    return offset;
}

double WaveFromGenerator::GetAmp(int ch)
{
    sprintf(cmd, "source%d:voltage?\n", ch);
    viPrintf(device, cmd);
    viScanf(device, "%t", buf);
    amp = atof(buf);
    qDebug() << "Amp : " << amp;
    return amp;
}

double WaveFromGenerator::GetPhase(int ch)
{
    sprintf(cmd, "source%d:phase?\n", ch);
    viPrintf(device, cmd);
    viScanf(device, "%t", buf);
    phase = atof(buf);
    qDebug() << "Phase : " << phase;
    return phase;
}

void WaveFromGenerator::GetSetting(int ch)
{
    qDebug() << "---- Get WF setting ----";
    //this->wf = GetWaveForm(ch);
    this->freq = GetFreq(ch);
    this->amp = GetAmp(ch);
    this->offset = GetOffset(ch);
    this->phase = GetPhase(ch);
    this->IO = GetChIO(ch);

}

