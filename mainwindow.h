#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "wfg_dialog.h"
#include "osc_dialog.h"
#include <QFile>
#include <QString>
#include <QDateTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //void on_comboBox_activated(int index);
    //void on_lineEdit_DC_returnPressed();
    //void on_lineEdit_Freq_returnPressed();
    //void on_lineEdit_Amp_returnPressed();
    //void on_lineEdit_Phase_returnPressed();
    //void on_checkBox_clicked(bool checked);
    //void on_pushButton_clicked();

    void on_actionWave_From_Generator_triggered();

    void on_actionOscilloscope_triggered();

private:
    Ui::MainWindow *ui;
    WFG_Dialog *wfgui;

    osc_Dialog *oscui;

    void Log(QString str, bool end);
};

#endif // MAINWINDOW_H
