#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    wfgui(NULL),
    oscui(NULL),
    customPlot(NULL)
{
    ui->setupUi(this);
    wfgui = new WFG_Dialog(this);
    oscui = new osc_Dialog(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete wfgui;
    delete oscui;
    delete customPlot;
}

void MainWindow::Log(QString str, bool end)
{
    QString path = QCoreApplication::applicationDirPath();
    //qDebug()<<path;
    path.append("/outlog.txt");
    //qDebug()<<path;
    QFile myfile(path);
    myfile.open(QIODevice::Append | QIODevice::Text);

    QDateTime date = QDateTime::currentDateTime();
    QString text = date.toString().toStdString().c_str();
    text.append(":  ");
    text.append(str);
    myfile.write(text.toStdString().c_str());

    ui->plainTextEdit->appendPlainText(text);

    if(end) myfile.write("\n");
    myfile.close();
}


void MainWindow::on_actionWave_From_Generator_triggered()
{
    if( wfgui->isHidden()){
        wfgui->setGeometry(this->geometry().x()+ this->geometry().width()+20,
                           this->geometry().y(),
                           wfgui->geometry().width(),
                           wfgui->geometry().height());
        wfgui->show();
    }
}

void MainWindow::on_actionOscilloscope_triggered()
{
    if( oscui->isHidden()){
        oscui->setGeometry(this->geometry().x()+ this->geometry().width()+20,
                           wfgui->geometry().y()+ wfgui->geometry().height()+60,
                           oscui->geometry().width(),
                           oscui->geometry().height());
        oscui->show();
    }
}

void MainWindow::on_pushButton_clicked()
{
    customPlot = ui->customPlot;

    int ch = ui->spinBox_ch->value();
    GetDataAndPlot(customPlot, ch);

}

void MainWindow::GetDataAndPlot(QCustomPlot *Plot, int ch){

    oscui->osc->GetData(ch, ui->spinBox_count->value(),oscui->osc->acqFlag);

    if( Plot->graphCount() == 0){
        Plot->xAxis->setLabel("time [us]");
        Plot->yAxis->setLabel("Volatge [V]");
        Plot->xAxis->setRange(oscui->osc->xMin,oscui->osc->xMax);
        Plot->yAxis->setRange(oscui->osc->yMin * 2, oscui->osc->yMax * 2);
    }

    double plotyMax = Plot->yAxis->range().upper;
    double plotyMin = Plot->yAxis->range().lower;
    if( oscui->osc->yMin * 2 < plotyMin ) plotyMin = oscui->osc->yMin * 2;
    if( oscui->osc->yMax * 2 > plotyMax ) plotyMax = oscui->osc->yMax * 2;
    Plot->yAxis->setRange(plotyMin, plotyMax);

    while(Plot->graphCount() < ch){
        Plot->addGraph();
    }

    switch (ch) {
    case 1:Plot->graph(ch-1)->setPen(QPen(Qt::blue)); break;
    case 2:Plot->graph(ch-1)->setPen(QPen(Qt::red)); break;
    case 3:Plot->graph(ch-1)->setPen(QPen(Qt::darkGreen)); break;
    case 4:Plot->graph(ch-1)->setPen(QPen(Qt::magenta)); break;
    }

    Plot->graph(ch-1)->setData(oscui->osc->xData[ch], oscui->osc->yData[ch]);

    Plot->replot();
}
