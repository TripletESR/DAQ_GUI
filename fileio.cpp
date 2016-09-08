#include "fileio.h"


FileIO::FileIO(QString dir, QString name, int mode)
{
    fileName = name;
    fileDir  = dir;
    filePath = dir.append("/").append(name);

    myfile = new QFile(filePath);
//    QDir::setCurrent(dir);
//    myfile->setFileName(name);

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

void FileIO::FileStructure()
{
    myfile->seek(0);
    QTextStream in(myfile);

    row = 0;
    col = 0;

    while(!in.atEnd()){
        QString line = in.readLine();
        QStringList linelist = line.split(',');
        if( row == 0 ){
            col = linelist.size()-1; //col is number of xdata.the 1st column is title
            if(col > 0){
                xdata = QVector<double>(col);
                for(int i = 0; i < col ; i ++){
                    xdata[i] = QString(linelist[i]).toDouble();
                }
            }
        }
        row++;
    }

    endPos = myfile->pos();

    qDebug() << xdata;
    qDebug("(row, col, endPos) = (%d, %d, %d)", row, col, endPos);

}

void FileIO::AppendData(QString head, QVector<double> xdata, QVector<double> zdata)
{

    myfile->seek(endPos);
    QTextStream out(myfile);
    QString outStr;
    QString temp;

    if( endPos == 0 ){
        this->xdata = xdata;
        col = this->xdata.size();
        outStr = "time [us]";
        for(int i = 0; i < col ; i++){
            temp.sprintf(",%12.3e", (this->xdata)[i]);
            outStr.append(temp);
        }
        outStr.append("\n");
        out << outStr;
        outStr.clear();
    }else{

        //check data structure consistancy
        int size = zdata.size();
        if( col != size ) {
            qDebug() << "the size of saving data is difference from existing data sturcture. Abort.";
            qDebug() << col << "," << size ;
            return;
        }
        //compare xdata and this->xdata
        //qDebug() << xdata;
        for(int i = 0; i < size ; i ++ ){
            if( xdata[i] != (this->xdata)[i]){
                qDebug() << xdata[i] << "," << (this->xdata)[i];
                qDebug() << "the x-data of saving data is not matching with existing xdata. Abort.";
                double x1 = (this->xdata)[1];
                double x2 = (this->xdata)[col-1];
                qDebug("(%f, %f) us, Range = %f us, Scale = %f us/div", x1, x2, x2-x1,(x2-x1)/10);
                return;
            }
        }
    }

    //save ydata
    outStr = head;
    for(int i = 0; i < col ; i++){
        temp.sprintf(",%12.3e", zdata[i]);
        outStr.append(temp);
    }
    outStr.append("\n");
    out << outStr;
    out.flush();

    outStr.clear();

    myfile->flush();
    endPos = myfile->pos();
    qDebug() << "saving data" << endPos;

}

void FileIO::SaveLogData(QString Msg){

    //QString path = QCoreApplication::applicationDirPath();
    //path.append("/data.dat");
    //QFile myfile(path);

    //QDateTime date = QDateTime::currentDateTime();
    //Msg.insert(0, ": ").insert(0,date.toString());
    myfile->write(Msg.toStdString().c_str());
    myfile->write("\n");
}
