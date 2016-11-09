#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <QObject>
#include <QDebug>
#include <QVector>
#include "matrix.h"

class Analysis : public QObject
{
    Q_OBJECT
public:
    explicit Analysis(QObject *parent = 0);
    ~Analysis();

    void SetData(QVector<double> x, QVector<double> y);
    QVector<double> EstimatePar(int i);
    int FindXIndex(double goal);
    int LMA(QVector<double> par0, double lambda0);
    int NonLinearFit(int i);
    void MeanAndvariance(int index_1, int index_2);
    int Regression(QVector<double> par0);

    double GetSampleMean(){return sampleMean;}

    void CalFitFunction();
    QVector<double> GetFitFuncVector(){return fitYData;}
    QVector<double> GetXDataVector(){return xData;}
    void PrintVector(QVector<double> vec, QString str);

signals:
    SendMsg(QString msg);

public slots:

private:
    QVector<double> xData, yData;
    QVector<double> fitYData;

    double sampleMean, sampleVariance;
    int dataSize, parSize, DF;
    int xStartIndex;

    double torr, torrGrad, lambda;
    double delta, SSR;
    int maxIter;

    int fitFlag;

    QVector<double> sol;
    QVector<double> dpar;
    QVector<double> error;
    QVector<double> gradSSR;

    Matrix CoVar;

    double FitFunc(double x, QVector<double> par){
        double fit = 0;
        fit = par[0] * exp(-x/par[1]);          // when a*Exp[t/Ta]
        if( par.size() == 3) fit += par[2];     // when a*Exp[t/Ta] + c
        if( par.size() == 4) fit += par[2] * exp(-x/par[3]); // when a*Exp[t/Ta] + b*Exp[t/Tb]
        if( par.size() == 5) fit += par[2] * exp(-x/par[3]) + par[4];     // when a*Exp[t/Ta] + b*Exp[t/Ta] + c
        return fit;
    }

    QVector<double> GradFitFunc(double x, QVector<double> par){
        QVector<double> gradFit;

        gradFit.push_back(exp(-x/par[1]));
        gradFit.push_back(par[0] * x* exp(-x/par[1])/par[1]/par[1]);
        if(par.size() == 3) gradFit.push_back(1); // in case of c

        if( par.size() == 4){
            gradFit.push_back(exp(-x/par[3]));
            gradFit.push_back(par[2] * x* exp(-x/par[3])/par[3]/par[3]);
        }
        if( par.size() == 5) {
            gradFit.push_back(exp(-x/par[3]));
            gradFit.push_back(par[2] * x* exp(-x/par[3])/par[3]/par[3]);
            gradFit.push_back(1); // in case of c
        }
        return gradFit;
    }


};

#endif // ANALYSIS_H
