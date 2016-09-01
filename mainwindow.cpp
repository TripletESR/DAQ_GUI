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
    oscui->osc->GetData(1, 100);

    customPlot = ui->customPlot;
    customPlot->addGraph();

    QVector<double> xdat(100);
    QVector<double> ydat(100);
    for (int i = 0; i < 100; i++){
        xdat[i] = oscui->osc->xData[i]; //TODO change data type to Qvector
        ydat[i] = oscui->osc->yData[i];

    }

    customPlot->graph(0)->setData(xdat, ydat);
    customPlot->xAxis->setLabel("time [us]");
    customPlot->yAxis->setLabel("Volatge [V]");
    customPlot->xAxis->setRange(-60,100);
    customPlot->yAxis->setRange(-1,1);

    customPlot->replot();
}
