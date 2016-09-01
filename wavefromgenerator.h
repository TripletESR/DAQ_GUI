#ifndef WAVEFROMGENERATOR_H
#define WAVEFROMGENERATOR_H

#include "scpi.h"

class WaveFromGenerator : public SCPI{
public:
    char unit[5];
    double freq, amp, offset, phase;
    int ch, wf, IO; //wf is the index in WFG_Dialog.h

    WaveFromGenerator(ViRsrc name);
    ~WaveFromGenerator();

    void OpenCh(int ch);
    void CloseCh(int ch);

    void SetWaveForm(int ch, int wf_id);
    void SetAmp(int ch, double amp);
    void SetFreq(int ch, double freq);
    void SetOffset(int ch, double offset);
    void SetPhase(int ch, double phase);

    int GetWaveForm(int ch);
    int GetChIO(int ch);
    double GetFreq(int ch);
    double GetOffset(int ch);
    double GetAmp(int ch);
    double GetPhase(int ch);

    void GetSetting(int ch);

private:

};

#endif // WAVEFROMGENERATOR_H
