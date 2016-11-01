#include "wfg_dialog.h"
#include "ui_wfg_dialog.h"

WFG_Dialog::WFG_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WFG_Dialog),
    hallProbe(NULL)
{
    ui->setupUi(this);

    wfg = new WaveFromGenerator(KEYSIGHT33500B);

    if( wfg->sta == VI_SUCCESS ) {
        Msg.sprintf("Opened : %s", wfg->name.toStdString().c_str());
        ui->lineEdit->setText(wfg->name);
    }else{
        Msg = "Cannot Open : Wave Form generator.";
        ui->lineEdit->setText(Msg);
    }

    //on_comboBox_ch_activated(1); //get setting of ch2
    //on_comboBox_ch_activated(0); //get setting of ch1

    plot = ui->HallPlot;
    plot->addGraph();
    plot->xAxis->setLabel("DC Vol. [V]");
    plot->yAxis->setLabel("Hall Vol. [mV]");
    plot->graph(0)->setPen(QPen(Qt::blue));
    plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle , 1));
    plot->xAxis->setRange(-0.5, 5);
    plot->yAxis->setRange(-10, 75);
    plot->addGraph(plot->xAxis, plot->yAxis2);
    plot->graph(1)->setPen(QPen(Qt::red));
    plot->graph(1)->setLineStyle(QCPGraph::lsNone);
    plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle , 1));
    plot->yAxis2->setVisible(1);
    plot->yAxis2->setLabel("B-Field [mT]");
    plot->yAxis2->setRange(-100,750);
    plot->setInteraction(QCP::iRangeDrag,true);
    plot->setInteraction(QCP::iRangeZoom,true);
    plot->axisRect()->setRangeDrag(Qt::Vertical);
    plot->axisRect()->setRangeZoom(Qt::Vertical);

    ClearData();
}

WFG_Dialog::~WFG_Dialog()
{
    delete ui;
    delete wfg;
    delete hallProbe;
    delete plot;
}

void WFG_Dialog::SaveOscDMM(double dvm){
    bField.push_back(-dvm);
}

void WFG_Dialog::OpenHallProbe(){
    hallProbe = new DMM(KEITHLEY2000);
    SendLogMsg(hallProbe->Msg);
    //hallProbe->SetMeasureDCV();
}

void WFG_Dialog::on_comboBox_ch_activated(int index)
{
    switch(index){
        case 0: wfg->ch = 1; break;
        case 1: wfg->ch = 2; break;
    }

    wfg->GetSetting(wfg->ch);
    ui->comboBox_WFG->setCurrentIndex(wfg->index);
    on_comboBox_WFG_activated(wfg->index); // enable/diable input
    DisplaySetting();
}

void WFG_Dialog::on_checkBox_clicked(bool checked)
{
    if( checked ) {
        wfg->OpenCh(wfg->ch);
    }else{
        wfg->CloseCh(wfg->ch);
    }
}

void WFG_Dialog::on_comboBox_WFG_activated(int index)
{
    if( index == 0) {
        wfg->SetWaveForm(wfg->ch,1); // 1 for Sin
        ui->doubleSpinBox_Amp->setDisabled(0);
        ui->doubleSpinBox_Freq->setDisabled(0);
        ui->doubleSpinBox_Offset->setDisabled(0);
        ui->doubleSpinBox_Phase->setDisabled(0);

        //Log("set WFG to sin",1);
        //ui->lineEdit_Amp->setText(QString::number(wfg->GetAmp(1)));
        //ui->lineEdit_Freq->setText(QString::number(wfg->GetFreq(1)));
        //ui->lineEdit_DC->setText(QString::number(wfg->GetOffset(1)));
        //ui->lineEdit_Phase->setText(QString::number(wfg->GetPhase(1)));
    }
    if( index == 1){
        wfg->SetWaveForm(wfg->ch,8); // 8 for DC
        //ui->lineEdit_DC->setText(QString::number(wfg->GetOffset(1)));
        ui->doubleSpinBox_Amp->setDisabled(1);
        ui->doubleSpinBox_Freq->setDisabled(1);
        ui->doubleSpinBox_Offset->setDisabled(0);
        ui->doubleSpinBox_Phase->setDisabled(1);

        //Log("set WFG to DC",1);
    }
    if( index == 2){
        wfg->SetWaveForm(wfg->ch,7); // 7 for noise
        ui->doubleSpinBox_Amp->setDisabled(0);
        ui->doubleSpinBox_Freq->setDisabled(1);
        ui->doubleSpinBox_Offset->setDisabled(0);
        ui->doubleSpinBox_Phase->setDisabled(1);

        //Log("set WFG to DC",1);
    }
}

void WFG_Dialog::on_doubleSpinBox_Freq_valueChanged(double arg1)
{
    wfg->SetFreq(wfg->ch, arg1*1000);
}

void WFG_Dialog::on_doubleSpinBox_Amp_valueChanged(double arg1)
{
    wfg->SetAmp(wfg->ch, arg1/1000);
}

void WFG_Dialog::on_doubleSpinBox_Offset_valueChanged(double arg1)
{
    wfg->SetOffset(wfg->ch, arg1/1000); // V

    on_pushButton_GetHPV_clicked();

    dcV.push_back(arg1/1000);
    hallV.push_back(hallVoltage);

    ReadOSCDMM();

    plot->graph(0)->clearData();
    plot->graph(0)->setData(dcV, hallV);
    plot->graph(1)->clearData();
    plot->graph(1)->setData(dcV, bField);
    plot->replot();

}

void WFG_Dialog::on_doubleSpinBox_Phase_valueChanged(double arg1)
{
    wfg->SetPhase(wfg->ch, arg1);
}

void WFG_Dialog::DisplaySetting()
{
    ui->checkBox->setChecked(wfg->IO);

    ui->comboBox_WFG->setCurrentIndex(wfg->index);

    if( wfg->index == 0){
        ui->doubleSpinBox_Freq->setValue(wfg->freq);
        ui->doubleSpinBox_Amp->setValue(wfg->amp*1000);
        ui->doubleSpinBox_Offset->setValue(wfg->offset*1000);
        ui->doubleSpinBox_Phase->setValue(wfg->phase);
    }

    if( wfg->index == 1 || wfg->index == 2){
        ui->doubleSpinBox_Offset->setValue(wfg->offset*1000);
    }
}

void WFG_Dialog::SetMagField(int ch, double mag)
{
    double DC = Mag2DC(mag);
    wfg->GoToOffset(ch, DC);

    //Check the Hall Probe reading

}

double WFG_Dialog::GetMagField()
{
    hallVoltage = hallProbe->GetReading() * 1000 ; //mV
    magField = HALLSLOPT * hallVoltage + HALLOFFSET; // mT

    Msg.sprintf("HallVolatge : %f mV, Mag-Field: %f mT", hallVoltage, magField);
    SendLogMsg(Msg);

    return magField;
}

double WFG_Dialog::Mag2DC(double mag){ // in mT to V

    return 0.005785*mag - 4.214e-6*mag*mag + 1.27533e-11*mag*mag*mag;
}

void WFG_Dialog::on_pushButton_Save_clicked()
{
    QFile saveFile("C:/Users/Triplet-ESR/Desktop/hall_measure.txt");
    saveFile.open(QIODevice::WriteOnly);

    QString line;
    QTextStream stream(&saveFile);

    //Set header
    line.sprintf("%10s, %10s, %10s \n", "DC [V]", "Hall V [mV]", "B-Field [mT]");
    stream << line;

    //Save Data;
    int n = dcV.length();

    if( n != bField.length()){
        for(int i = 0; i < n-1 ; i++){
            line.sprintf("%10f, %10f, %10f \n", dcV[i+1], hallV[i+1], bField[i]);
            stream << line;
        }
    }else{
        for(int i = 0; i < n-1 ; i++){
            line.sprintf("%10f, %10f, %10f \n", dcV[i], hallV[i], bField[i]);
            stream << line;
        }
    }

    saveFile.close();
    SendLogMsg("Data saved. C:/Users/Triplet-ESR/Desktop/hall_measure.txt");
}

void WFG_Dialog::ClearData()
{
    dcV.clear();
    hallV.clear();
    bField.clear();
}

void WFG_Dialog::on_pushButton_Clear_clicked()
{
    ClearData();
    plot->replot();
}

void WFG_Dialog::on_pushButton_Auto_clicked()
{
    int dc = ui->doubleSpinBox_Offset->text().toInt();
    int maxDC = 4500; // mV
    int incr = 10;
    int waittime = 1200; // msec

    SendLogMsg("Auto Measurement of the Hall probe volatge.");
    Msg.sprintf("Start DC: %d mV, End DC: %d mV, Incr: %d mV.", dc, maxDC, incr);
    SendLogMsg(Msg);

    ClearData();    

    bool breakFlag = 0;

    QProgressDialog progBox("Measuring Data....", "Abort.", 0, (maxDC - dc)/incr);
    progBox.setWindowModality(Qt::WindowModal);
    int waitcount = 0;
    for(int i = dc; i <= maxDC; i+=incr){
        //on_doubleSpinBox_Offset_valueChanged(i);
        ui->doubleSpinBox_Offset->setValue(i);
        Sleep(waittime);
        waitcount ++;
        progBox.setValue(waitcount);
        if( progBox.wasCanceled() ){
            breakFlag = 1;
            break;
        }
    }

    if( breakFlag){
        SendLogMsg("Breaked.");
    }else{
        SendLogMsg("Finsihed.");
    }
}

void WFG_Dialog::on_pushButton_GetHPV_clicked()
{
    GetMagField();
    ui->lineEdit_HPV->setText(QString::number(hallVoltage));
    ui->lineEdit_B->setText(QString::number(magField));

}

void WFG_Dialog::on_pushButton_GetDeviceSetting_clicked()
{
    wfg->Clear();
    int ch = ui->comboBox_ch->currentIndex() + 1;
    wfg->GetSetting(ch);
    DisplaySetting();

    on_pushButton_GetHPV_clicked();
}

void WFG_Dialog::on_pushButton_SetB_clicked()
{
    double current_mag = GetMagField();
    double mag = ui->lineEdit_B->text().toDouble(); // mT
    double dc = Mag2DC(mag)*1000; // mV

    Msg.sprintf("Set Magnetic field from %f mT to %f mT. Please wait.", current_mag, mag);
    SendLogMsg(Msg);

    Msg.sprintf("suitable DC should be %f mV", dc);
    SendLogMsg(Msg);


    int ch = ui->comboBox_ch->currentIndex() +1 ;
    wfg->GoToOffset(ch, dc);

    SendLogMsg("Fine Turning Mag Field.");
    current_mag = GetMagField();
    Msg.sprintf("diff Mag: %f mT", current_mag-mag);
    SendLogMsg(Msg);
    //compare Hall Probe
    while(current_mag > mag){
        dc = dc - 0.5;
        wfg->SetOffset(ch, dc/1000);
        Sleep(500);
        current_mag = GetMagField();
        Msg.sprintf("diff Mag: %f mT", current_mag-mag);
        SendLogMsg(Msg);
    }
    while(current_mag < mag){
        dc = dc + 0.5;
        wfg->SetOffset(ch, dc/1000);
        Sleep(500);
        current_mag = GetMagField();
        Msg.sprintf("diff Mag: %f mT", current_mag-mag);
        SendLogMsg(Msg);
    }

    wfg->GetSetting(ch);
    DisplaySetting();

}
