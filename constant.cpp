#include "constant.h"

const QString DESKTOP_PATH = QStandardPaths::locate(QStandardPaths::DesktopLocation, QString(), QStandardPaths::LocateDirectory);
const QString HOME_PATH = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
//const QString CONFIG_PATH = DESKTOP_PATH + "AnalysisProgram.ini";

QString DATA_PATH;
QString HALL_DIR_PATH;
QString HALL_PATH;
QString LOG_PATH;
QString DB_PATH;

QString OSCILLOSCOPE ;
QString WAVEFROM_GENERATOR;
QString DIGITALMETER ;
