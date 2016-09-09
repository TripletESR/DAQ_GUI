#ifndef QANALYSIS_H
#define QANALYSIS_H

#include <QObject>
//#include "Matrix.h" //Need to rewrite
#include <QString>
#include <QDebug>
#include <QVector>

class QAnalysis : public QObject
{
    Q_OBJECT
public:
    explicit QAnalysis(QObject *parent = 0);
    ~QAnalysis();

signals:

public slots:

    double cum_tDis30(double x); // cumulative t-distribution of DF = 30.
    double mean(QVector<double> y);
    double variance(QVector<double> y);

    //void Regression(bool tag, QVector<double>　par);
    //void Fitting(QVector<double> par);

    void SaveFitResult();

};

#endif // QANALYSIS_H
