#include "qfileio.h"

QFileIO::QFileIO(QString dir, QString name, int mode, QObject *parent) : QObject(parent)
{
    fileName = name;
    fileDir  = dir;
    filePath = dir.append("/").append(name);

    myfile = new QFile(filePath);

    switch (mode) {
    case 1:isOpen = myfile->open(QIODevice::ReadOnly | QIODevice::Text);break;
    case 2:isOpen = myfile->open(QIODevice::WriteOnly | QIODevice::Text);break;
    case 3:isOpen = myfile->open(QIODevice::ReadWrite | QIODevice::Text);break;
    case 4:isOpen = myfile->open(QIODevice::Append | QIODevice::Text);break;
    }

    if( isOpen ){
        Msg = "Opened : ";
        Msg += filePath;
    }else{
        Msg = "Fail to open";
        Msg += filePath;
    }
}

QFileIO::~QFileIO(){
    myfile->close();
    delete myfile;
}

void QFileIO::FileStructure()
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
                    xdata[i] = QString(linelist[i+1]).toDouble();
                }
            }
        }
        row++;
    }

    endPos = myfile->pos();

    //qDebug() << xdata;
    if( col > 0 ){
        double x1 = (this->xdata)[0];
        double x2 = (this->xdata)[col-1];
        double xStep = (x2-x1)/(col-1);
        double range = x2 - x1 + xStep;
        SendMsg("=========== Existing x-data");
        SendMsg(filePath);
        Msg.sprintf("(%f, %f) us", x1, x2);                                  SendMsg(Msg);
        Msg.sprintf("Range  = %f us", range);                                SendMsg(Msg);
        Msg.sprintf("Scale  = %f us/div", range/10);                         SendMsg(Msg);
        Msg.sprintf("Offset = %f us", x1 + range/2);                         SendMsg(Msg);
        Msg.sprintf("(row, col, endPos) = (%d, %d, %d)", row, col, endPos);  SendMsg(Msg);
    }else{
        SendMsg("Openned an empty File.");
        SendMsg(filePath);
    }
}

void QFileIO::AppendData(QString head, QVector<double> xdata, QVector<double> zdata)
{

    myfile->seek(endPos);
    QTextStream out(myfile);
    QString outStr;
    QString temp;

    if( endPos == 0 ){
        this->xdata = xdata;
        col = this->xdata.size();
        outStr.sprintf("%30s","time [us]");
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
            SendMsg("The size of input data is difference from existing data sturcture. Abort.");
            Msg.sprintf("Existing col : %d", col ); SendMsg(Msg);
            Msg.sprintf(" Input col   : %d", size); SendMsg(Msg);
            return;
        }
        //compare xdata and this->xdata
        //qDebug() << xdata;
        double x1 = xdata[0];
        double x2 = xdata[col-1];
        double xStep = (x2-x1)/(col-1);
        double range = x2 - x1 + xStep;

        double qx1 = this->xdata[0];
        double qx2 = this->xdata[col-1];
        double qxStep = (qx2-qx1)/(col-1);
        //double qrange = qx2 - qx1 + qxStep;

        double torr = 1e-9;
        if( fabs(x1- qx1) > torr) qDebug() << "x1:" << x1 << ", " << qx1 ;
        if( fabs(x1- qx1) > torr) qDebug() << "x2:" << x2 << ", " << qx2 ;
        if( fabs(xStep- qxStep) > torr) qDebug() << "xStep:" << xStep << ", " << qxStep ;

        if( fabs(x1- qx1) > torr || fabs(x1- qx1) > torr || fabs(xStep- qxStep) > torr){
            SendMsg("The x-data of input data is not matching with existing xdata. Abort.");
            SendMsg("======== Input x-data structure.");
            Msg.sprintf("(%f, %f) us", x1, x2);          SendMsg(Msg);
            Msg.sprintf("Range  = %f us", range);        SendMsg(Msg);
            Msg.sprintf("Scale  = %f us/div", range/10); SendMsg(Msg);
            Msg.sprintf("Offset = %f us", x1 + range/2); SendMsg(Msg);
            return;
        }

        /*for(int i = 0; i < size ; i ++ ){
            if( xdata[i] != (this->xdata)[i]){
                SendMsg("The x-data of input data is not matching with existing xdata. Abort.");
                Msg.sprintf("Difference at %d , input x-data = %f, existing x-data = %f", i, xdata[i], (this->xdata)[i]);
                SendMsg(Msg);
                SendMsg("======== Input x-data structure.");
                double x1 = xdata[0];
                double x2 = xdata[col-1];
                double xStep = (x2-x1)/(col-1);
                double range = x2 - x1 + xStep;
                Msg.sprintf("(%f, %f) us", x1, x2);          SendMsg(Msg);
                Msg.sprintf("Range  = %f us", range);        SendMsg(Msg);
                Msg.sprintf("Scale  = %f us/div", range/10); SendMsg(Msg);
                Msg.sprintf("Offset = %f us", x1 + range/2); SendMsg(Msg);
                return;
            }
        }*/
    }

    //save ydata
    outStr.sprintf("%30s", head.toStdString().c_str());
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
    Msg = "===== Data saved. Data Name : ";
    Msg += head;
    SendMsg(Msg);

}

void QFileIO::SaveLogData(QString Msg){

    myfile->write(Msg.toStdString().c_str());
    myfile->write("\n");
    myfile->flush();
}
