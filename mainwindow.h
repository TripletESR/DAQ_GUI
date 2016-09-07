#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "wfg_dialog.h"
#include "osc_dialog.h"
#include "qcustomplot.h"
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

    void on_actionWave_From_Generator_triggered();
    void on_actionOscilloscope_triggered();
    void on_pushButton_clicked();
    void GetDataAndPlot(QCustomPlot *Plot, int ch);

private:
    Ui::MainWindow *ui;

    WFG_Dialog *wfgui;
    osc_Dialog *oscui;

    QCustomPlot *customPlot;

    void Log(QString str, bool end);
};

#endif // MAINWINDOW_H
