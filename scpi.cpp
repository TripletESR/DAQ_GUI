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
    //qDebug() << "Ask device Error code : " << this->name;
    viPrintf(device, "SYST:ERR?\n");
    ReadRespond();
    return QString(buf);
}

void SCPI::SendCmd(char *cmd){
    if( sta == VI_SUCCESS ){
        viPrintf(device, cmd);
        *std::remove(cmd, cmd+strlen(cmd), '\n') = '\0'; //remove "\n"
        qDebug("%s", cmd);
    }else{
        qDebug() << "Device offline." ;
    }
}

QString SCPI::ReadRespond() //Change to AskQ
{
    if( sta != VI_SUCCESS ) return "Err.";
    //need to ask SBR, if there is something in the output Queue
    viScanf(device, "%t", this->buf);
    *std::remove(buf, buf+strlen(buf), '\n') = '\0';
    qDebug("%s", buf);
    return QString(this->buf);

}

QString SCPI::Ask(char *cmd){
    if( sta != VI_SUCCESS) return "Err.";
    SendCmd(cmd);
    return ReadRespond();
}

bool SCPI::isOperationCompleted(){
    if( sta != VI_SUCCESS) return "Err.";
    viPrintf(device, "*OPC?\n");
    viScanf(device, "%t", this->buf);
    *std::remove(buf, buf+strlen(buf), '\n') = '\0';
    return QString(this->buf).toInt();
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
