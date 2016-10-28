#ifndef WFG_DIALOG_H
#define WFG_DIALOG_H

#include <QDialog>
#include <QDebug>
#include <QVector>
#include <QFile>
#include "qcustomplot.h"
#include "qscpi.h"
#include "wavefromgenerator.h"
#include "dmm.h"
#include "constant.h"

namespace Ui {
class WFG_Dialog;
}

class WFG_Dialog : public QDialog
{
    Q_OBJECT

public:
    bool chFlag;

    QString Msg;

    WaveFromGenerator *wfg;
    DMM *hallProbe;

    explicit WFG_Dialog(QWidget *parent = 0);
    ~WFG_Dialog();

    void OpenHallProbe();
signals:

    void SendLogMsg(QString msg);
    void ReadOSCDMM();

public slots:
    void on_checkBox_clicked(bool checked);

    void on_comboBox_ch_activated(int index);
    void on_comboBox_WFG_activated(int index);

    void on_doubleSpinBox_Freq_valueChanged(double arg1);
    void on_doubleSpinBox_Amp_valueChanged(double arg1);
    void on_doubleSpinBox_Offset_valueChanged(double arg1);
    void on_doubleSpinBox_Phase_valueChanged(double arg1);

    void DisplaySetting();

    void SAVEOSCDMM(double dvm);
    void ClearData();

private slots:
    void on_pushButton_Save_clicked();

    void on_pushButton_Clear_clicked();

private:
    Ui::WFG_Dialog *ui;

    QCustomPlot *plot;

    QVector<double> dcV;
    QVector<double> hallV;
    QVector<double> bField;

};

#endif // WFG_DIALOG_H
