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
    osc->SetAcqMode(index);
    switch (index){
        case 0: ui->spinBox->setDisabled(1);break;
        case 1: ui->spinBox->setDisabled(0);break;
    }
}

void osc_Dialog::on_spinBox_valueChanged(int arg1)
{
    osc->SetAverage(arg1);
}
