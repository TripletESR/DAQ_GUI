#include "osc_dialog.h"
#include "ui_osc_dialog.h"

osc_Dialog::osc_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::osc_Dialog),
    osc(NULL)
{
    ui->setupUi(this);

    osc = new Oscilloscope(KEYSIGHTDSOX3024T,0);
    osc->Initialize(1);
    ui->spinBox->setDisabled(1);
    osc->GetTime();
    osc->GetChannelData(1); DisplayChannel(1);
    osc->GetChannelData(2); DisplayChannel(2);
    osc->GetChannelData(3); DisplayChannel(3);
    osc->GetChannelData(4); DisplayChannel(4);

    if( osc->sta == VI_SUCCESS ) {
        ui->lineEdit->setText(QString(osc->name));
    }else{
        ui->lineEdit->setText("no Oscilloscope detected.");
    }

    DisplayTime();
    DisplayTrigger();

}

osc_Dialog::~osc_Dialog()
{
    delete ui;
    delete osc;
}

void osc_Dialog::on_comboBox_currentIndexChanged(int index)
{
    osc->SetAcqMode(index);
    switch (index){
        case 0:
            ui->spinBox->setDisabled(1);
            break;
        case 1:
            ui->spinBox->setDisabled(0);
            ui->spinBox->setValue(osc->count);
            break;
    }
}

void osc_Dialog::on_spinBox_valueChanged(int arg1)
{
    osc->SetAverage(arg1);
}

void osc_Dialog::DisplayTime()
{
    ui->lineEdit_timeRange->setText(QString::number(osc->tRange));
    ui->lineEdit_timeDelay->setText(QString::number(osc->tDelay));
}

void osc_Dialog::DisplayChannel(int ch)
{
    if( ch == 1){
        ui->lineEdit_1_offset->setText(QString::number(osc->xOffset[ch]));
        ui->lineEdit_1_range->setText(QString::number(osc->xRange[ch]));
        ui->lineEdit_1_TL->setText(QString::number(osc->trgLevel[ch]));
        if(osc->IO[ch]) ui->checkBox_1_IO->setChecked(1);
        ui->comboBox_1_ohm->setCurrentIndex(osc->ohm[ch]);
    }
    if( ch == 2){
        ui->lineEdit_2_offset->setText(QString::number(osc->xOffset[ch]));
        ui->lineEdit_2_range->setText(QString::number(osc->xRange[ch]));
        ui->lineEdit_2_TL->setText(QString::number(osc->trgLevel[ch]));
        if(osc->IO[ch]) ui->checkBox_2_IO->setChecked(1);
        ui->comboBox_2_ohm->setCurrentIndex(osc->ohm[ch]);
    }
    if( ch == 3){
        ui->lineEdit_3_offset->setText(QString::number(osc->xOffset[ch]));
        ui->lineEdit_3_range->setText(QString::number(osc->xRange[ch]));
        ui->lineEdit_3_TL->setText(QString::number(osc->trgLevel[ch]));
        if(osc->IO[ch]) ui->checkBox_3_IO->setChecked(1);
        ui->comboBox_3_ohm->setCurrentIndex(osc->ohm[ch]);
    }
    if( ch == 4){
        ui->lineEdit_4_offset->setText(QString::number(osc->xOffset[ch]));
        ui->lineEdit_4_range->setText(QString::number(osc->xRange[ch]));
        ui->lineEdit_4_TL->setText(QString::number(osc->trgLevel[ch]));
        if(osc->IO[ch]) ui->checkBox_3_IO->setChecked(1);
        ui->comboBox_4_ohm->setCurrentIndex(osc->ohm[ch]);
    }
}

void osc_Dialog::DisplayTrigger()
{
    switch (osc->trgCh){
        case 1: ui->radioButton_1->setChecked(1); break;
        case 2: ui->radioButton_2->setChecked(1); break;
        case 3: ui->radioButton_3->setChecked(1); break;
        case 4: ui->radioButton_4->setChecked(1); break;
    }
}

//void osc_Dialog::DisplayTouch()
//{
//
//}
