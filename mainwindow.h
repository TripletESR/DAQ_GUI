#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QEventLoop>
#include "qfileio.h"
#include "wfg_dialog.h"
#include "osc_dialog.h"
#include "qcustomplot.h"
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
    void on_pushButton_GetSingle_clicked();
    void GetData(int ch, int points);
    void GetBGData(int ch, int points);
    void PlotGraph(int ch, QVector<double> x,
                      QVector<double> y,
                      double xMin = 0,
                      double xMax = 0,
                      double yMin = 0,
                      double yMax = 0);
    void SaveData(QString head, QVector<double> x, QVector<double> y);

    void on_pushButton_Auto_clicked();
    void on_pushButton_Save_clicked();
    void on_spinBox_count_valueChanged(int arg1);
    void on_pushButton_openFile_clicked();

    void Write2Log(QString msg);
    void on_lineEdit_step_editingFinished();

    void SetProgressBar(double value);
    void WhenOSCReady(QString msg);

private:
    Ui::MainWindow *ui;

    WFG_Dialog *wfgui;
    osc_Dialog *oscui;

    QFileIO * logFile;
    QFileIO * dataFile;

    QCPPlotTitle * plotTitle;
    QCustomPlot * plot;

};

#endif // MAINWINDOW_H
