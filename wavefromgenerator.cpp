#include "wavefromgenerator.h"

WaveFromGenerator::WaveFromGenerator(ViRsrc name, bool init):
    SCPI(name, init)
{
}

WaveFromGenerator::~WaveFromGenerator(){
}

void WaveFromGenerator::OpenCh(int ch){
    if( sta != VI_SUCCESS) return;
    sprintf(cmd, "output%d ON\n", ch); SendCmd(cmd);
}

void WaveFromGenerator::CloseCh(int ch)
{
    if( sta != VI_SUCCESS) return;
    sprintf(cmd, "output%d Off\n", ch); SendCmd(cmd);
}

void WaveFromGenerator::SetWaveForm(int ch, int wf_id) {

    if( sta != VI_SUCCESS) return;

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
    if( sta != VI_SUCCESS) return;
    this->amp = amp;
    sprintf(cmd, "source%d:voltage %f\n", ch, amp);
    SendCmd(cmd);
}

void WaveFromGenerator::SetFreq(int ch, double freq)
{
    if( sta != VI_SUCCESS) return;
    this->freq = freq;
    sprintf(cmd, "source%d:frequency %f\n", ch, freq);
    SendCmd(cmd);
}

void WaveFromGenerator::SetOffset(int ch, double offset)
{
    if( sta != VI_SUCCESS) return;
    if( std::abs(offset) > 5) {
        qDebug() << "DC offset must be smaller than +- 5 V !\n";
    }else{
        this->offset = offset;
        sprintf(cmd, "source%d:voltage:offset %f\n", ch, offset);
        SendCmd(cmd);
     }

}

void WaveFromGenerator::SetPhase(int ch, double phase)
{
    if( sta != VI_SUCCESS) return;
    this->phase = phase;
    sprintf(cmd, "source%d:phase %f\n", ch, phase); // angle:unit = degree
    SendCmd(cmd);
}

int WaveFromGenerator::GetWaveForm(int ch)
{
    if( sta != VI_SUCCESS) return -1;
    sprintf(cmd, "source%d:function?\n", ch);
    Ask(cmd);
    //viPrintf(device, cmd);
    //viScanf(device, "%t", buf);

    if( QString(buf) == "SIN") this->index = 0;
    if( QString(buf) == "DC")  this->index = 1;
    if( QString(buf) == "NOIS") this->index = 2;


    qDebug() << QString(buf) << "," << this->index;

    return this->index;
}

int WaveFromGenerator::GetChIO(int ch)
{
    if( sta != VI_SUCCESS) return 0;
    sprintf(cmd, "output%d?\n", ch);
    viPrintf(device, cmd);
    viScanf(device, "%t", buf);
    this->IO = atoi(buf);
    qDebug() << "On/OFF : " << this->IO;
    return this->IO;
}

double WaveFromGenerator::GetFreq(int ch)
{
    if( sta != VI_SUCCESS) return 0;
    sprintf(cmd, "source%d:frequency?\n", ch);
    this->freq = Ask(cmd).toDouble()/1000;
    qDebug() << "Freq : " << this->freq << " kHz";
    return this->freq;
}

double WaveFromGenerator::GetOffset(int ch)
{
    if( sta != VI_SUCCESS) return 0;
    sprintf(cmd, "source%d:voltage:offset?\n", ch);
    this->offset = Ask(cmd).toDouble()*1000;
    qDebug() << "Offset : " << this->offset << " mV";
    return this->offset;
}

double WaveFromGenerator::GetAmp(int ch)
{
    if( sta != VI_SUCCESS) return 0;
    sprintf(cmd, "source%d:voltage?\n", ch);
    this->amp = Ask(cmd).toDouble()*1000;
    qDebug() << "Amp : " << this->amp << " mV";
    return this->amp;
}

double WaveFromGenerator::GetPhase(int ch)
{
    if( sta != VI_SUCCESS) return 0;
    sprintf(cmd, "source%d:phase?\n", ch);
    this->phase = Ask(cmd).toDouble();
    qDebug() << "Phase : " << this->phase << " deg";
    return this->phase;
}

void WaveFromGenerator::GetSetting(int ch)
{
    if( sta != VI_SUCCESS) return;

    qDebug() << "---- Get WF setting ----";
    GetWaveForm(ch);
    GetFreq(ch);
    GetAmp(ch);
    GetOffset(ch);
    GetPhase(ch);
    GetChIO(ch);

}

