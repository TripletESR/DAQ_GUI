#ifndef OSC_DIALOG_H
#define OSC_DIALOG_H

#include <QDialog>
#include <QDebug>
#include "qscpi.h"
#include "oscilloscope.h"
#include "constant.h"

namespace Ui {
class osc_Dialog;
}

class osc_Dialog : public QDialog
{
    Q_OBJECT

public:

    QString Msg;
    Oscilloscope *osc;

    explicit osc_Dialog(QWidget *parent = 0);
    ~osc_Dialog();

signals:

    void SendLogMsg(QString msg);
    void SendDMM(double dmm);

public slots:
    void DisplaySystemStatus();

    void on_comboBox_currentIndexChanged(int index);
    void on_spinBox_valueChanged(int arg1);
    void on_pushButton_Reset_clicked();
    void on_checkBox_Touch_clicked(bool checked);
    void on_checkBox_Lock_clicked(bool checked); // like getting System Status
    void on_checkBox_Log_clicked(bool checked);
    void on_pushButton_Clear_clicked();

    void on_checkBox_1_IO_clicked(bool checked);
    void on_checkBox_2_IO_clicked(bool checked);
    void on_checkBox_3_IO_clicked(bool checked);
    void on_checkBox_4_IO_clicked(bool checked);

    void on_lineEdit_timeRange_returnPressed();
    void on_lineEdit_timeDelay_returnPressed();

    void on_lineEdit_1_TL_returnPressed();
    void on_lineEdit_2_TL_returnPressed();
    void on_lineEdit_3_TL_returnPressed();
    void on_lineEdit_4_TL_returnPressed();

    void on_radioButton_1_clicked();
    void on_radioButton_2_clicked();
    void on_radioButton_3_clicked();
    void on_radioButton_4_clicked();

    void on_lineEdit_1_range_returnPressed();
    void on_lineEdit_1_offset_returnPressed();
    void on_comboBox_1_ohm_currentIndexChanged(int index);

    void on_lineEdit_2_range_returnPressed();
    void on_lineEdit_2_offset_returnPressed();
    void on_comboBox_2_ohm_currentIndexChanged(int index);

    void on_lineEdit_3_range_returnPressed();
    void on_lineEdit_3_offset_returnPressed();
    void on_comboBox_3_ohm_currentIndexChanged(int index);

    void on_lineEdit_4_range_returnPressed();
    void on_lineEdit_4_offset_returnPressed();
    void on_comboBox_4_ohm_currentIndexChanged(int index);

    void on_checkBox_DVM_clicked(bool checked);
    void on_pushButton_GetDVM_clicked();

    void on_comboBox_DVM_Mode_currentIndexChanged(int index);
    void on_comboBox_DVM_ch_currentIndexChanged(int index);

    void on_radioButton_ext_clicked();

private slots:




    void on_pushButton_getTrigRate_clicked();

private:
    Ui::osc_Dialog *ui;

    void OpenChannel(int ch, bool checked);

    void DisplayTime();
    void DisplayChannel(int ch);
    void DisplayTrigger();
    void DisplayTouch();

    void EnableControl(bool flag);  //Not finsihed

};

#endif // OSC_DIALOG_H
