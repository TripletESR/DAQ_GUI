#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    wfgui(NULL),
    oscui(NULL),
    logFile(NULL),
    dataFile(NULL),
    plot(NULL)
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


    connect(wfgui, SIGNAL(ReadOSCDMM()), oscui, SLOT(on_pushButton_2_clicked()));
    connect(oscui, SIGNAL(SendDMM(double)), wfgui, SLOT(SaveOscDMM(double)));

    wfgui->OpenHallProbe();
    connect(wfgui->hallProbe, SIGNAL(SendMsg(QString)), this, SLOT(Write2Log(QString)));
    wfgui->hallProbe->SetMeasureDCV();

    //Get Setting
    wfgui->on_comboBox_ch_activated(0);
    //wfgui->on_pushButton_GetDeviceSetting_clicked();
    oscui->on_checkBox_Lock_clicked(1); //get osc status
    on_spinBox_count_valueChanged(1000);
    //oscui->osc->Clear(); //TODO somehow, the OSC has error msg that setting conflict.

    ui->pushButton_Auto->setEnabled(0);
    ui->pushButton_Save->setEnabled(0);

    plot = ui->customPlot;
    plot->setInteraction(QCP::iRangeDrag,true);
    plot->setInteraction(QCP::iRangeZoom,true);
    plot->xAxis->setLabel("time [us]");
    plot->yAxis->setLabel("Volatge [V]");
    //plot->plotLayout()->insertRow(0);


    Write2Log("-----------------------------------");

    //Display Massege for devices
    Write2Log(wfgui->Msg);
    Write2Log(oscui->Msg);
    Write2Log(wfgui->hallProbe->Msg);

    Write2Log(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Ready.");
}

MainWindow::~MainWindow()
{
    Write2Log("========================= Program ended.");

    delete ui;
    delete wfgui;
    delete oscui;

    delete logFile;
    delete dataFile;

    delete plot;

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
        wfgui->on_pushButton_GetHPV_clicked();
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

void MainWindow::on_pushButton_GetSingle_clicked()
{
    int ch = ui->spinBox_ch->value();
    int points = ui->spinBox_count->value();
    GetData(ch, points);

    //oscui->osc->SetDVM(1,2, 1); // ch2, DC
    //qDebug() << "+++++++++++ DVM ?" << oscui->osc->GetDVM();
    PlotGraph(ch, oscui->osc->xData[ch],
             oscui->osc->yData[ch],
             oscui->osc->xMin,
             oscui->osc->xMax,
             oscui->osc->yMin,
             oscui->osc->yMax);

}

void MainWindow::GetData(int ch, int points){
    logMsg.sprintf("=========== Get Data == Ch %d, #pt %d", ch, ui->spinBox_count->value());
    Write2Log(logMsg);
    oscui->osc->GetData(ch, points ,0);
}

void MainWindow::GetBGData(int ch, int points)
{
    logMsg.sprintf("=========== Get BG Data == Ch %d, #pt %d", ch, ui->spinBox_count->value());
    Write2Log(logMsg);
    oscui->osc->GetData(ch, points, 1);
}

void MainWindow::PlotGraph(int ch, QVector<double> x, QVector<double> y, double xMin, double xMax, double yMin, double yMax){

    logMsg.sprintf("=========== Plot Ch %d", ch);
    Write2Log(logMsg);

    // plot title
    wfgui->GetMagField();
    QString plotLabel;
    plotLabel.sprintf("%06.4fV %6.2fmV", wfgui->wfg->offset, wfgui->GetHallVoltage());
    QCPPlotTitle * title = new QCPPlotTitle(plot, plotLabel);
    plot->plotLayout()->addElement(0,0, title);


    // initialize
    if( plot->graphCount() == 0){
        plot->xAxis->setRange(xMin, xMax);
        plot->yAxis->setRange(yMin * 2, yMax * 2);
    }

    // adjust y range
    double plotyMax = plot->yAxis->range().upper;
    double plotyMin = plot->yAxis->range().lower;
    if( yMin * 2 < plotyMin ) plotyMin = yMin * 2;
    if( yMax * 2 > plotyMax ) plotyMax = yMax * 2;
    plot->yAxis->setRange(plotyMin, plotyMax);

    // add graph
    while(plot->graphCount() < ch){
        plot->addGraph();
    }

    // set plot color
    switch (ch) {
    case 1:plot->graph(ch-1)->setPen(QPen(Qt::blue)); break;
    case 2:plot->graph(ch-1)->setPen(QPen(Qt::red)); break;
    case 3:plot->graph(ch-1)->setPen(QPen(Qt::darkGreen)); break;
    case 4:plot->graph(ch-1)->setPen(QPen(Qt::magenta)); break;
        // when ch > 4, it would be the fitting function.
    }

    //fill data
    plot->graph(ch-1)->setData(x, y);

    //replot
    plot->replot();
}

void MainWindow::SaveData(QString head, QVector<double> x, QVector<double> y){

    if( dataFile != NULL){
        logMsg = "++++++ Saving Data .... Data name :";
        logMsg += head;
        Write2Log(logMsg);
        dataFile->AppendData(head, x, y);
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

    qDebug() << old_filePath << "," << old_dirName << "," << old_fileName;

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
    qDebug() << filePath << "," << dirName << "," << fileName;

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

    if(dataFile != NULL){
        ui->pushButton_Auto->setEnabled(1);
        ui->pushButton_Save->setEnabled(1);
    }

}

void MainWindow::on_pushButton_Auto_clicked()
{

    if( ui->lineEdit_start->text()=="") return;
    if( ui->lineEdit_end->text()=="") return;
    if( ui->lineEdit_step->text()=="") return;


    const int wfgch = 1;
    const double bStart = ui->lineEdit_start->text().toDouble();
    const double bEnd   = ui->lineEdit_end->text().toDouble();
    const double bInc   = ui->lineEdit_step->text().toDouble();

    // Force the control volatge to go down
    //if( bStart < bEnd ) return;

    // other restriction on the control voltage
    //if( bInc >= 0) return;
    if( bStart < 0 || bEnd < 0 )return;
    if( bStart > 5 || bEnd > 5) return;


    //open file
    if(dataFile == NULL){
        Write2Log("============== Please open a file to save data. Abort.");
        return;
    }

    const int ch = ui->spinBox_ch->value();
    const int points = ui->spinBox_count->value();

    //Get BG Data;
    //oscui->osc->GetData(ch, points, 1);

    //double bgmin = oscui->osc->GetMin(oscui->osc->BGData);
    //double bgmax = oscui->osc->GetMax(oscui->osc->BGData);
    //qDebug() << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << bgmin << "," << bgmax;

    //PlotGraph(ch, oscui->osc->xData[ch],
    //         oscui->osc->BGData,
    //         oscui->osc->xMin,
    //         oscui->osc->xMax,
    //          oscui->osc->yMin,
    //          oscui->osc->yMax);

    QVector<double> Y(points);

    //Get the WFG to be DC mode
    wfgui->wfg->SetWaveForm(wfgch, 8); // 1= sin,  8 = DC
    wfgui->wfg->GoToOffset(wfgch, bStart);
    wfgui->on_doubleSpinBox_Offset_valueChanged(bStart*1000);
    //wfgui->wfg->SetOffset(wfgch, bStart);
    //wfgui->SetMagField(wfgch, bStart);
    // wait for few min for B-field to stablized.
    Write2Log("------------------------------------- wait for 1 sec.");
    Sleep(1000);


    //Start measurement loop;
    int count = 0;
    //int n = ((int)bStart-(int)bEnd)/(int)bInc + 1;
    //QString str;
    //QProgressDialog progress("Getting Data ....", "Abort", 0, n, this);
    //progress.setWindowModality(Qt::WindowModal);

    QString name1 = ui->lineEdit_DataName->text();
    QString name;
    for( double b = bStart; b >= bEnd ; b += bInc){

       // str.sprintf("Getting Data from %f to %f, size %f | Current : %f", bStart, bEnd, -bInc, b);
       // progress.setLabelText(str);
       // progress.setValue(count);
       // if(progress.wasCanceled()) break;
        count ++;

        oscui->osc->GetData(ch, points, 0);

        PlotGraph(ch, oscui->osc->xData[ch],
                 oscui->osc->yData[ch],
                 oscui->osc->xMin,
                 oscui->osc->xMax,
                 oscui->osc->yMin,
                 oscui->osc->yMax);

        for( int i = 0; i < points; i++){
            //Y[i] = oscui->osc->yData[ch][i] - oscui->osc->BGData[i];
            Y[i] = oscui->osc->yData[ch][i];
        }

        //double mag = wfgui->GetMagField();
        double hallV = wfgui->GetHallVoltage();
        name.sprintf("%s_%06.4fV_%06.2fmV", name1.toStdString().c_str(),b, hallV);

        dataFile->AppendData(name, oscui->osc->xData[ch], Y);

        QString msg;
        msg.sprintf("recorded and saved %s.", name.toStdString().c_str());
        Write2Log(msg);

        //wfgui->wfg->SetFreq(wfgch, b*1000);
        //wfgui->SetMagField(wfgch, b);
        wfgui->wfg->GoToOffset(wfgch, b);
        wfgui->on_doubleSpinBox_Offset_valueChanged(b*1000);
    }

    Write2Log("===================  Auto DAQ completed.");
}

void MainWindow::on_pushButton_Save_clicked()
{
    int ch = ui->spinBox_ch->value();
    QString saveName = ui->lineEdit_DataName->text();
    double dc = wfgui->wfg->offset; //V
    //double mag = wfgui->GetMagField(); // mT
    double hallV = wfgui->GetHallVoltage(); // mV
    QString tmp;
    tmp.sprintf("_%06.4fV_%06.2fmV", dc, hallV);
    saveName.append(tmp);
    SaveData(saveName,  oscui->osc->xData[ch], oscui->osc->yData[ch]);
}

void MainWindow::on_spinBox_count_valueChanged(int arg1)
{
    oscui->osc->GetTime();
    double tRange=oscui->osc->tRange;

    double resol = tRange/arg1;
    if(resol == 0){
        ui->lineEdit_Resol->setText("NaN");
    }else{
        ui->lineEdit_Resol->setText(QString::number(resol));
    }
}
