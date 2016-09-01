#include "scpi.h"

SCPI::SCPI(ViRsrc name)
{
    viOpenDefaultRM(&defaultRM);
    ViStatus sta = viOpen(defaultRM, name, VI_NULL, VI_NULL, &device);

    // Initialize device
    viPrintf(device, "*RST\n");

    // Send an *IDN? string to the device, ask the devices name
    viPrintf(device, "*IDN?\n");
    // Read respond
    viScanf(device, "%t", this->name);
    *std::remove(this->name, this->name+strlen(this->name), '\n') = '\0';
    // Print results
    //printf("Instrument identification string:\n     %s\n", buf);
    if( sta == VI_SUCCESS ){
        qDebug() << "Instrument identification string:\n \t" <<  this->name;
    }else{
        qDebug() << "Cannot open " << this->name;
    }

}

SCPI::~SCPI(){
    viClose(device);
    viClose(defaultRM);
    qDebug() << "Closing VIAS session for " << this->name;
}

void SCPI::Reset()
{
    viPrintf(device, "*RST\n");
    qDebug() << "Reset : " << this->name;
}

void SCPI::Clear()
{
    viPrintf(device, "*CLS\n");
    qDebug() << "Clear : " << this->name;
}

QString SCPI::GetError()
{
    qDebug() << "Ask device Error code : " << this->name;
    viPrintf(device, "SYST:ERR?\n");
    ReadRespond();
    return buf;
}

void SCPI::SendCmd(char *cmd)
{
    viPrintf(device, cmd);
    *std::remove(cmd, cmd+strlen(cmd), '\n') = '\0'; //remove "\n"
    qDebug().noquote() << cmd;
}

QString SCPI::ReadRespond()
{
    viScanf(device, "%t", this->buf);
    *std::remove(buf, buf+strlen(buf), '\n') = '\0';
    qDebug() << buf;
    return QString(this->buf);
}
