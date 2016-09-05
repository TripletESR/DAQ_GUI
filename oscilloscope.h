#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "scpi.h"
#include <QVector>

class Oscilloscope : public SCPI{
public:
    QVector<double> xData[5], yData[5];
    int count;
    double yMax, yMin;
    double xMax, xMin;

    bool lockFlag, touchFlag, acqFlag, logFlag;

    double tRange, tDelay;
    double trgLevel[5]; // index 0 does not use for convienience
    int trgCh, ohm[5], IO[5];
    double xRange[5], xOffset[5];

    Oscilloscope(ViRsrc name, bool init);
    ~Oscilloscope();

    void Initialize(int ch);

    void SetRemoteLog(bool log);
    void SetTouch(bool touch);
    void SetPreset();
    void SystemLock(bool lock);
    void SetTime(double range,double delay);
    void OpenCh(int ch, bool display);
    void SetVoltage(int ch, double range, double offset, bool ohm50);
    void SetTrigger(int ch);
    void SetTriggerLevel(int ch, double level);
    void SetAcqMode(int mode);
    void SetAverage(int count);
    void SetDVM(bool IO, int ch, int mode);

    void GetChannelData(int ch);
    void GetTime();
    void GetSystemStatus();  // lock, touch, Acq Mode

    void GetData(int ch, const int points);
    double GetMax(QVector<double> vec);
    double GetMin(QVector<double> vec);

    void SaveData(QString filename);

private:

};

#endif // OSCILLOSCOPE_H
