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
    explicit osc_Dialog(QWidget *parent = 0);
    ~osc_Dialog();

private:
    Ui::osc_Dialog *ui;

    Oscilloscope *osc;
};

#endif // OSC_DIALOG_H
