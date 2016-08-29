#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    WaveFromGenerator wfg(KEYSIGHT33500B);

    wfg.SetSinWave(1,1234,0.1,0,30);
    wfg.OpenCh(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}
