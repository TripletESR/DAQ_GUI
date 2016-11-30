#ifndef CONSTANT_H
#define CONSTANT_H

#include <QString>
#include <QStandardPaths>

//Directory
const QString DESKTOP_PATH = QStandardPaths::locate(QStandardPaths::DesktopLocation, QString(), QStandardPaths::LocateDirectory);
const QString HOME_PATH = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
const QString DATA_PATH = HOME_PATH + "ESR_Data_201610/";
const QString LOG_PATH = DATA_PATH + "DAQ_Log";

//device address
#define KEYSIGHT33500B "USB0::0x0957::0x2607::MY52202101::0::INSTR"
#define KEYSIGHTDSOX3024T "USB0::0x2A8D::0x1766::MY56181001::0::INSTR"
#define KEITHLEY2000 "GPIB0::4::INSTR"
#define AGILENTINFINIIVISION "TCPIP0::a-dx3024a-50186.rarfadv.riken.go.jp::inst0::INSTR"

// B-Field [mT] = HALL0 + HALL1 * HV + HALL2 * HV^2 + HALL3 * HV^3 + HALL4 * HV^4
// Fit on 2016/10/28
#define HALL0 -2.3732
#define HALL1 9.71921
#define HALL2 -0.00350328
#define HALL3 5.32829e-6
#define HALL4 0

// Laser freqeuncy
#define LASERFREQ 60 // Hz

#endif // CONSTANT_H
