#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "scpi.h"

class Oscilloscope : public SCPI{
public:
    double *xData,*yData;
    //char cmd[200];

    Oscilloscope(ViRsrc name);
    ~Oscilloscope();

    void Initialize(int ch);
    void SetAverage(int count);

    //void Read(int ch);


private:

};

#endif // OSCILLOSCOPE_H
