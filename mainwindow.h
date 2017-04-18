#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDir>
#include <QEventLoop>
#include <QCloseEvent>
#include "qfileio.h"
#include "wfg_dialog.h"
#include "osc_dialog.h"
#include "qcustomplot.h"
#include <QFileDialog>
#include <QString>
#include <QDateTime>
#include <QtSql>
#include "analysis.h"

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
    void closeEvent(QCloseEvent * event);
    void keyPressEvent(QKeyEvent* keyEvent);
    void keyReleaseEvent(QKeyEvent* keyEvent);

    int loadConfigurationFile();

    void TryToBreakAutoDAQ();

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

    void on_pushButton_Auto_clicked();
    void on_pushButton_Save_clicked();
    //void on_spinBox_count_valueChanged(int arg1);
    void on_pushButton_openFile_clicked();

    void Write2Log(QString msg);
    void on_lineEdit_start_editingFinished();
    void on_lineEdit_end_editingFinished();
    void on_lineEdit_step_editingFinished();

    void SetProgressBar(double value);
    void WhenOSCReady(QString msg);

    void on_comboBox_Fit_currentIndexChanged(int index);
    void on_comboBox_points_currentTextChanged(const QString &arg1);

    QStringList GetTableColEntries(QString tableName, int col);
    void updateChemicalCombox();
    void updateSampleCombox();
    void updateLaserCombox();

    void on_comboBox_Chemical_currentIndexChanged(int index);
    void on_comboBox_Sample_currentIndexChanged(int index);
    void on_comboBox_laser_currentIndexChanged(int index);

    void on_pushButton_ComfirmSelection_clicked();

    void on_lineEdit_Temperature_editingFinished();
    void on_lineEdit_DataComment_editingFinished();

    void on_actionCloseProgram_triggered();
    void updatePlannel();

private:
    Ui::MainWindow *ui;

    WFG_Dialog *wfgui;
    osc_Dialog *oscui;

    QFileIO * logFile;
    QFileIO * dataFile;

    QCPPlotTitle * plotTitle;
    QCustomPlot * plot;

    Analysis *ana;

    QProgressDialog * progress;
    bool breakAutoDAQFlag;

    QSqlDatabase db;
    QSqlTableModel *dbTable;

    int openFlag;

    const QString DataCommentStr = "<Comment: Laser Power, uW freq, power, type>";

    QStringList chemIDList;
    QStringList sampleIDList;
    QStringList laserIDList;
    bool enableUpdateSample;
    bool canCloseProgram;

};

#endif // MAINWINDOW_H
