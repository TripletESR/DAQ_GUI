#ifndef CONSTANT_H
#define CONSTANT_H

//device address
#define KEYSIGHT33500B "USB0::0x0957::0x2607::MY52202101::0::INSTR"
#define KEYSIGHTDSOX3024T "USB0::0x2A8D::0x1766::MY56181001::0::INSTR"
#define KEITHLEY2000 "GPIB0::4::INSTR"
#define AGILENTINFINIIVISION "TCPIP0::a-dx3024a-50186.rarfadv.riken.go.jp::inst0::INSTR"

// B-Field [mT] = HALL0 + HALL1 * HV + HALL2 * HV^2 + HALL3 * HV^3 + HALL4 * HV^4
// Fit on 2016/10/28
#define HALL0 -4.82 // +- 0.18
#define HALL1 9.449 // +- 0.004
#define HALL2 1e-6
#define HALL3 1e-11
#define HALL4 1e-17

// Laser freqeuncy
#define LASERFREQ 60 // Hz

#endif // CONSTANT_H
