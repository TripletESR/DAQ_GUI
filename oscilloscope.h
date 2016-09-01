#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "scpi.h"

class Oscilloscope : public SCPI{
public:
    double *xData,*yData;
    char rawData[2000];

    Oscilloscope(ViRsrc name, bool init);
    ~Oscilloscope();

    void Initialize(int ch);

    void SetAcqMode(int mode);
    void SetAverage(int count);

    void GetData(int ch, const int points);


private:

};

#endif // OSCILLOSCOPE_H
