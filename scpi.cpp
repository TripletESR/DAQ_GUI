#include "scpi.h"

SCPI::SCPI(ViRsrc name)
{
    qDebug() << "--------------------------------------";
    viOpenDefaultRM(&defaultRM);
    sta = viOpen(defaultRM, name, VI_NULL, VI_NULL, &device);
//    sta = viOpen(defaultRM, name, VI_NULL, 1000., &device);

    //qDebug() << StatusByteRegister();

}

SCPI::~SCPI(){
    viClose(device);
    viClose(defaultRM);
    qDebug() << "Closing VIAS session for " << this->name;
}

void SCPI::SendCmd(char *cmd){
    if( sta != VI_SUCCESS ) return;
    viPrintf(device, cmd);
    *std::remove(cmd, cmd+strlen(cmd), '\n') = '\0'; //remove "\n"
    qDebug("%s", cmd);
}

QString SCPI::ReadRespond() //Change to AskQ
{
    if( sta != VI_SUCCESS ) return "Err.";
    viScanf(device, "%t", this->buf);
    *std::remove(buf, buf+strlen(buf), '\n') = '\0';
    qDebug("%s", buf);
    return QString(buf);

}

QString SCPI::Ask(char *cmd){
    if( sta != VI_SUCCESS) return "Err.";
    SendCmd(cmd);
    return ReadRespond();
}

void SCPI::Reset(){
    if( sta != VI_SUCCESS ) return;
    viPrintf(device, "*RST\n");
    qDebug() << "Reset : " << this->name;
}

void SCPI::Clear(){
    if( sta != VI_SUCCESS ) return;
    viPrintf(device, "*CLS\n");
    qDebug() << "Clear : " << this->name;
}

QString SCPI::GetName()
{
    if( sta != VI_SUCCESS ) return "";
    sprintf(cmd,"*IDN?\n");
    return Ask(cmd);
}

QString SCPI::GetErrorMsg(){
    if( sta != VI_SUCCESS ) return "Err.";
    sprintf(cmd,"SYST:ERR?\n");
    return Ask(cmd);
}

bool SCPI::isOperationCompleted(){
    if( sta != VI_SUCCESS) return "Err.";
    sprintf(cmd,"*OPC?\n");
    return Ask(cmd).toInt();
}

int SCPI::StatusByteRegister()
{
    ViUInt16 statusByte;
    viReadSTB(device,&statusByte);
    return statusByte;
}

int SCPI::EventStatusRegister()
{
    sprintf(cmd,"*ESR?\n");
    return Ask(cmd).toInt();
}
