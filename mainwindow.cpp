#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    wfgui(NULL),
    oscui(NULL),
    logFile(NULL),
    dataFile(NULL),
    plot(NULL),
    progress(NULL)
{
    ui->setupUi(this);
    int configLoadedFlag = loadConfigurationFile();

    if(configLoadedFlag == 1) {
        QMessageBox msgBox;
        msgBox.setText("The configuration file not exist.\n"
                       "please check the ProgramConfiguration.ini exist on Desktop.");
        msgBox.exec();
    }else if(configLoadedFlag == 2){
        QMessageBox msgBox;
        msgBox.setText("The configuration file fail to open.");
        msgBox.exec();
    }else if(configLoadedFlag == 3){
        QMessageBox msgBox;
        msgBox.setText("Some items are missing in configuration file.");
        msgBox.exec();
    }

    QString title; title.sprintf("DAQ v%3.2f", VERSION);
    this->setWindowTitle(title);
    this->setGeometry(10,50,this->geometry().width(),this->geometry().height());

    //Check Directory, is not exist, create
    QDir myDir;
    myDir.setPath(DATA_PATH); if( !myDir.exists()) myDir.mkpath(DATA_PATH);
    myDir.setPath(LOG_PATH); if( !myDir.exists()) myDir.mkpath(LOG_PATH);
    myDir.setPath(HALL_DIR_PATH); if( !myDir.exists()) myDir.mkpath(HALL_DIR_PATH);

    // logfile
    MsgCount = 0;
    QString logFileName;
    QDateTime dateTime = QDateTime::currentDateTime();
    logFileName.sprintf("Log%s.txt", dateTime.toString("yyyyMMdd_HHmmss").toStdString().c_str());
    logFile = new QFileIO (LOG_PATH, logFileName, 4);
    logFile->SaveLogData("========================================== new session.");
    Write2Log("Program " + title);
    const QString APP_PATH = QApplication::applicationDirPath();
    Write2Log("Program Location : " + APP_PATH);
    Write2Log(logFile->Msg);
    connect(logFile, SIGNAL(SendMsg(QString)), this, SLOT(Write2Log(QString)));

    ui->groupBox_log->setTitle(logFileName.insert(0,"/").insert(0, LOG_PATH).insert(0,"Log : "));

    // call wfg and osc dialog, in which the wfg and osc will be created
    wfgui = new WFG_Dialog(this);
    Write2Log(wfgui->Msg);
    //wfgui->setWindowFlags(wfgui->windowFlags() | Qt::Tool);
    oscui = new osc_Dialog(this);
    Write2Log(oscui->Msg);

    //Setting up connectting
    connect(oscui, SIGNAL(SendLogMsg(QString)), this, SLOT(Write2Log(QString)));
    connect(wfgui, SIGNAL(SendLogMsg(QString)), this, SLOT(Write2Log(QString)));
    connect(wfgui->wfg, SIGNAL(SendMsg(QString)), this, SLOT(Write2Log(QString)));
    connect(oscui->osc, SIGNAL(SendMsg(QString)), this, SLOT(Write2Log(QString)));
    connect(oscui->osc, SIGNAL(DeviceNotReady(double)), this, SLOT(SetProgressBar(double)));
    connect(oscui->osc, SIGNAL(DeviceReady(QString)), this, SLOT(WhenOSCReady(QString)));

    connect(wfgui, SIGNAL(ReadOSCDMM()), oscui, SLOT(on_pushButton_GetDVM_clicked()));
    connect(oscui, SIGNAL(SendDMM(double)), wfgui, SLOT(SaveOscDMM(double)));

    wfgui->OpenHallProbe();
    connect(wfgui->hallProbe, SIGNAL(SendMsg(QString)), this, SLOT(Write2Log(QString)));
    wfgui->hallProbe->SetMeasureDCV();

    //Get Setting
    wfgui->on_comboBox_ch_activated(0);
    wfgui->OpenHallParsFile();
    //wfgui->on_pushButton_GetDeviceSetting_clicked();
    oscui->on_checkBox_Lock_clicked(1); //Lock the osc, get osc status
    //oscui->osc->Clear(); //TODO somehow, the OSC has error msg that setting conflict.

    ui->pushButton_Auto->setEnabled(0);
    ui->pushButton_Save->setEnabled(0);
    ui->lineEdit_start->setEnabled(0);
    ui->lineEdit_end->setEnabled(0);
    ui->lineEdit_step->setEnabled(0);

    if( !oscui->osc->IsOpen()) ui->pushButton_GetSingle->setEnabled(0);

    plot = ui->customPlot;
    plot->setInteraction(QCP::iRangeDrag,true);
    plot->setInteraction(QCP::iRangeZoom,true);
    //plot->axisRect()->setRangeDrag(Qt::Vertical);
    //plot->axisRect()->setRangeZoom(Qt::Vertical);
    plot->axisRect()->setRangeDrag(Qt::Horizontal);
    plot->axisRect()->setRangeZoom(Qt::Horizontal);
    plot->xAxis->setLabel("time [us]");
    plot->yAxis->setLabel("Volatge [V]");
    plot->plotLayout()->insertRow(0);
    plotTitle = new QCPPlotTitle(plot, "title");
    plotTitle->setFont(QFont("sans", 9, QFont::Bold));
    plot->plotLayout()->addElement(0,0, plotTitle);
    //plot->plotLayout()->insertRow(0);

    ana = new Analysis();
    connect(ana, SIGNAL(SendMsg(QString)), this, SLOT(Write2Log(QString)));
    ui->comboBox_Fit->setEnabled(0);

    Write2Log("----------------------------------- Device status : ");

    //Display Massege for devices
    if(wfgui->wfg->IsOpen()){
        logMsg.sprintf("Opened : %s", wfgui->wfg->name.toStdString().c_str());
        ui->actionWave_From_Generator->setEnabled(1);
    }else{
        logMsg.sprintf("Not Opened : %s", wfgui->wfg->name.toStdString().c_str());
        ui->actionWave_From_Generator->setEnabled(0);
    }
    Write2Log(logMsg);

    if(oscui->osc->IsOpen()){
        logMsg.sprintf("Opened : %s", oscui->osc->name.toStdString().c_str());
        ui->actionOscilloscope->setEnabled(1);
        ui->spinBox_ch->setEnabled(1);
        ui->comboBox_points->setEnabled(1);
    }else{
        logMsg.sprintf("Not Opened : %s", oscui->osc->name.toStdString().c_str());
        ui->actionOscilloscope->setEnabled(0);
        ui->spinBox_ch->setEnabled(0);
        ui->comboBox_points->setEnabled(0);
        ui->lineEdit_Resol->setText("NaN");
        ui->lineEdit_repeatition->setText("NaN");
    }
    Write2Log(logMsg);

    if(wfgui->hallProbe->IsOpen()){
        logMsg.sprintf("Opened : %s", wfgui->hallProbe->name.toStdString().c_str());
    }else{
        logMsg.sprintf("Not Opened : %s", wfgui->hallProbe->name.toStdString().c_str());
    }
    Write2Log(logMsg);

    //=================================================== Setup linkage with database
    Write2Log("----------------------------------- Opening database.....");
    bool isDBExist = false;
    if( QFile::exists(DB_PATH) ){
        Write2Log("database exist : " + DB_PATH);
        isDBExist = true;
    }else{
        Write2Log("No database : " + DB_PATH);
        isDBExist = false;
    }

    ui->comboBox_Sample->setEnabled(false);
    ui->pushButton_ComfirmSelection->setEnabled(false);
    ui->comboBox_laser->setEnabled(false);
    ui->lineEdit_repeatition->setEnabled(false);
    ui->lineEdit_Temperature->setEnabled(false);

    if( isDBExist){
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(DB_PATH);
        db.open();
        if( !db.isOpen()){
            Write2Log("Database open Error.");
            ui->checkBox_TestRun->setEnabled(false);
            ui->comboBox_Chemical->setEnabled(false);
            QMessageBox msgBox;
            msgBox.setText("Database failed to load.");
            msgBox.exec();
        }
        dbTable = new QSqlTableModel();
        Write2Log("Database open succesful.");
        ui->checkBox_TestRun->setEnabled(true);
        ui->comboBox_Chemical->setEnabled(true);
        ui->comboBox_laser->setEnabled(true);

        updateChemicalCombox();
        updateLaserCombox();

    }

    if(oscui->osc->IsOpen()){
        Write2Log("----------- Get Time resolution and trigger rate.");
        int points = ui->comboBox_points->currentText().toInt();
        oscui->osc->GetTime();
        double tRange=oscui->osc->tRange;
        double resol = tRange/points*1000;
        ui->lineEdit_Resol->setText(QString::number(resol));
        ui->lineEdit_repeatition->setText(QString::number(oscui->osc->trgRate));
    }

    Write2Log(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Ready.");
}

MainWindow::~MainWindow()
{
    delete dataFile;
    delete plotTitle;
    delete plot;

    delete oscui;
    delete wfgui;
    Write2Log("========================= Program ended.");
    delete logFile;
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *keyEvent)
{
    if( keyEvent->key() == Qt::Key_Shift){
        plot->axisRect()->setRangeDrag(Qt::Vertical);
        plot->axisRect()->setRangeZoom(Qt::Vertical);
        //plot->axisRect()->setRangeDrag(Qt::Horizontal);
        //plot->axisRect()->setRangeZoom(Qt::Horizontal);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *keyEvent)
{
    //plot->axisRect()->setRangeDrag(Qt::Vertical);
    //plot->axisRect()->setRangeZoom(Qt::Vertical);
    plot->axisRect()->setRangeDrag(Qt::Horizontal);
    plot->axisRect()->setRangeZoom(Qt::Horizontal);

}

int MainWindow::loadConfigurationFile()
{
    QDateTime date;// = QDateTime::currentDateTime();
    QString countStr;
    countStr.sprintf(" [%06d]:    ", 0);
    QString msg;

    QString path = DESKTOP_PATH + "/ProgramsConfiguration.ini";
    if( !QFile::exists(path) ){
        msg = "Configuration not found. | " + path;
        msg.insert(0,countStr).insert(0,date.currentDateTime().toString());
        ui->plainTextEdit->appendPlainText(msg);
        qDebug() << msg;
        return 1;
    }

    QFile configFile(path);
    configFile.open(QIODevice::ReadOnly);
    if( configFile.isOpen() ){
        msg = "Configuration file found and opened :" + path;
        msg.insert(0,countStr).insert(0,date.currentDateTime().toString());
        ui->plainTextEdit->appendPlainText(msg);
        qDebug() << msg;
    }else{
        msg = "Configuration file found but fail to open.";
        msg.insert(0,countStr).insert(0,date.currentDateTime().toString());
        ui->plainTextEdit->appendPlainText(msg);
        qDebug() << msg;
        return 2;
    }

    QTextStream stream(&configFile);
    QString line;
    QStringList lineList;

    int itemCount = 0;

    while(stream.readLineInto(&line) ){
        if( line.left(1) == "#" ) continue;
        lineList = line.split(" ");
        //qDebug() << lineList[0] << ", " << lineList[lineList.size()-1];
        if( lineList[0] == "DATA_PATH") {
            DATA_PATH = lineList[lineList.size()-1];
            itemCount ++;
        }
        if( lineList[0] == "DB_PATH") {
            DB_PATH = lineList[lineList.size()-1];
            itemCount ++;
        }
        if( lineList[0] == "HALL_DIR_PATH") {
            HALL_DIR_PATH = lineList[lineList.size()-1];
            itemCount ++;
        }
        if( lineList[0] == "HALL_PATH") {
            HALL_PATH = lineList[lineList.size()-1];
            itemCount ++;
        }
        if( lineList[0] == "LOG_PATH") {
            LOG_PATH = lineList[lineList.size()-1];
            itemCount ++;
        }
        if( lineList[0] == "OSCILLOSCOPE") {
            OSCILLOSCOPE = lineList[lineList.size()-1];
            itemCount ++;
        }
        if( lineList[0] == "WAVEFROM_GENERATOR") {
            WAVEFROM_GENERATOR = lineList[lineList.size()-1];
            itemCount ++;
        }
        if( lineList[0] == "DIGITALMETER") {
            DIGITALMETER = lineList[lineList.size()-1];
            itemCount ++;
        }
    }

    if( itemCount != 8){
        return 3;
    }

    qDebug() << OSCILLOSCOPE;
    qDebug() << WAVEFROM_GENERATOR;
    qDebug() << DIGITALMETER;
    return 0;
}

void MainWindow::Write2Log(QString msg) //TODO not finished
{
    MsgCount ++;

    QDateTime date;// = QDateTime::currentDateTime();
    QString countStr;
    countStr.sprintf(" [%06d]:    ", MsgCount);
    msg.insert(0,countStr).insert(0,date.currentDateTime().toString());
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
    //int points = ui->spinBox_count->value();
    int points = ui->comboBox_points->currentText().toInt();

    double trigRate = oscui->osc->GetTriggerRate();
    if( trigRate <= 0 || trigRate > 1e+8){
        QString msg;
        msg.sprintf("Trigger Rate is abnormal : %e. Not Take Data.", trigRate);
        Write2Log(msg);
        return;
    }

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
    logMsg.sprintf("=========== Get Data == Ch %d, #pt %d", ch, points);
    Write2Log(logMsg);
    oscui->osc->GetData(ch, points ,0);

    qDebug() << "set ana data";
    ana->SetData(oscui->osc->xData[ch], oscui->osc->yData[ch]);
    ui->comboBox_Fit->setEnabled(1);
    ui->comboBox_Fit->setCurrentIndex(0);

}

void MainWindow::GetBGData(int ch, int points)
{
    logMsg.sprintf("=========== Get BG Data == Ch %d, #pt %d", ch, points);
    Write2Log(logMsg);
    oscui->osc->GetData(ch, points, 1);
}

void MainWindow::PlotGraph(int ch, QVector<double> x, QVector<double> y, double xMin, double xMax, double yMin, double yMax){


    if( 1 <= ch && ch <=4 ){
        logMsg.sprintf("=========== Plot Ch %d", ch);
    }else if( ch == 5){
        logMsg = "========== Plot Fit Function.";
    }
    Write2Log(logMsg);

    // plot title
    if( 1 <= ch && ch <=4 ){
        QString plotLabel;
        plotLabel.sprintf("%6.4fV %8.4fmV", wfgui->wfg->offset, wfgui->GetHallVoltage());
        plotTitle->setText(plotLabel);
    }
    // initialize
    if( plot->graphCount() == 0){
        for(int i = 1; i <= 5 ; i++ ){
            plot->addGraph();
        }
        // set plot color
        plot->graph(0)->setPen(QPen(Qt::blue));
        plot->graph(1)->setPen(QPen(Qt::red));
        plot->graph(2)->setPen(QPen(Qt::darkGreen));
        plot->graph(3)->setPen(QPen(Qt::magenta));
        plot->graph(4)->setPen(QPen(Qt::darkCyan));

        plot->xAxis->setRange(xMin, xMax);
        double yMean = (yMax + yMin )/2;
        double yWidth = (yMax - yMin )/2;
        plot->yAxis->setRange( yMean - yWidth*2, yMean+ yWidth*2 );
    }

    // adjust y range
    double plotyMax = plot->yAxis->range().upper;
    double plotyMin = plot->yAxis->range().lower;
    if( yMin * 2 < plotyMin ) plotyMin = yMin * 2;
    if( yMax * 2 > plotyMax ) plotyMax = yMax * 2;
    plot->yAxis->setRange(plotyMin, plotyMax);

    //fill data
    plot->graph(ch-1)->clearData();
    plot->graph(ch-1)->setData(x, y);

    //replot
    plot->replot();
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
    QString filePath = QFileDialog::getSaveFileName(this, "Save File", DATA_PATH);
    QString dirName;
    QString fileName;

    if( filePath == "" ) {
        filePath = old_filePath;
        dirName  = old_dirName;
        fileName = old_fileName;
    }else{
        //add .dat in the end of filePath
        if(filePath.right(4) != ".dat"){
            filePath.append(".dat");
        }
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
        Write2Log(dataFile->Msg);
        connect(dataFile, SIGNAL(SendMsg(QString)), this, SLOT(Write2Log(QString)));
        dataFile->FileStructure();

    }

    if(dataFile != NULL){
        //ui->pushButton_Auto->setEnabled(1);
        ui->pushButton_Save->setEnabled(1);
        ui->lineEdit_start->setEnabled(1);
        ui->lineEdit_end->setEnabled(1);
        ui->lineEdit_step->setEnabled(1);
    }
    openFlag = 1;
}

void MainWindow::on_pushButton_Auto_clicked()
{

    if( ui->lineEdit_start->text()=="") return;
    if( ui->lineEdit_end->text()=="") return;
    if( ui->lineEdit_step->text()=="") return;

    const double bStart = ui->lineEdit_start->text().toDouble();
    const double bEnd   = ui->lineEdit_end->text().toDouble();
    const double bInc   = ui->lineEdit_step->text().toDouble();

    //================ restrictions on the control voltage
    if( bStart < 0 || bEnd < 0 )return;
    if( bStart > 5 || bEnd > 5) return;

    if( bStart > bEnd && bInc >=0 ){
        Write2Log("Step size should be negative.");
        return;
    }

    if( bStart < bEnd && bInc <=0 ){
        Write2Log("Step size should be positive.");
        return;
    }

    //============== open file check
    if(dataFile == NULL){
        Write2Log("============== Please open a file to save data. Abort.");
        return;
    }

    Write2Log("========================== Start Auto DAQ.");
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    this->show();
    ui->lineEdit_start->setEnabled(0);
    ui->lineEdit_end->setEnabled(0);
    ui->lineEdit_step->setEnabled(0);
    const int ch = ui->spinBox_ch->value();
    //const int points = ui->spinBox_count->value();
    const int points = ui->comboBox_points->currentText().toInt();

    //=============== Get BG Data;
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

    //======================== Set the WFG to be DC mode
    const int wfgch = 1;
    const double rate = wfgui->GetOffSetRate() /1000.; // in V
    wfgui->wfg->SetWaveForm(wfgch, 8); // 1= sin,  8 = DC
    wfgui->wfg->GoToOffset(wfgch, bStart, rate);
    wfgui->on_doubleSpinBox_Offset_valueChanged(bStart*1000);
    // wait for few min for B-field to stablized.
    //Write2Log("------------------------------------- wait for 3 sec.");
    //QEventLoop eventloop;
    //QTimer::singleShot(3*1000, &eventloop, SLOT(quit()));
    //eventloop.exec();

    oscui->hide();
    wfgui->hide();
    ui->actionOscilloscope->setEnabled(0);
    ui->actionWave_From_Generator->setEnabled(0);

    //======================== Start measurement loop;
    int count = 0;
    int totCount = ui->lineEdit_numData->text().toInt();
    QString str;
    progress = new QProgressDialog("Getting Data ....", "Abort", 0, totCount, this);
    //progress.setWindowModality(Qt::WindowModal);
    //disconnect the close signal.
    progress->disconnect();
    //when the close button clicked , canceled signal will send;
    connect(progress, SIGNAL(canceled()), this, SLOT(TryToBreakAutoDAQ()));
    //when the X clicked, the rejected and finished signal will send, onlt rejected will hide;
    //connect(progress, SIGNAL(rejected()), this, SLOT(TryToBreakAutoDAQ()));
    //remove the dialog X button
    progress->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    progress->setAutoClose(false);
    breakAutoDAQFlag = 0;
    progress->show();

    QVector<double> Y(points);
    QString name1 = ui->lineEdit_DataName->text();
    QString name;
    double b = bStart;
    while( (bStart > bEnd && b >= bEnd) || (bStart < bEnd && b <= bEnd) ){

        if( breakAutoDAQFlag ){
            delete progress;
            break;
        }else{
            str.sprintf("From %5.3f V to %5.3f V, size %5.3f V| Current : %5.3f V", bStart, bEnd, bInc, b);
            progress->setLabelText(str);
            progress->setValue(count);
        }

        wfgui->wfg->GoToOffset(wfgch, b, rate);
        wfgui->on_doubleSpinBox_Offset_valueChanged(b*1000);

        GetData(ch, points);

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

        double hallV = wfgui->GetHallVoltage();
        int acqCount = oscui->osc->GetAcquireCount();
        name.sprintf("%s_Avg%05d_%06.4fV_%08.4fmV", name1.toStdString().c_str(), acqCount, b, hallV);

        dataFile->AppendData(name, oscui->osc->xData[ch], Y);

        count ++;
        QString msg;
        msg.sprintf(" %d/%d =================== recorded and saved %s.", count, totCount, name.toStdString().c_str());
        Write2Log(msg);

        b += bInc;
    }

    if( breakAutoDAQFlag){
        Write2Log("================= Auto DAQ was aborted by User.");
    }else{
        progress->hide();
        delete progress;
        Write2Log("===================  Auto DAQ completed.");

        //after the data-taking, save data condition;
        QVector<double> dummy;
        dataFile->AppendData("####EndOfData####", dummy, dummy);
        QDateTime dateTime = QDateTime::currentDateTime();
        dataFile->AppendData("# Date       : " + dateTime.toString("yyyy-MM-dd, HH:mm:ss"), dummy, dummy);
        dataFile->AppendData("# repetition : " + QString::number(oscui->osc->GetTriggerRate()), dummy, dummy);
        dataFile->AppendData("# average    : " + QString::number(oscui->osc->acqCount), dummy, dummy);

        if( openFlag == 2){
            dataFile->AppendData("# Chemical      : " + ui->comboBox_Chemical->currentText(), dummy, dummy);
            dataFile->AppendData("# Sample        : " + ui->comboBox_Sample->currentText(), dummy, dummy);
            dataFile->AppendData("# Laser         : " + ui->comboBox_Sample->currentText(), dummy, dummy);
            dataFile->AppendData("# Temperature   : " + ui->lineEdit_Temperature->text() + "K", dummy, dummy);
        }

    }

    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    this->show();
    ui->lineEdit_start->setEnabled(1);
    ui->lineEdit_end->setEnabled(1);
    ui->lineEdit_step->setEnabled(1);
    ui->actionOscilloscope->setEnabled(1);
    ui->actionWave_From_Generator->setEnabled(1);

}

void MainWindow::TryToBreakAutoDAQ()
{
    progress->show();
    QMessageBox msgBox("Warning!", "Are you sure want to Abort?",
                       QMessageBox::Warning,
                       QMessageBox::No | QMessageBox::Default | QMessageBox::Escape,
                       QMessageBox::Yes,
                       QMessageBox::NoButton);

    if( msgBox.exec() == QMessageBox::Yes){
        breakAutoDAQFlag = 1;
        progress->hide();
        progress->disconnect();
    }else{
        breakAutoDAQFlag = 0;
    }

}

void MainWindow::on_pushButton_Save_clicked()
{
    int ch = ui->spinBox_ch->value();
    QString saveName = ui->lineEdit_DataName->text();
    double dc = wfgui->wfg->offset; //V
    //double mag = wfgui->GetMagField(); // mT
    double hallV = wfgui->GetHallVoltage(); // mV
    int acqCount = oscui->osc->GetAcquireCount();
    QString tmp;
    tmp.sprintf("_Avg%05d_%06.4fV_%06.2fmV", acqCount, dc, hallV);
    saveName.append(tmp);
    dataFile->AppendData(saveName, oscui->osc->xData[ch], oscui->osc->yData[ch]);
    //SaveData(saveName, oscui->osc->xData[ch], oscui->osc->yData[ch]);
}

void MainWindow::on_lineEdit_step_editingFinished()
{
    double bStart = ui->lineEdit_start->text().toDouble();
    double bEnd = ui->lineEdit_end->text().toDouble();
    double bStep = ui->lineEdit_step->text().toDouble();

    if( bStart == bEnd ){
        ui->lineEdit_numData->setText("Start == End.");
        ui->pushButton_Auto->setEnabled(0);
        return;
    }

    if( bStep == 0){
        ui->lineEdit_numData->setText("Step Size == 0.");
        ui->pushButton_Auto->setEnabled(0);
        return;
    }

    if( bStart > bEnd && bStep >= 0 ){
        ui->lineEdit_numData->setText("Step should < 0.");
        ui->pushButton_Auto->setEnabled(0);
        return;
    }
    if( bStart < bEnd && bStep <= 0 ){
        ui->lineEdit_numData->setText("Step should > 0.");
        ui->pushButton_Auto->setEnabled(0);
        return;
    }

    int n = qFloor(fabs(bStart-bEnd)/fabs(bStep) + 1.0001);
    ui->lineEdit_numData->setText(QString::number(n));

    double tRate = oscui->osc->GetTriggerRate();
    if( tRate > 1e+8 || tRate == 0){
        ui->lineEdit_EstTime->setText("Trigger Rate is abnormal.");
        ui->pushButton_Auto->setEnabled(0);
        return;
    }

    int acqCount = oscui->osc->GetAcquireCount();
    double estTime = acqCount/tRate*n/60.;
    ui->lineEdit_EstTime->setText(QString::number(estTime));

    ui->pushButton_Auto->setEnabled(1);

}

void MainWindow::SetProgressBar(double value)
{
    if( value == 0){
        double maxWaitTime = oscui->osc->GetMaxWaitTime();
        ui->progressBar->setMaximum(maxWaitTime);
        QString text;
        text.sprintf(" sec / %3.0f sec", maxWaitTime);
        text.insert(0,"%v");
        ui->progressBar->setFormat(text);
    }
    ui->progressBar->setValue(value);
}

void MainWindow::WhenOSCReady(QString msg)
{
    Write2Log(msg);
    ui->progressBar->setFormat("Completed.");
}

void MainWindow::on_comboBox_Fit_currentIndexChanged(int index)
{

    int parSize = 2;
    switch (index) {
    case 1:
        parSize = 2;
        Write2Log("================ Fit for : a * Exp(-t/Ta).");
        break;
    case 2:
        parSize = 3;
        Write2Log("================ Fit for : a * Exp(-t/Ta) + c.");
        break;
    case 3:
        parSize = 4;
        Write2Log("================ Fit for : a * Exp(-t/Ta) + b * Exp(-t/Tb).");
        break;
    case 4:
        parSize = 5;
        Write2Log("================ Fit for : a * Exp(-t/Ta) + b * Exp(-t/Tb) + c.");
        break;
    }

    if( index != 0){
        ana->NonLinearFit(parSize);

        QVector<double> fitYData = ana->GetFitFuncVector();
        QVector<double> xData = ana->GetXDataVector();
        plot->graph(4)->clearData();
        PlotGraph(5, xData, fitYData);

    }
}

void MainWindow::on_lineEdit_start_editingFinished()
{
    on_lineEdit_step_editingFinished();
}

void MainWindow::on_lineEdit_end_editingFinished()
{
    on_lineEdit_step_editingFinished();
}

void MainWindow::on_comboBox_points_currentTextChanged(const QString &arg1)
{
    int points = arg1.toInt();

    qDebug() << ui->comboBox_points->currentText().toInt();

    oscui->osc->GetTime();
    double tRange=oscui->osc->tRange;

    double resol = tRange/points*1000;

    if(resol == 0){
        ui->lineEdit_Resol->setText("NaN");
    }else{
        ui->lineEdit_Resol->setText(QString::number(resol));
    }
}

QStringList MainWindow::GetTableColEntries(QString tableName, int col)
{
    QSqlQuery query;
    query.exec("SELECT * FROM " + tableName);
    //qDebug() << query.executedQuery();

    QStringList entries;

    while(query.next()){
        //qDebug() << query.size() << "," << query.value(col);
        entries << query.value(col).toString();
    }

    return entries;
}

void MainWindow::updateChemicalCombox()
{
    dbTable->clear();
    dbTable->setTable("Chemical");
    dbTable->select();
    int nameIdx = dbTable->fieldIndex("NAME");
    qDebug() << nameIdx;
    QStringList ChemicalList = GetTableColEntries("Chemical", nameIdx);
    ui->comboBox_Chemical->clear();
    ui->comboBox_Chemical->addItems(ChemicalList);
}

void MainWindow::updateSampleCombox()
{
    dbTable->clear();
    dbTable->setTable("Sample");
    int nameIdx = dbTable->fieldIndex("NAME");
    QString ChemicalName = "'" + ui->comboBox_Chemical->currentText() + "'";
    QStringList SampleList = GetTableColEntries("Sample Where Sample.Chemical = " + ChemicalName, nameIdx);
    ui->comboBox_Sample->clear();
    ui->comboBox_Sample->addItems(SampleList);
}

void MainWindow::updateLaserCombox()
{
    dbTable->clear();
    dbTable->setTable("Laser");
    int nameIdx = dbTable->fieldIndex("NAME");
    QStringList LaserList = GetTableColEntries("Laser",nameIdx);
    ui->comboBox_laser->clear();
    ui->comboBox_laser->addItems(LaserList);
}

void MainWindow::on_comboBox_Chemical_currentIndexChanged(int index)
{
    if (index == -1) return;
    ui->comboBox_Sample->setEnabled(true);
    ui->lineEdit_Temperature->setEnabled(true);
    //ui->pushButton_ComfirmSelection->setEnabled(true);
    updateSampleCombox();
}

void MainWindow::on_comboBox_Sample_currentIndexChanged(int index)
{
    if( index == -1 ) return;
    dbTable->clear();
    dbTable->setTable("Sample");
    int solventIdx = dbTable->fieldIndex("Solvent");
    int concentrationIdx = dbTable->fieldIndex("Concentration");
    int dateIdx = dbTable->fieldIndex("Date");
    int commentIdx = dbTable->fieldIndex("Comment");

    QString SampleName = "'" + ui->comboBox_Sample->currentText() + "'";
    QStringList SolventList = GetTableColEntries("Sample WHERE Sample.NAME = " + SampleName, solventIdx);
    if(SolventList.size() == 1) ui->lineEdit_Solvent->setText(SolventList[0]);

    QStringList ConcentrationList = GetTableColEntries("Sample WHERE Sample.NAME = " + SampleName, concentrationIdx);
    if(ConcentrationList.size() == 1) ui->lineEdit_Concentration->setText(ConcentrationList[0]);

    QStringList CreationDateList = GetTableColEntries("Sample WHERE Sample.NAME = " + SampleName, dateIdx);
    if(CreationDateList.size() == 1) ui->lineEdit_CreationDate->setText( CreationDateList[0] );

    QStringList CommentList = GetTableColEntries("Sample WHERE Sample.NAME = " + SampleName, commentIdx);
    if(CommentList.size() == 1) ui->lineEdit_Comment->setText( CommentList[0]);

}

void MainWindow::on_comboBox_laser_currentIndexChanged(int index)
{
    if( index == -1 ) return;
    dbTable->clear();
    dbTable->setTable("Laser");
    int nameIdx = dbTable->fieldIndex("NAME");
    QString LaserName = "'" + ui->comboBox_laser->currentText() + "'";
    QStringList WavelengthList = GetTableColEntries("Laser WHERE Laser.NAME = " + LaserName, nameIdx);
    if(WavelengthList.size() == 1) ui->lineEdit_waveLength->setText(WavelengthList[0]);
}

void MainWindow::on_pushButton_ComfirmSelection_clicked()
{
    QString dirName, fileName;
    QDateTime dateTime = QDateTime::currentDateTime();
    fileName = dateTime.toString("yyyyMMddHHmmss");
    if( ui->checkBox_TestRun->isChecked() ) fileName += "_test";
    fileName += "_" + ui->comboBox_Chemical->currentText();
    fileName += "_" + ui->comboBox_Sample->currentText();
    fileName += "_" + ui->lineEdit_Solvent->text();
    fileName += "_" + ui->lineEdit_Concentration->text();
    if( ui->lineEdit_Temperature->text() != "<Temp>" ){
        fileName += "_" + ui->lineEdit_Temperature->text() + "K";
    }
    fileName += ".dat";

    dirName = DATA_PATH + "Data/";
    dirName += dateTime.toString("yyyy") + "/";
    dirName += dateTime.toString("MMMM") + "/";
    dirName += dateTime.toString("dd") ;

    ui->lineEdit_FileName->setText(dirName + "/" + fileName);

    if( dataFile != NULL){
        delete dataFile;
    }
    dataFile = new QFileIO (dirName, fileName, 3);
    Write2Log(dataFile->Msg);
    connect(dataFile, SIGNAL(SendMsg(QString)), this, SLOT(Write2Log(QString)));
    //dataFile->FileStructure();

    if(dataFile != NULL){
        ui->pushButton_Save->setEnabled(1);
        ui->lineEdit_start->setEnabled(1);
        ui->lineEdit_end->setEnabled(1);
        ui->lineEdit_step->setEnabled(1);
    }

    //save to Database
    if( ui->checkBox_TestRun->isChecked() == false){

        QSqlQuery query;
        query.prepare("INSERT INTO Data (Sample, Date, Laser, repetition, Average, DataPoint, Temperature, TimeRange, Comment, PATH)"
                      "VALUES (:Sample, :Date, :Laser, :repetition, :Average, :DataPoint, :Temperature, :TimeRange, :Comment, :PATH)");

        query.bindValue(0, ui->comboBox_Sample->currentText());
        query.bindValue(1, dateTime.toString("yyyy-MM-dd"));
        query.bindValue(2, ui->comboBox_laser->currentText());
        query.bindValue(3, oscui->osc->GetTriggerRate());
        query.bindValue(4, oscui->osc->acqCount);
        query.bindValue(5, ui->comboBox_points->currentText());
        query.bindValue(6, ui->lineEdit_Temperature->text());
        query.bindValue(7, oscui->osc->tRange);
        query.bindValue(8, ui->lineEdit_Comment->text());

        int len1 = DATA_PATH.length();
        int len2 = dirName.length();
        query.bindValue(9, dirName.right(len2-len1) + fileName);
        query.exec();

        Write2Log("Written to database : ");
        QString msg;
        msg = "ID, Sample, Date, Laser, repetition, Average, DataPoint, Temperature, TimeRange, Comment, PATH";
        Write2Log(msg);

        query.exec("SELECT * FROM Data");
        int col = query.record().count();
        query.last();
        msg.clear();
        for( int i = 0 ; i < col; i++){
            msg += query.value(i).toString() + ", ";
        }

        Write2Log(msg);
    }else{
        Write2Log("Test run : Database untouched.");
    }
    openFlag = 2;
}

void MainWindow::on_lineEdit_Temperature_editingFinished()
{
    double temperature = ui->lineEdit_Temperature->text().toDouble();
    if( temperature > 0 ){
        ui->pushButton_ComfirmSelection->setEnabled(true);
    }else{
        ui->pushButton_ComfirmSelection->setEnabled(false);
    }
}

