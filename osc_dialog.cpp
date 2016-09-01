#include "osc_dialog.h"
#include "ui_osc_dialog.h"

osc_Dialog::osc_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::osc_Dialog),
    osc(NULL)
{
    ui->setupUi(this);

    osc = new Oscilloscope(KEYSIGHTDSOX3024T);

    ui->lineEdit->setText(QString(osc->name));
}

osc_Dialog::~osc_Dialog()
{
    delete ui;
    delete osc;
}
