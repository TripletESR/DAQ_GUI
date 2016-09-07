#include "fileio.h"


FileIO::FileIO(QString dir, QString name, int mode)
{
    fileName = name;
    fileDir  = dir;
    filePath = dir.append("/").append(name);

    myfile = new QFile();
    QDir::setCurrent(dir);
    myfile->setFileName(name);

    switch (mode) {
    case 1:isOpen = myfile->open(QIODevice::ReadOnly | QIODevice::Text);break;
    case 2:isOpen = myfile->open(QIODevice::WriteOnly | QIODevice::Text);break;
    case 3:isOpen = myfile->open(QIODevice::ReadWrite | QIODevice::Text);break;
    case 4:isOpen = myfile->open(QIODevice::Append | QIODevice::Text);break;
    }

    if( isOpen ){
        qDebug() << name << " opened.";
    }else{
        qDebug() << "fail to open : " <<name ;
    }

}

FileIO::~FileIO(){
    qDebug() << "Closing file : " << filePath;
    myfile->close();
    delete myfile;
}

void FileIO::SaveLogData(QString Msg){

    //QString path = QCoreApplication::applicationDirPath();
    //path.append("/data.dat");
    //QFile myfile(path);
    myfile->write(Msg.toStdString().c_str());
    myfile->write("\n");
}
