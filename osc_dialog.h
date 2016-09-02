#ifndef OSC_DIALOG_H
#define OSC_DIALOG_H

#include <QDialog>
#include <QDebug>
#include "scpi.h"
#include "oscilloscope.h"
#include "constant.h"

namespace Ui {
class osc_Dialog;
}

class osc_Dialog : public QDialog
{
    Q_OBJECT

public:

    Oscilloscope *osc;

    explicit osc_Dialog(QWidget *parent = 0);
    ~osc_Dialog();

private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_spinBox_valueChanged(int arg1);


private:
    Ui::osc_Dialog *ui;

    void DisplayTime();
    void DisplayChannel(int ch);
    void DisplayTrigger();
    void DisplayTouch();
};

#endif // OSC_DIALOG_H
