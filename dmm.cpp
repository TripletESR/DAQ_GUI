#include "dmm.h"

DMM::DMM(ViRsrc name):
  QSCPI(name)
{

    if( status == VI_SUCCESS){
        this->name = GetName();
        Msg.sprintf("Instrument identification string : %s",  this->name.toStdString().c_str());
        qDebug() << Msg;
        openFlag = 1;
    }else{
        Msg.sprintf("Cannot Open : %s ", "Digital Meter");
        qDebug() << Msg;
        openFlag = 0;
        this->name = "Digitial Meter";
        return;
    }
    Clear();
}

DMM::~DMM()
{
    qDebug() << "Closing DMM";
}

void DMM::SetMeasureDCV()
{
    sprintf(cmd, ":configure:voltage:DC\n"); SendCmd(cmd);
}

double DMM::GetReading()
{
    sprintf(cmd, ":READ?\n");
    return fabs(Ask(cmd).toDouble());
}


