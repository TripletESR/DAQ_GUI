#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qfileio.h"
#include "wfg_dialog.h"
#include "osc_dialog.h"
#include "qcustomplot.h"
#include "analysis.h"
#include <QFileDialog>

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

    int MsgCount;
    QString logMsg;

private slots:

    void on_actionWave_From_Generator_triggered();
    void on_actionOscilloscope_triggered();
    void on_pushButton_clicked();
    void GetDataAndPlot(QCustomPlot *Plot, int ch);

    void on_pushButton_openFile_clicked();

    void Write2Log(QString msg);

private:
    Ui::MainWindow *ui;

    WFG_Dialog *wfgui;
    osc_Dialog *oscui;

    QFileIO * logFile;
    QFileIO * dataFile;

    QCustomPlot *customPlot;

};

#endif // MAINWINDOW_H
