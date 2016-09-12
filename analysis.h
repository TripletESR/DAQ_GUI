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

    //explicit Analysis(QObject *parent = 0);
    explicit Analysis(const QVector<double> x, const QVector<double> y);

    void SetData(const QVector<double> x, const QVector<double> y);
    void SetStartFitIndex(int index);

    double Mean(int index_1, int index_2);
    double Variance(int index_1, int index_2);

    void Regression(const bool fitType, QVector<double> par);
    void NonLinearFit(QVector<double> par);

signals:

    void SendMsg(QString msg);

public slots:
    int GetDataSize(){ return n;}
    int GetParametersSize() {return p;}
    int GetDegreeOfFreedom() {return DF;}
    QVector<double> GetData_x() {return xdata;}
    QVector<double> GetData_y() {return ydata;}
    Matrix GetParameters() {return par;}
    Matrix GetParError() {return error;}
    Matrix GetParPValue() {return pValue;}
    double GetSSR() {return SSR;}
    double GetFitSigma() {return sigma;}
    void Connector(QString msg){
        emit SendMsg(msg);
    }

private:
    QVector<double> xdata, ydata;
    Matrix par;
    Matrix dpar;
    Matrix error;
    Matrix tDis;
    Matrix pValue;
    double SSR;
    double sigma; //fit sigma
    int n, p, DF;
    int startFitIndex;

    bool errFlag;

    double cum_tDis30(double x){
        return 1/(1+exp(-x/0.6));
    }

    double fitFunction(bool fitType, double x, QVector<double> par){
        double fit = 0;
        if(fitType == 1 ) fit = par[0] * exp(-x/par[1]) + par[2] * exp(-x/par[3]);
        if(fitType == 0 ) fit = par[0] * exp(-x/par[1]);
        return fit;
    }

    QVector<double> ColVector2QVector(Matrix m){
        QVector<double> out;
        for(int i = 1; i <= m.GetRows(); i++){
            out.push_back(m(i,1));
        }
        return out;
    }

    Matrix QVector2ColVector(QVector<double> vec){
        int n = vec.size();
        Matrix res(n,1);
        for(int i = 0; i < n; i++){
            res(i+1, 1) = vec[i];
        }
        return res;
    }

};

#endif // ANALYSIS_H
