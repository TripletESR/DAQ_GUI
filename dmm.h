#ifndef DMM_H
#define DMM_H

#include "qscpi.h"
#include <QString>
#include <QDebug>

class DMM : public QSCPI
{
public:
    DMM(ViRsrc name);
    ~DMM();

    QString Msg;

public slots:
    void SetMeasureDCV();
    double GetReading();
    bool IsOpen() {return openFlag;}

private:

    bool openFlag;
};

#endif // DMM_H
