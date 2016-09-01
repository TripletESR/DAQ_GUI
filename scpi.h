#ifndef SCPI_H
#define SCPI_H

#include <stdio.h>
#include <cmath>
#include "C:\Program Files (x86)\IVI Foundation\VISA\WinNT\Include\visa.h"
#include <QDebug>

class SCPI
{
public:
    char cmd[100], buf[256], name[100];

    SCPI(ViRsrc name);
    ~SCPI();

    void Reset();
    void Clear();
    QString GetError();
    void SendCmd(char *cmd);
    QString ReadRespond();

protected:
    ViSession defaultRM;
    ViSession device;
};

#endif // SCPI_H
