#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    wfgui(NULL),
    oscui(NULL),
    customPlot(NULL),
    logFile(NULL),
    dataFile(NULL)
{
    MsgCount = 0;
    ui->setupUi(this);
    wfgui = new WFG_Dialog(this);
    LogMsg = wfgui->Msg;
    Log(LogMsg);
    oscui = new osc_Dialog(this);
    LogMsg = oscui->Msg;
    Log(LogMsg);

    //Setting up connectting
    connect(oscui, SIGNAL(osc_LogMsg(QString)), this, SLOT(Log(QString)));
    connect(wfgui, SIGNAL(wfg_LogMsg(QString)), this, SLOT(Log(QString)));

    //Get Setting
    wfgui->on_comboBox_ch_activated(1);
    wfgui->on_comboBox_ch_activated(0);

    oscui->osc->SetRemoteLog(1);
    oscui->on_checkBox_Lock_clicked(1); //get osc status

}

MainWindow::~MainWindow()
{
    delete customPlot;

    delete ui;
    delete wfgui;
    delete oscui;

    delete logFile;
    delete dataFile;

}

void MainWindow::Log(QString logMsg) //TODO not finished
{
    MsgCount ++;

    QDateTime date = QDateTime::currentDateTime();
    QString countStr;
    countStr.sprintf(" [%04d]:    ", MsgCount);
    logMsg.insert(0,countStr).insert(0,date.toString());

    if( logFile != NULL){
        logFile->SaveLogData(logMsg);
    }
    ui->plainTextEdit->appendPlainText(logMsg);
    qDebug() << logMsg;

}


void MainWindow::on_actionWave_From_Generator_triggered()
{
    if( wfgui->isHidden()){
        wfgui->setGeometry(this->geometry().x()+ this->geometry().width()+20,
                           this->geometry().y(),
                           wfgui->geometry().width(),
                           wfgui->geometry().height());
        wfgui->show();
    }
}

void MainWindow::on_actionOscilloscope_triggered()
{
    if( oscui->isHidden()){
        oscui->setGeometry(this->geometry().x()+ this->geometry().width()+20,
                           wfgui->geometry().y()+ wfgui->geometry().height()+60,
                           oscui->geometry().width(),
                           oscui->geometry().height());
        oscui->show();
        Log(oscui->Msg);
    }
}

void MainWindow::on_pushButton_clicked()
{
    customPlot = ui->customPlot;

    int ch = ui->spinBox_ch->value();
    GetDataAndPlot(customPlot, ch);

    LogMsg.sprintf("Get data & try to save. ch %d, points %d", ch, ui->spinBox_count->value() );
    Log(LogMsg);

}

void MainWindow::GetDataAndPlot(QCustomPlot *Plot, int ch){

    oscui->osc->GetData(ch, ui->spinBox_count->value(),oscui->osc->acqFlag);

    if( Plot->graphCount() == 0){
        Plot->xAxis->setLabel("time [us]");
        Plot->yAxis->setLabel("Volatge [V]");
        Plot->xAxis->setRange(oscui->osc->xMin,oscui->osc->xMax);
        Plot->yAxis->setRange(oscui->osc->yMin * 2, oscui->osc->yMax * 2);
    }

    double plotyMax = Plot->yAxis->range().upper;
    double plotyMin = Plot->yAxis->range().lower;
    if( oscui->osc->yMin * 2 < plotyMin ) plotyMin = oscui->osc->yMin * 2;
    if( oscui->osc->yMax * 2 > plotyMax ) plotyMax = oscui->osc->yMax * 2;
    Plot->yAxis->setRange(plotyMin, plotyMax);

    while(Plot->graphCount() < ch){
        Plot->addGraph();
    }

    switch (ch) {
    case 1:Plot->graph(ch-1)->setPen(QPen(Qt::blue)); break;
    case 2:Plot->graph(ch-1)->setPen(QPen(Qt::red)); break;
    case 3:Plot->graph(ch-1)->setPen(QPen(Qt::darkGreen)); break;
    case 4:Plot->graph(ch-1)->setPen(QPen(Qt::magenta)); break;
    }

    Plot->graph(ch-1)->setData(oscui->osc->xData[ch], oscui->osc->yData[ch]);

    Plot->replot();

    if( dataFile != NULL){
        dataFile->AppendData("test", oscui->osc->xData[ch], oscui->osc->yData[ch]);
    }
}

void MainWindow::on_pushButton_openFile_clicked()
{
    //Get current file Path
    QString old_filePath = ui->lineEdit_FileName->text();
    QString old_dirName  = "";
    QString old_fileName = "";

    QStringList old_sFile = old_filePath.split('/');
    int old_size = old_sFile.size();
    if( old_size >= 2){
        for ( int i = 0; i < old_size-2 ; i++){
            old_dirName.append(old_sFile[i]);
            old_dirName.append("/");
        }
        old_dirName.append(old_sFile[old_size-2]);
        old_fileName = old_sFile[old_size-1];
    }

    //qDebug() << old_filePath << "," << old_dirName << "," << old_fileName;

    // Set new FIle Path
    QString filePath = QFileDialog::getSaveFileName(this, "Save File", "C:/Users/Triplet-ESR/Desktop");
    QString dirName;
    QString fileName;


    if( filePath == "" ) {
        filePath = old_filePath;
        dirName  = old_dirName;
        fileName = old_fileName;
    }else{
        QStringList sFile = filePath.split('/');
        int size = sFile.size();
        if( size >= 2){
            for ( int i = 0; i < size-2 ; i++){
                dirName.append(sFile[i]);
                dirName.append("/");
            }
            dirName.append(sFile[size-2]);
            fileName = sFile[size-1];
        }
    }
    //qDebug() << filePath << "," << dirName << "," << fileName;

    //Display
    ui->lineEdit_FileName->setText(filePath);
    if( filePath != old_filePath){
        QString logfilePath = dirName;
        logfilePath.append("/log.txt").insert(0, "Log : ");
        ui->groupBox_log->setTitle(logfilePath);
    }

    // refreash file
    if( old_dirName != dirName) {
        if( logFile != NULL ){
            delete logFile;
        }
        logFile = new FileIO (dirName, "log.txt", 4);

        if( dataFile != NULL ){
            delete dataFile;
        }
        dataFile = new FileIO (dirName, fileName, 3);
        dataFile->FileStructure();

    }

    if( old_dirName == dirName && old_fileName != fileName){
        if( dataFile != NULL ){
            delete dataFile;
        }
        dataFile = new FileIO (dirName, fileName, 3);
        dataFile->FileStructure();
    }


}
