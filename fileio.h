/*******************************
for general file save, read, and append.
There are mainly 2 kind of files need to access
1) log file
2) data file

the log file only need to append

the data file, when opened,
    i) analysis the file structure,
    ii) save the xdata
since many zdata will be stored, append mehtod can save time.
*********************************/

#ifndef FILEIO_H
#define FILEIO_H

#include <QFile>
#include <QDir>
#include <QString>
#include <QDateTime>
#include <QCoreApplication>
#include <QVector>
#include <QDebug>

class FileIO
{
public:
    QString fileName;
    QString fileDir;
    QString filePath;

    QFile * myfile;

    bool isOpen;

    //file structure
    int row, col;


    FileIO(QString dir, QString name, int mode);
    ~FileIO();

    void FileStructure();

    void AppendData(QString head,
                    QVector<double> xdata,
                    QVector<double> zdata);

    void SaveLogData(QString Msg);

};

#endif // FILEIO_H
