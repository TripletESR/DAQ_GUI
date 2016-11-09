#include "analysis.h"

Analysis::Analysis(QObject *parent) : QObject(parent)
{

}

Analysis::~Analysis()
{

}

void Analysis::SetData(QVector<double> x, QVector<double> y){
    xData = x;
    yData = y;

    if( xData.size() != yData.size()){
        //msg
        return;
    };
    dataSize = xData.size();
    xStartIndex = FindXIndex(3);

    //========= Cal. Sample mean, Variance
    int x2 = FindXIndex(-1);
    MeanAndvariance(0, x2);

    //========= Other initialization
    torr = 1e-6;
    torrGrad = 1e-3;
    lambda = -1; // -1 enable cal. Lambda
    sampleMean = 0;
    sampleVariance = 0;

}

QVector<double> Analysis::EstimatePar(int i)
{
    double a , Ta, b, Tb, c;
    //Estimate c to be the sampleMean;
    c = sampleMean;
    //Estimate Ta;
    Ta = 20;
    //Estimate Tb;
    Tb = 80;
    //Estimate a, to the the maximum
    a = yData[0];
    b = yData[0];
    for( int i = 1; i < dataSize; i++){
        if( a < yData[i]) a = yData[i];
        if( b > yData[i]) b = yData[i];
    }
    a = a - c;
    b = b - c;
    if( a < b) {
        double tmp = a;
        a = b;
        b = tmp;
    }

    QVector<double> par;
    par.push_back(a);
    par.push_back(Ta);
    if(i == 3){
        par.push_back(c);
    }else if(i == 4){
        par.push_back(b);
        par.push_back(Tb);
    }else if(i == 5){
        par.push_back(b);
        par.push_back(Tb);
        par.push_back(c);
    }

    return par;
}

int Analysis::FindXIndex(double goal){
    int xIndex = 0;
    for(int i = 0; i < dataSize ; i++){
        if( xData[i] >= goal){
            xIndex = i;
            break;
        }
    }
    return xIndex;
}

int Analysis::LMA( QVector<double> par0, double lambda0){

    lambda = lambda0;

    QString tmp, Msg;
    //PrintVector(par0, "ini. par:");
    //tmp.sprintf("Fit Torr: %7.1e, Max Iteration: %d", torr, MaxIter);SendMsg(tmp);

    int nIter = 0;
    QVector<double> par = par0;
    bool contFlag;
    Msg.sprintf(" === Start fit using Levenberg-Marquardt Algorithm: ");
    do{
        Regression(par);
        par = sol;
        nIter ++;
        if( nIter >= maxIter ) {
            fitFlag = 2; // fitFlag = 2 when iteration too many
            break;
        }
        bool converge = 0;
        //since this is 4-parameter fit
        converge = std::abs(delta) <  torr * SSR;
        for(int i = 0; i < parSize; i++){
            converge &= std::abs(gradSSR[i]) < torrGrad;
        }
        // if lambda to small or too big, reset
        //if( this->lambda < 1e-5) this->lambda = 1e+5;
        //if( this->lambda > 1e+10) this->lambda = 1e-4;
        contFlag = fitFlag == 0 && ( !converge );

    }while(contFlag);

    tmp.sprintf(" %d", nIter);
    Msg += tmp;
    if( fitFlag == 0) {
        Msg += "| End Normally.";
    }else if(fitFlag == 1){
        Msg += "| Terminated. Covariance fail to cal.";
    }else if(fitFlag == 2){
        tmp.sprintf("| Terminated. Fail to converge in %d trials.", maxIter);
        Msg += tmp;
    }
    SendMsg(Msg);

    //===== cal error
    double fitVar = SSR / DF;
    Matrix error(parSize,1);
    for( int i = 1; i <= parSize ; i++){
        error(i,1)  = sqrt(fitVar * CoVar(i,i));
    }

    this->error = error.Matrix2QVec();

    return 0;
}

int Analysis::NonLinearFit(int i)
{
    QVector<double> par = EstimatePar(i);
    LMA(par, -1);

    PrintVector(sol, "sol.");
    PrintVector(error, "error");
    PrintVector(gradSSR, "grad. SSR");

    QString msg;
    double chisq = sqrt(SSR / DF / sampleVariance);
    msg.sprintf("SSR : %f, delta: %f, chi-sqaured: %f", SSR, delta, chisq);
    SendMsg(msg);

    return 0;
}

void Analysis::MeanAndvariance(int index_1, int index_2)
{
    if( dataSize == 0 ||
        index_1 < 0 ||
        index_2 < 0 ||
        index_1 > dataSize ||
        index_1 >= index_2 ||
        index_2 > dataSize){
        //Msg.sprintf("index Error. n = %d, range = (%d, %d)", n, index_1, index_2);
        //emit SendMsg(Msg);
        return;
    }

    int size = index_2 - index_1 + 1;
    sampleMean = 0;
    for( int i = index_1 ; i <= index_2 ; i++){
        sampleMean += (this->yData)[i]/size;
    }

    sampleVariance = 0;
    for( int i = index_1 ; i <= index_2 ; i++){
        sampleVariance += pow((this->yData)[i]-sampleMean,2);
    }
    sampleVariance = sampleVariance / (size-1);

    QString Msg;
    Msg.sprintf("From index %d to %d (%d data)\nMean = %f, Variance = %f, sigma = %f", index_1, index_2, size, sampleMean, sampleVariance, sqrt(sampleVariance));
    SendMsg(Msg);

}

int Analysis::Regression(QVector<double> par0)
{
    //Levenberg-Marquardt Algorithm
    fitFlag = 0;
    int xStart = xStartIndex;
    int xEnd = dataSize - 1;
    int fitSize = xEnd - xStart + 1;

    parSize = par0.size();
    DF = fitSize - parSize;
    Matrix par_old(parSize,1); for(int i = 0; i < parSize; i++){ par_old(i+1,1) = par0[i];}

    //============================Start regression
    Matrix Y(fitSize,1);
    Matrix f(fitSize,1);
    Matrix F(fitSize,parSize); // F = grad(f)
    for(int i = 1; i <= fitSize ; i++) {
        Y(i,1) = yData[i + xStart - 1];
        double x = xData[i + xStart - 1];
        f(i,1) = FitFunc(x, par0);
        QVector<double> gradf = GradFitFunc(x, par0);
        for(int j = 1; j <= parSize; j++){
            F(i,j) = gradf[j-1];
        }
    }

    Matrix Ft = F.Transpose();
    Matrix FtF = Ft*F;

    //======== Cal lambda
    if( lambda == -1 ){
        lambda = 0;
        for(int i = 1; i <= fitSize; i++){
            for(int j = 1; j <= parSize ; j++){
                lambda += F(i,j)* F(i,j);
            }
        }
        lambda = sqrt(lambda/fitSize/parSize);
        //Msg.sprintf(" ==== cal inital lambda : %f", this->lambda);
        //SendMsg(Msg);
    }

    Matrix DI(parSize,parSize);
    for(int i = 1; i <= parSize ; i++) {DI(i,i) = lambda;}

    try{
        CoVar = (FtF + DI).Inverse();
    }catch( Exception err){
        fitFlag = 1;
        return 0; // return 1 when covariance cannot be compute.
    }

    Matrix dY = f-Y;
    SSR = (dY.Transpose() * dY)(1,1);

    Matrix FtdY = Ft*dY; // gradient of SSR
    Matrix dpar = CoVar * FtdY;
    Matrix sol = par_old + dpar;

    //========================================== Check the SSR(p+dpar)
    QVector<double> par_new = sol.Matrix2QVec();

    Matrix fn(fitSize, 1);
    Matrix Fn(fitSize, parSize);
    for(int i = 1; i <= fitSize ; i++) {
        double x = xData[i + xStart - 1];
        fn(i,1) = FitFunc(x, par_new);
        QVector<double> gradf = GradFitFunc(x, par_new);
        for(int j = 1; j <= parSize; j++){
            Fn(i,j) = gradf[j-1];
        }
    }
    Matrix dYn = fn-Y;
    double SSRn = (dYn.Transpose() * dYn)(1,1);

    delta = SSRn - SSR;

    //========== SSRn > SSR
    if( delta >= 0){
        lambda = lambda * 10;
        sol = par_old;
        this->sol = sol.Matrix2QVec();
        this->dpar = Matrix(parSize,1).Matrix2QVec(); ///Zero matrix
        this->gradSSR = FtdY.Matrix2QVec();
        return 0;
    }
    //========== SSRn < SSR

    lambda = lambda / 10;
    this->sol = sol.Matrix2QVec();
    this->dpar = dpar.Matrix2QVec();
    this->SSR = SSRn;
    //new gradient of SSR
    this->gradSSR = (Fn.Transpose()*dYn).Matrix2QVec();

    // Cal CoVar
    try{
        CoVar = (Fn.Transpose()*Fn).Inverse();
    }catch( Exception err){
        fitFlag = 1;
        return 0; // return 0 when covariance cannot be compute.
    }

    return 1;
}

void Analysis::CalFitFunction()
{
    fitYData.clear();
    for ( int i = 0; i < dataSize; i++){
        double x = xData[i];
        fitYData.push_back( FitFunc(x, sol) );
    }
}

void Analysis::PrintVector(QVector<double> vec, QString str)
{
    QString tmp, Msg;
    tmp.sprintf("%*s(%d) : [ ", 15, str.toStdString().c_str(), vec.size());
    Msg = tmp;
    for(int i = 0; i < vec.size() - 1; i++){
        tmp.sprintf(" %7.3f,", vec[i]);
        Msg += tmp;
    }
    tmp.sprintf(" %7.3f]", vec[vec.size()-1]);
    Msg += tmp;

    SendMsg(Msg);

}
