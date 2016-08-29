#ifndef WAVEFROMGENERATOR_H
#define WAVEFROMGENERATOR_H

#include <stdio.h>
#include <cmath>
#include "C:\Program Files (x86)\IVI Foundation\VISA\WinNT\Include\visa.h"
#include <QDebug>

class WaveFromGenerator
{
public:
    char cmd[100], wf[10], unit[5];
    double freq, amp, offset, phase;
    int ch;

    WaveFromGenerator(ViRsrc name);
    ~WaveFromGenerator();

    void Reset();
    void OpenCh(int ch);

    void SetSinWave(int ch, double freq, double amp, double offset, double phase, char unit[5] = "Vpp");
    void SetDC(int ch, double offset);
    void SetCustomWF(int ch, QString filenam);

    void GetSetting(int ch);
    double GetFreq(int ch);
    double GetOffset(int ch);
    double GetAmp(int ch);

private:
    ViSession defaultRM;
    ViSession device;
    //ViRsrc name;

    char buf[256];

};

#endif // WAVEFROMGENERATOR_H
