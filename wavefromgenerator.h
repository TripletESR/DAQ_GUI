#ifndef WAVEFROMGENERATOR_H
#define WAVEFROMGENERATOR_H

#include "qscpi.h"
#include <QProgressBar>

class WaveFromGenerator : public QSCPI{
public:
    double freq; //in kHz
    double phase;
    double amp, offset; // in mV
    int ch, index, IO; //index is the index in WFG_Dialog.h

    WaveFromGenerator(ViRsrc name);
    ~WaveFromGenerator();

    void OpenCh(int ch);
    void CloseCh(int ch);

    void SetWaveForm(int ch, int wf_id);
    void SetAmp(int ch, double amp);
    void SetFreq(int ch, double freq);
    void SetOffset(int ch, double offset);
    void GoToOffset(int ch, double offset);
    void SetPhase(int ch, double phase);

    int GetWaveForm(int ch);
    int GetChIO(int ch);
    double GetFreq(int ch); // kHz
    double GetOffset(int ch); // mV
    double GetAmp(int ch); // mV
    double GetPhase(int ch); // deg

    void GetSetting(int ch);

private:

};

#endif // WAVEFROMGENERATOR_H
