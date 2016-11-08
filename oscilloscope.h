#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "qscpi.h"
#include <QVector>
#include <windows.h> // for Sleep
#include <QFile>
#include <QTimer>
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

    QVector<double> BGData;
    QVector<double> xData[5], yData[5];
    double yMax, yMin;
    double xMax, xMin;

    int acqCount;
    double trgRate;
    bool lockFlag, touchFlag, acqFlag, logFlag;

    double tRange, tDelay;
    double trgLevel[5]; // index 0 does not use for convienience
    int trgCh, ohm[5], IO[5];
    double vRange[5], vOffset[5];
    bool DVMIO; int DVMCh, DVMMode;

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
    double GetDVM();
    void GetTime();
    double GetTriggerRate();
    int GetAcquireCount();
    void GetSystemStatus();  // lock, touch, Acq Mode
    void GetData(int ch, const int points, bool Save2BG);
    double GetMax(QVector<double> vec);
    double GetMin(QVector<double> vec);

    void Resume(int ch);
    void TranslateRawData(int ch, int points, bool Save2BG);
    double GetMaxWaitTime(){return maxWaitTime;}

signals:
    void NotFinished(double i);

private slots:
    void SyncOSC(int ch, int points, double waitsec, bool Save2BG);


private:
    double timeElapsed;
    double maxWaitTime;
    bool openFlag;
};

#endif // OSCILLOSCOPE_H
