#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <QObject>
#include <QVector>
#include <QString>
#include <cmath>
#include <math.h>
#include "matrix.h"

class Analysis : public QObject
{
    Q_OBJECT
public:
    QString Msg;

    explicit Analysis(QObject *parent = 0);
    Analysis(const QVector<double> x, const QVector<double> y);

    void SetData(const QVector<double> x, const QVector<double> y);

    double Mean(int index_1, int index_2);
    double Variance(int index_1, int index_2);
    Matrix *Regression(bool fitType, int startFitIndex, QVector<double> par);
    Matrix *NonLinearFit(int startFitIndex, QVector<double> iniPar);

signals:

    void SendMsg(QString msg);

public slots:
    int GetDataSize(){ return n;}
    int GetParametersSize() {return p;}
    int GetDegreeOfFreedom() {return DF;}
    QVector<double> GetData_x() {return xdata;}
    QVector<double> GetData_y() {return ydata;}
    QVector<double> GetParameters() {return par;}
    QVector<double> GetParError() {return error;}
    QVector<double> GetParPValue() {return pValue;}
    double GetSSR() {return SSR;}
    double GetFitSigma() {return sigma;}

private:
    QVector<double> xdata, ydata;
    QVector<double> par;
    QVector<double> error;
    QVector<double> pValue;
    double SSR;
    double sigma; //fit sigma
    int n, p, DF;

    double cum_tDis30(double x){
        return 1/(1+exp(-x/0.6));
    }

    double fitFunction(bool fitType, double x, QVector<double> par){
        double fit = 0;
        if(fitType == 1 ) fit = par[0] * exp(-x/par[1]) + par[2] * exp(-x/par[3]);
        if(fitType == 0 ) fit = par[0] * exp(-x/par[1]);
        return fit;
    }

    QVector<double> RowVector2QVector(Matrix m){
        QVector<double> out;
        for(int i = 1; i <= m.GetRows(); i++){
            out.push_back(m(i,1));
        }
        return out;
    }
};

#endif // ANALYSIS_H
