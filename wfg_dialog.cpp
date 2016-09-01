#include "wfg_dialog.h"
#include "ui_wfg_dialog.h"

WFG_Dialog::WFG_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WFG_Dialog),
    wfg(NULL)
{
    ui->setupUi(this);
    ui->comboBox_ch->addItem("channel 1");
    ui->comboBox_ch->addItem("channel 2");
    //ui->comboBox_ch->setCurrentIndex(0);

    ui->comboBox_WFG->addItem("Sin wave");
    ui->comboBox_WFG->addItem("DC");
    ui->comboBox_WFG->addItem("Noise");
    //ui->comboBox_WFG->setCurrentIndex(0);

    wfg = new WaveFromGenerator(KEYSIGHT33500B);
    wfg->ch = 1; // default channel 1
    wfg->wf = 1; // default sin
    wfg->GetSetting(wfg->ch);
    DisplaySetting();

    wfg->GetWaveForm(1);

    ui->lineEdit->setText(QString(wfg->name));
}

WFG_Dialog::~WFG_Dialog()
{
    delete ui;
    delete wfg;
}


void WFG_Dialog::on_comboBox_ch_activated(int index)
{
    switch(index){
        case 0: wfg->ch = 1; break;
        case 1: wfg->ch = 2; break;
    }

    wfg->GetSetting(wfg->ch);
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
        wfg->SetWaveForm(1,1); // 1 for Sin
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
        wfg->SetWaveForm(1,8); // 8 for DC
        //ui->lineEdit_DC->setText(QString::number(wfg->GetOffset(1)));
        ui->doubleSpinBox_Amp->setDisabled(1);
        ui->doubleSpinBox_Freq->setDisabled(1);
        ui->doubleSpinBox_Offset->setDisabled(0);
        ui->doubleSpinBox_Phase->setDisabled(1);

        //Log("set WFG to DC",1);
    }
    if( index == 2){
        wfg->SetWaveForm(1,7); // 7 for noise
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
    wfg->SetOffset(wfg->ch, arg1/1000);
}

void WFG_Dialog::on_doubleSpinBox_Phase_valueChanged(double arg1)
{
    wfg->SetPhase(wfg->ch, arg1);
}

void WFG_Dialog::DisplaySetting()
{
    ui->checkBox->setChecked(wfg->IO);
    ui->doubleSpinBox_Freq->setValue(wfg->freq/1000);
    ui->doubleSpinBox_Amp->setValue(wfg->amp*1000);
    ui->doubleSpinBox_Offset->setValue(wfg->offset*1000);
    ui->doubleSpinBox_Phase->setValue(wfg->phase);
    ui->comboBox_WFG->setCurrentIndex(wfg->wf);
}
