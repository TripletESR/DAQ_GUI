#include "dmm.h"

DMM::DMM(ViRsrc name):
  QSCPI(name)
{

    if( sta == VI_SUCCESS){
        this->name = GetName();
        Msg.sprintf("Instrument identification string: %s",  this->name.toStdString().c_str());
        qDebug() << Msg;
    }else{
        Msg.sprintf("Cannot Open : %s ", this->name.toStdString().c_str());
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


