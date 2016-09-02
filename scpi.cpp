#include "scpi.h"

SCPI::SCPI(ViRsrc name, bool init)
{
    viOpenDefaultRM(&defaultRM);
    sta = viOpen(defaultRM, name, VI_NULL, VI_NULL, &device);

    if( sta == VI_SUCCESS ){
        // Initialize device
        if ( init ) viPrintf(device, "*RST\n");
        // Send an *IDN? string to the device, ask the devices name
        viPrintf(device, "*IDN?\n");
        // Read respond
        viScanf(device, "%t", this->name);
        *std::remove(this->name, this->name+strlen(this->name), '\n') = '\0';
        // Print results
        //printf("Instrument identification string:\n     %s\n", buf);
        qDebug() << "Instrument identification string:\n \t" <<  this->name;
    }else{
        qDebug() << "Cannot open " << name;
    }

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

QString SCPI::GetError(){
    if( sta != VI_SUCCESS ) return "Err.";
    qDebug() << "Ask device Error code : " << this->name;
    viPrintf(device, "SYST:ERR?\n");
    ReadRespond();
    return buf;
}

void SCPI::SendCmd(char *cmd){
    if( sta == VI_SUCCESS ){
        viPrintf(device, cmd);
        *std::remove(cmd, cmd+strlen(cmd), '\n') = '\0'; //remove "\n"
        qDebug().noquote() << cmd;
    }else{
        qDebug() << "Device offline." ;
    }
}

QString SCPI::ReadRespond() //Change to AskQ
{
    if( sta != VI_SUCCESS ) return "Err.";
    viScanf(device, "%t", this->buf);
    *std::remove(buf, buf+strlen(buf), '\n') = '\0';
    qDebug() << buf;
    return QString(this->buf);

}

QString SCPI::Ask(char *cmd){
    if( sta != VI_SUCCESS) return "Err.";
    SendCmd(cmd);
    return ReadRespond();
}
