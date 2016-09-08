#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "fileio.h"
#include "wfg_dialog.h"
#include "osc_dialog.h"
#include "qcustomplot.h"
#include <QFileDialog>

//#include <QFile>
#include <QString>
//#include <QDateTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString LogMsg;

private slots:

    void on_actionWave_From_Generator_triggered();
    void on_actionOscilloscope_triggered();
    void on_pushButton_clicked();
    void GetDataAndPlot(QCustomPlot *Plot, int ch);

    void on_pushButton_openFile_clicked();

private:
    Ui::MainWindow *ui;

    WFG_Dialog *wfgui;
    osc_Dialog *oscui;

    FileIO * logFile;
    FileIO * dataFile;

    QCustomPlot *customPlot;

    void Log(QString logMsg);


};

#endif // MAINWINDOW_H
