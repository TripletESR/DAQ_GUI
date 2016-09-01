#ifndef WFG_DIALOG_H
#define WFG_DIALOG_H

#include <QDialog>
#include <QDebug>
#include "scpi.h"
#include "wavefromgenerator.h"
#include "constant.h"

namespace Ui {
class WFG_Dialog;
}

class WFG_Dialog : public QDialog
{
    Q_OBJECT

public:
    bool chFlag;

    explicit WFG_Dialog(QWidget *parent = 0);
    ~WFG_Dialog();

private slots:
    void on_checkBox_clicked(bool checked);

    void on_comboBox_ch_activated(int index);
    void on_comboBox_WFG_activated(int index);

    void on_doubleSpinBox_Freq_valueChanged(double arg1);
    void on_doubleSpinBox_Amp_valueChanged(double arg1);
    void on_doubleSpinBox_Offset_valueChanged(double arg1);
    void on_doubleSpinBox_Phase_valueChanged(double arg1);

    void DisplaySetting();

private:
    Ui::WFG_Dialog *ui;

    WaveFromGenerator *wfg;
};

#endif // WFG_DIALOG_H
