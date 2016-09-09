#ifndef QFILEIO_H
#define QFILEIO_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QString>
#include <QDateTime>
#include <QCoreApplication>
#include <QVector>
#include <QDebug>

class QFileIO : public QObject
{
    Q_OBJECT
public:
    QString fileName;
    QString fileDir;
    QString filePath;
    QString Msg;

    QFile * myfile;

    bool isOpen;

    //file structure
    int row, col, endPos;
    QVector<double> xdata;

    explicit QFileIO(QString dir, QString name, int mode,QObject *parent = 0);
    ~QFileIO();

    void FileStructure();
    void AppendData(QString head,QVector<double> xdata, QVector<double> zdata);
    void SaveLogData(QString Msg);

signals:
    void SendMsg(QString msg);

public slots:
};

#endif // QFILEIO_H
