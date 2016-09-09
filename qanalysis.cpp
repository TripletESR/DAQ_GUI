#include "qanalysis.h"

QAnalysis::QAnalysis(QObject *parent) : QObject(parent)
{

}

QAnalysis::~QAnalysis(){

}

double QAnalysis::cum_tDis30(double x)
{
    return 1/(1+exp(-x/0.6));
}

double QAnalysis::mean(QVector<double> y)
{

}

double QAnalysis::variance(QVector<double> y)
{

}

void QAnalysis::SaveFitResult()
{

}
