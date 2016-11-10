#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "qscpi.h"
#include <QVector>
#include <windows.h> // for Sleep
#include <QFile>
#include <QTimer>
#include <QEventLoop>
#include <QProgressDialog>
#include "constant.h"
#include <qmath.h>

class Oscilloscope : public QSCPI{

public:

    //status Byte Register
    /*** Table 77
     0 = trigger
     1 = user event
     2 = Message , usually error msg
     3 = no used, always 0
     4 = message in output Queue, after send an question
     5 = event status bit, *ESR?
     6 = Request Services or Master Summary Status
     7 = Operation Status Register
    ***/

    Oscilloscope(ViRsrc name);
    ~Oscilloscope();

    void Initialize();
    bool IsOpen(){return openFlag;}
    void SetRemoteLog(bool log);
    void SetTouch(bool touch);
    void SetPreset();
    void SystemLock(bool lock);
    void SetTime(double range,double delay);
    void OpenCh(int ch, bool display);
    void SetVoltage(int ch, double range, double offset, bool ohm1M);
    void SetTrigger(int ch);
    void SetTriggerLevel(int ch, double level);
    void SetAcqMode(int mode);
    void SetAverage(int count);
    void SetDVM(bool IO, int ch, int mode);

    void GetChannelData(int ch);
    void GetTime();
    void GetSystemStatus();  // lock, touch, Acq Mode

    double GetDVMValue();
    int GetAcquireCount();
    double GetTriggerRate();
    int GetTriggerChannel();

    void GetData(int ch, const int points, bool Save2BG);
    void Resume(int ch);
    void TranslateRawData(int ch, int points, bool Save2BG);
    void SyncOSC(int ch, int points, double waitsec, bool Save2BG);

    double CalMax(QVector<double> vec);
    double CalMin(QVector<double> vec);

    QVector<double> BGData;
    QVector<double> xData[5], yData[5];
    double yMax, yMin;
    double xMax, xMin;

    bool lockFlag, touchFlag, acqFlag, logFlag;

    double tRange, tDelay;
    int acqCount;
    double trgRate;
    double trgLevel[5]; // index 0 does not use for convienience
    int trgCh, ohm[5], IO[5];
    double vRange[5], vOffset[5];
    bool DVMIO; int DVMCh, DVMMode;

    double GetMaxWaitTime(){return maxWaitTime;}

private:

    double timeElapsed;
    double maxWaitTime;
    bool openFlag;
};

#endif // OSCILLOSCOPE_H
