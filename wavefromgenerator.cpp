#include "wavefromgenerator.h"

WaveFromGenerator::WaveFromGenerator(ViRsrc name): QSCPI(name)
{
    this->name = GetName();

    if( status == VI_SUCCESS){
        openFlag = 1;
        qDebug() << "Instrument identification string:\n \t" <<  this->name;
    }else{
        openFlag = 0;
        qDebug() << "Cannot open " << name;
        this->name = "Wave From Generator.";
    }
}

WaveFromGenerator::~WaveFromGenerator(){
}


void WaveFromGenerator::OpenCh(int ch){
    if( status != VI_SUCCESS) return;
    sprintf(cmd, "output%d ON\n", ch); SendCmd(cmd);
}

void WaveFromGenerator::CloseCh(int ch)
{
    if( status != VI_SUCCESS) return;
    sprintf(cmd, "output%d Off\n", ch); SendCmd(cmd);
}

void WaveFromGenerator::SetWaveForm(int ch, int wf_id) {

    if( status != VI_SUCCESS) return;

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

    if( wf_id == 1) this->index = 0;
    if( wf_id == 8) this->index = 1;
    if( wf_id == 7) this->index = 2;

}


void WaveFromGenerator::SetAmp(int ch, double amp)
{
    if( status != VI_SUCCESS) return;
    this->amp = amp;
    sprintf(cmd, "source%d:voltage %f\n", ch, amp);
    SendCmd(cmd);
}

void WaveFromGenerator::SetFreq(int ch, double freq)
{
    if( status != VI_SUCCESS) return;
    this->freq = freq;
    sprintf(cmd, "source%d:frequency %f\n", ch, freq);
    SendCmd(cmd);
}

void WaveFromGenerator::SetOffset(int ch, double offset)
{
    if( status != VI_SUCCESS) return;
    if( std::abs(offset) > 5) {
        qDebug() << "DC offset must be smaller than +- 5 V !\n";
    }else{
        this->offset = offset;
        sprintf(cmd, "source%d:voltage:offset %f\n", ch, offset);
        SendCmd(cmd);
     }

}

void WaveFromGenerator::GoToOffset(int ch, double offset)
{
    double presentDC = GetOffset(ch)*1000 ; //mV
    double incr; //mV

    offset = offset*1000; //mV

    scpi_Msg.sprintf("Going to the offset: %f mV, from %f mV", offset, presentDC);
    SendMsg(scpi_Msg);
    SendMsg("please wait.");

    // 20 mV per sec.

    if( offset > presentDC){ // increase gentlely
        incr = +20;
    }else if( offset < presentDC){ // reduced gentlely
        incr = -20;
    }

    double diff = offset - presentDC;
    qDebug("diff : %f mV", diff);

    scpi_Msg.sprintf("Estimated wait time : %5.0f sec (20 mV/sec)", diff/fabs(incr));
    SendMsg(scpi_Msg);

    while(diff != 0){
        QEventLoop eventloop;
        QTimer::singleShot(1000, &eventloop, SLOT(quit()));
        eventloop.exec();
        presentDC = presentDC + incr;
        diff = offset - presentDC;
        qDebug("diff : %f mV", diff);
        if( fabs(diff) > fabs(incr)){
            SetOffset(ch, presentDC/1000);
        }else{
            SetOffset(ch, offset/1000);
            diff = 0;
        }
    }
    scpi_Msg.sprintf("Reached offset : %f mV", offset);
    SendMsg(scpi_Msg);
}

void WaveFromGenerator::SetPhase(int ch, double phase)
{
    if( status != VI_SUCCESS) return;
    this->phase = phase;
    sprintf(cmd, "source%d:phase %f\n", ch, phase); // angle:unit = degree
    SendCmd(cmd);
}

int WaveFromGenerator::GetWaveForm(int ch)
{
    if( status != VI_SUCCESS) return -1;
    sprintf(cmd, "source%d:function?\n", ch);
    Ask(cmd);
    //viPrintf(device, cmd);
    //viScanf(device, "%t", buf);

    if( QString(buf) == "SIN") this->index = 0;
    if( QString(buf) == "DC")  this->index = 1;
    if( QString(buf) == "NOIS") this->index = 2;
    //qDebug() << QString(buf) << "," << this->index;

    return this->index;
}

int WaveFromGenerator::GetChIO(int ch)
{
    if( status != VI_SUCCESS) return 0;
    sprintf(cmd, "output%d?\n", ch);
    this->IO = Ask(cmd).toInt();
    return this->IO;
}

double WaveFromGenerator::GetFreq(int ch)
{
    if( status != VI_SUCCESS) return 0;
    sprintf(cmd, "source%d:frequency?\n", ch);
    this->freq = Ask(cmd).toDouble()/1000;
    //qDebug() << "Freq : " << this->freq << " kHz";
    return this->freq;
}

double WaveFromGenerator::GetOffset(int ch)
{
    if( status != VI_SUCCESS) return 0;
    sprintf(cmd, "source%d:voltage:offset?\n", ch);
    this->offset = Ask(cmd).toDouble();
    //qDebug() << "Offset : " << this->offset << " mV";
    return this->offset;
}

double WaveFromGenerator::GetAmp(int ch)
{
    if( status != VI_SUCCESS) return 0;
    sprintf(cmd, "source%d:voltage?\n", ch);
    this->amp = Ask(cmd).toDouble();
    //qDebug() << "Amp : " << this->amp << " mV";
    return this->amp;
}

double WaveFromGenerator::GetPhase(int ch)
{
    if( status != VI_SUCCESS) return 0;
    sprintf(cmd, "source%d:phase?\n", ch);
    this->phase = Ask(cmd).toDouble();
    //qDebug() << "Phase : " << this->phase << " deg";
    return this->phase;
}

void WaveFromGenerator::GetSetting(int ch)
{
    if( status != VI_SUCCESS) return;

    scpi_Msg.sprintf("------- Get WF setting, Ch %d", ch);
    SendMsg(scpi_Msg);
    GetWaveForm(ch);
    GetFreq(ch);
    GetAmp(ch);
    GetOffset(ch);
    GetPhase(ch);
    GetChIO(ch);

}
