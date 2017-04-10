#ifndef CONSTANT_H
#define CONSTANT_H

#include <QString>
#include <QStandardPaths>

extern const QString DESKTOP_PATH ;
extern const QString HOME_PATH ;
//extern const QString CONFIG_PATH ;

extern QString DATA_PATH ;
extern QString HALL_DIR_PATH ;
extern QString HALL_PATH ;
extern QString LOG_PATH ;
extern QString DB_PATH ;

//Directory
//const QString DESKTOP_PATH = QStandardPaths::locate(QStandardPaths::DesktopLocation, QString(), QStandardPaths::LocateDirectory);
//const QString HOME_PATH = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
//const QString DATA_PATH = HOME_PATH + "ESR_Data_201610/";
//const QString LOG_PATH = DATA_PATH + "DAQ_Log";
//const QString HALL_DIR_PATH = DATA_PATH + "Hall_data";
//const QString HALL_PATH = HALL_DIR_PATH + "/Hall_pars.txt";
//const QString DB_PATH = DATA_PATH + "database.db";

//device address
#define KEYSIGHT33500B "USB0::0x0957::0x2607::MY52202101::0::INSTR" //waveform generator
#define KEYSIGHTDSOX3024T "USB0::0x2A8D::0x1766::MY56181001::0::INSTR" // oscilloscope
#define KEITHLEY2000 "GPIB0::4::INSTR" // digital multimeter
#define AGILENTINFINIIVISION "TCPIP0::a-dx3024a-50186.rarfadv.riken.go.jp::inst0::INSTR" // oscilloscope
#define KEYSIGHT34460A "USB0::0x2A8D::0x1601::MY53102568::0::INSTR" //digital multimeter

//choose which device is which
extern QString OSCILLOSCOPE ;
extern QString WAVEFROM_GENERATOR;
extern QString DIGITALMETER ;
//#define OSCILLOSCOPE KEYSIGHTDSOX3024T
//#define WAVEFROM_GENERATOR KEYSIGHT33500B
//#define DIGITALMETER KEYSIGHT34460A

// B-Field [mT] = HALL0 + HALL1 * HV + HALL2 * HV^2 + HALL3 * HV^3 + HALL4 * HV^4
// Fit on 2016/10/28
#define HALL0 -2.3732
#define HALL1 9.71921
#define HALL2 -0.00350328
#define HALL3 5.32829e-6
#define HALL4 0

//naive version definition
#define VERSION 2.11

#endif // CONSTANT_H
