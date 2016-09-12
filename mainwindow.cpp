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

    ui->setupUi(this);
    this->setGeometry(10,50,this->geometry().width(),this->geometry().height());

    // logfile
    MsgCount = 0;
    QString logFileName;
    QDate date = QDate::currentDate();
    logFileName.sprintf("Log%s.txt", date.toString("yyyyMMdd").toStdString().c_str());
    logFile = new QFileIO ("C:/Users/Triplet-ESR/Desktop/DAQ_Log", logFileName, 4);
    logFile->SaveLogData("========================================== new session.");
    Write2Log(logFile->Msg);
    connect(logFile, SIGNAL(SendMsg(QString)), this, SLOT(Write2Log(QString)));

    ui->groupBox_log->setTitle(logFileName.insert(0,"Log : C:/Users/Triplet-ESR/Desktop/DAQ_Log/"));

    // call wfg and osc dialog, in which the wfg and osc will be created
    wfgui = new WFG_Dialog(this);
    Write2Log(wfgui->Msg);
    oscui = new osc_Dialog(this);
    Write2Log(oscui->Msg);


    //Setting up connectting
    connect(oscui, SIGNAL(SendLogMsg(QString)), this, SLOT(Write2Log(QString)));
    connect(wfgui, SIGNAL(SendLogMsg(QString)), this, SLOT(Write2Log(QString)));
    connect(wfgui->wfg, SIGNAL(SendMsg(QString)), this, SLOT(Write2Log(QString)));
    connect(oscui->osc, SIGNAL(SendMsg(QString)), this, SLOT(Write2Log(QString)));

    //Get Setting
    wfgui->on_comboBox_ch_activated(0);
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

    Write2Log("========================= Program ended.");

}

void MainWindow::Write2Log(QString msg) //TODO not finished
{
    MsgCount ++;

    QDateTime date = QDateTime::currentDateTime();
    QString countStr;
    countStr.sprintf(" [%04d]:    ", MsgCount);
    msg.insert(0,countStr).insert(0,date.toString());
    if( logFile != NULL){
        logFile->SaveLogData(msg);
    }
    ui->plainTextEdit->appendPlainText(msg); //has default endline
    QScrollBar *v = ui->plainTextEdit->verticalScrollBar();
    v->setValue(v->maximum());
    qDebug() << msg;

}


void MainWindow::on_actionWave_From_Generator_triggered()
{
    if( wfgui->isHidden()){
        Write2Log("=== Open Wave From Generator Pannel.");
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
        Write2Log("=== Open Oscillopscope Pannel.");
        oscui->setGeometry(this->geometry().x()+ this->geometry().width()+20,
                           this->geometry().y() + wfgui->geometry().height()+ 60,
                           oscui->geometry().width(),
                           oscui->geometry().height());
        oscui->show();
    }
}

void MainWindow::on_pushButton_clicked()
{
    customPlot = ui->customPlot;

    int ch = ui->spinBox_ch->value();
    GetDataAndPlot(customPlot, ch);
}

void MainWindow::GetDataAndPlot(QCustomPlot *Plot, int ch){

    logMsg.sprintf("=========== Get Data == Ch %d, #pt %d", ch, ui->spinBox_count->value());
    Write2Log(logMsg);
    oscui->osc->GetData(ch, ui->spinBox_count->value(),oscui->osc->acqFlag);

    logMsg.sprintf("=========== Plot Ch %d", ch);
    Write2Log(logMsg);
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
        logMsg = "++++++ Saving Data .... Data name :";
        logMsg += "test";
        Write2Log(logMsg);
        dataFile->AppendData("test", oscui->osc->xData[ch], oscui->osc->yData[ch]);
    }else{
        Write2Log("Save file did not set. Data not saved.");
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

    // refreash data file while changed
    if( old_dirName != dirName || old_fileName != fileName){
        if( dataFile != NULL ){
            delete dataFile;
        }
        dataFile = new QFileIO (dirName, fileName, 3);
        connect(dataFile, SIGNAL(SendMsg(QString)), this, SLOT(Write2Log(QString)));
        dataFile->FileStructure();

    }


}
