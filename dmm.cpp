#include "dmm.h"

DMM::DMM(ViRsrc name):
  QSCPI(name)
{

    if( sta == VI_SUCCESS){
        this->name = GetName();
        Msg.sprintf("Open Device : %s",  this->name.toStdString().c_str());
        qDebug() << Msg;
    }else{
        Msg.sprintf("Cannot Open : %s ", "Keithley 2000 DMM");
        qDebug() << Msg;
        return;
    }
    Clear();
}

DMM::~DMM()
{

}

void DMM::SetMeasureDCV()
{
    sprintf(cmd, ":configure:voltage:DC\n"); SendCmd(cmd);
}

double DMM::GetReading()
{
    sprintf(cmd, ":READ?\n");
    return Ask(cmd).toDouble();
}


