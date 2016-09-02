#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "scpi.h"
#include <QVector>

class Oscilloscope : public SCPI{
public:
    QVector<double> xData, yData;
    char rawData[90000];
    int count;
    double yMax, yMin;
    double xMax, xMin;

    double xOrigin, xStep;

    double tRange, tDelay;
    double trgLevel[5]; // index 0 does not use for convienience
    int trgCh, ohm[5], IO[5];
    double xRange[5], xOffset[5];

    Oscilloscope(ViRsrc name, bool init);
    ~Oscilloscope();

    void Initialize(int ch);

    void SetTouch(bool touch);
    void SetPreset();
    void SetTime(double range,double delay);
    void SetVoltage(int ch, bool display, double range, double offset, bool ohm50);
    void SetTrigger(int ch, double level);
    void SetAcqMode(int mode);
    void SetAverage(int count);
    void SetDVM(bool IO, int ch, int mode);

    void GetChannelData(int ch);
    void GetTime();
    //void GetTouch();

    void GetData(int ch, const int points);
    double GetMax(QVector<double> vec);
    double GetMin(QVector<double> vec);

    void GetXOriginStep();

private:

};

#endif // OSCILLOSCOPE_H
