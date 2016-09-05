#include "osc_dialog.h"
#include "ui_osc_dialog.h"

osc_Dialog::osc_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::osc_Dialog),
    osc(NULL)
{
    ui->setupUi(this);

    osc = new Oscilloscope(KEYSIGHTDSOX3024T,0);
    //osc->Initialize(1);
    osc->SetRemoteLog(1);
    on_checkBox_Lock_clicked(1);//get osc status

    if( osc->sta == VI_SUCCESS ) {
        ui->lineEdit->setText(QString(osc->name));
    }else{
        ui->lineEdit->setText("no Oscilloscope detected.");
    }

}

osc_Dialog::~osc_Dialog()
{
    delete ui;
    delete osc;
}

void osc_Dialog::on_comboBox_currentIndexChanged(int index)
{
    ui->comboBox->setCurrentIndex(index);
    osc->SetAcqMode(index);
    switch (index){
        case 0:
            ui->spinBox->setEnabled(0);
            break;
        case 1:
            ui->spinBox->setEnabled(1);
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
    switch (osc->trgCh) {
    case 1:
        ui->radioButton_1->setChecked(1);
        break;
    case 2:
        ui->radioButton_2->setChecked(1);
        break;
    case 3:
        ui->radioButton_3->setChecked(1);
        break;
    case 4:
        ui->radioButton_4->setChecked(1);
        break;
    }

    if( ch == 1){
        ui->lineEdit_1_offset->setText(QString::number(osc->xOffset[ch]));
        ui->lineEdit_1_range->setText(QString::number(osc->xRange[ch]));
        ui->lineEdit_1_TL->setText(QString::number(osc->trgLevel[ch]));
        ui->comboBox_1_ohm->setCurrentIndex(osc->ohm[ch]);

        ui->checkBox_1_IO->setChecked(osc->IO[ch]);
        ui->lineEdit_1_range->setEnabled(osc->IO[ch]);
        ui->lineEdit_1_offset->setEnabled(osc->IO[ch]);
        ui->lineEdit_1_TL->setEnabled(osc->IO[ch]);
        ui->comboBox_1_ohm->setEnabled(osc->IO[ch]);

    }
    if( ch == 2){
        ui->lineEdit_2_offset->setText(QString::number(osc->xOffset[ch]));
        ui->lineEdit_2_range->setText(QString::number(osc->xRange[ch]));
        ui->lineEdit_2_TL->setText(QString::number(osc->trgLevel[ch]));
        ui->comboBox_2_ohm->setCurrentIndex(osc->ohm[ch]);

        ui->checkBox_2_IO->setChecked(osc->IO[ch]);
        ui->lineEdit_2_range->setEnabled(osc->IO[ch]);
        ui->lineEdit_2_offset->setEnabled(osc->IO[ch]);
        ui->lineEdit_2_TL->setEnabled(osc->IO[ch]);
        ui->comboBox_2_ohm->setEnabled(osc->IO[ch]);
    }
    if( ch == 3){
        ui->lineEdit_3_offset->setText(QString::number(osc->xOffset[ch]));
        ui->lineEdit_3_range->setText(QString::number(osc->xRange[ch]));
        ui->lineEdit_3_TL->setText(QString::number(osc->trgLevel[ch]));
        ui->comboBox_3_ohm->setCurrentIndex(osc->ohm[ch]);

        ui->checkBox_3_IO->setChecked(osc->IO[ch]);
        ui->lineEdit_3_range->setEnabled(osc->IO[ch]);
        ui->lineEdit_3_offset->setEnabled(osc->IO[ch]);
        ui->lineEdit_3_TL->setEnabled(osc->IO[ch]);
        ui->comboBox_3_ohm->setEnabled(osc->IO[ch]);
    }
    if( ch == 4){
        ui->lineEdit_4_offset->setText(QString::number(osc->xOffset[ch]));
        ui->lineEdit_4_range->setText(QString::number(osc->xRange[ch]));
        ui->lineEdit_4_TL->setText(QString::number(osc->trgLevel[ch]));
        ui->comboBox_4_ohm->setCurrentIndex(osc->ohm[ch]);

        ui->checkBox_4_IO->setChecked(osc->IO[ch]);
        ui->lineEdit_4_range->setEnabled(osc->IO[ch]);
        ui->lineEdit_4_offset->setEnabled(osc->IO[ch]);
        ui->lineEdit_4_TL->setEnabled(osc->IO[ch]);
        ui->comboBox_4_ohm->setEnabled(osc->IO[ch]);
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


void osc_Dialog::on_pushButton_Reset_clicked()
{
    osc->SetPreset();
}

void osc_Dialog::on_checkBox_Touch_clicked(bool checked)
{
    osc->SetTouch(checked);
}

void osc_Dialog::DisplaySystemStatus()
{
    ui->checkBox_Touch->setChecked(osc->touchFlag);
    ui->checkBox_Lock->setChecked(osc->lockFlag);
}

void osc_Dialog::on_checkBox_Lock_clicked(bool checked) // like getting System Status
{
    osc->SystemLock(checked);

    if(!checked) return;

    osc->GetTime(); DisplayTime();
    osc->GetSystemStatus(); DisplaySystemStatus();

    osc->GetChannelData(1); DisplayChannel(1);
    osc->GetChannelData(2); DisplayChannel(2);
    osc->GetChannelData(3); DisplayChannel(3);
    osc->GetChannelData(4); DisplayChannel(4);
    DisplayTrigger();

    if( checked) {
        ui->checkBox_Touch->setEnabled(0);
    }else{
        ui->checkBox_Touch->setEnabled(1);
    }

    if( osc->acqFlag){ //acqFlag = True = average
        on_comboBox_currentIndexChanged(1);
    }else{
        on_comboBox_currentIndexChanged(0);
    }
}

void osc_Dialog::on_pushButton_Clear_clicked()
{
    osc->Clear();
}

void osc_Dialog::on_checkBox_1_IO_clicked(bool checked)
{
    ui->lineEdit_1_range->setEnabled(checked);
    ui->lineEdit_1_offset->setEnabled(checked);
    ui->lineEdit_1_TL->setEnabled(checked);
    ui->comboBox_1_ohm->setEnabled(checked);

    double range  = ui->lineEdit_1_range->text().toDouble();
    double offset = ui->lineEdit_1_offset->text().toDouble();
    bool      ohm = ui->comboBox_1_ohm->currentIndex(); //index 0 = 50, 1 = 1M

    osc->OpenCh(1, checked);
    if(checked) osc->SetVoltage(1, range, offset, !ohm);
}

void osc_Dialog::on_checkBox_2_IO_clicked(bool checked)
{
    ui->lineEdit_2_range->setEnabled(checked);
    ui->lineEdit_2_offset->setEnabled(checked);
    ui->lineEdit_2_TL->setEnabled(checked);
    ui->comboBox_2_ohm->setEnabled(checked);

    double range  = ui->lineEdit_2_range->text().toDouble();
    double offset = ui->lineEdit_2_offset->text().toDouble();
    bool      ohm = ui->comboBox_2_ohm->currentIndex(); //index 0 = 50, 1 = 1M

    osc->OpenCh(2, checked);
    if(checked) osc->SetVoltage(2, range, offset, !ohm);

}

void osc_Dialog::on_checkBox_3_IO_clicked(bool checked)
{
    ui->lineEdit_3_range->setEnabled(checked);
    ui->lineEdit_3_offset->setEnabled(checked);
    ui->lineEdit_3_TL->setEnabled(checked);
    ui->comboBox_3_ohm->setEnabled(checked);

    double range  = ui->lineEdit_3_range->text().toDouble();
    double offset = ui->lineEdit_3_offset->text().toDouble();
    bool      ohm = ui->comboBox_3_ohm->currentIndex(); //index 0 = 50, 1 = 1M

    osc->OpenCh(3, checked);
    if(checked) osc->SetVoltage(3, range, offset, !ohm);
}

void osc_Dialog::on_checkBox_4_IO_clicked(bool checked)
{
    ui->lineEdit_4_range->setEnabled(checked);
    ui->lineEdit_4_offset->setEnabled(checked);
    ui->lineEdit_4_TL->setEnabled(checked);
    ui->comboBox_4_ohm->setEnabled(checked);

    double range  = ui->lineEdit_4_range->text().toDouble();
    double offset = ui->lineEdit_4_offset->text().toDouble();
    bool      ohm = ui->comboBox_4_ohm->currentIndex(); //index 0 = 50, 1 = 1M

    osc->OpenCh(4, checked);
    if(checked) osc->SetVoltage(4, range, offset, !ohm);

}

void osc_Dialog::on_lineEdit_timeRange_returnPressed()
{
    double range = ui->lineEdit_timeRange->text().toDouble() * 1e-6;
    double delay = ui->lineEdit_timeDelay->text().toDouble() * 1e-6;
    osc->SetTime(range, delay);
}

void osc_Dialog::on_lineEdit_timeDelay_returnPressed()
{
    on_lineEdit_timeRange_returnPressed();
}

void osc_Dialog::on_lineEdit_1_TL_returnPressed()
{
    double TL = ui->lineEdit_1_TL->text().toDouble();
    osc->SetTriggerLevel(1, TL);
}

void osc_Dialog::on_lineEdit_2_TL_returnPressed()
{
    double TL = ui->lineEdit_2_TL->text().toDouble();
    osc->SetTriggerLevel(2, TL);
}

void osc_Dialog::on_lineEdit_3_TL_returnPressed()
{
    double TL = ui->lineEdit_3_TL->text().toDouble();
    osc->SetTriggerLevel(3, TL);
}

void osc_Dialog::on_lineEdit_4_TL_returnPressed()
{
    double TL = ui->lineEdit_4_TL->text().toDouble();
    osc->SetTriggerLevel(4, TL);
}

void osc_Dialog::on_radioButton_1_clicked()
{
    osc->SetTrigger(1);
}

void osc_Dialog::on_radioButton_2_clicked()
{
    osc->SetTrigger(2);
}

void osc_Dialog::on_radioButton_3_clicked()
{
    osc->SetTrigger(3);
}

void osc_Dialog::on_radioButton_4_clicked()
{
    osc->SetTrigger(4);
}
