#include "analysis.h"

Analysis::Analysis(QObject *parent) : QObject(parent)
{

}

Analysis::Analysis(const QVector<double> x, const QVector<double> y)
{
    SetData(x,y);
}

void Analysis::SetData(const QVector<double> x, const QVector<double> y)
{
    // assume x.size() == y.size()
    this->xdata = x;
    this->ydata = y;

    this->n = this->xdata.size();
    Msg.sprintf("Input Data, size = %d", this->n);
    emit SendMsg(Msg);

    this->startFitIndex = 0;
}

void Analysis::SetStartFitIndex(int index){
    this->startFitIndex = index;
}

double Analysis::Mean(int index_1, int index_2)
{
    if( this->n == 0 ||
            index_1 < 0 ||
            index_2 < 0 ||
        index_1 > this->n ||
        index_1 >= index_2 ||
        index_2 > this->n){
        Msg.sprintf("index Error. n = %d", n);
        emit SendMsg(Msg);
        return 0;
    }

    int size = index_2 - index_1 + 1;
    double mean = 0;
    for( int i = index_1 ; i <= index_2 ; i++){
        mean += (this->ydata)[i];
    }
    mean = mean / size;

    Msg.sprintf("Mean from index %d to %d of y-data (%d data) = %f", index_1, index_2, size, mean);
    emit SendMsg(Msg);

    return mean;
}

double Analysis::Variance(int index_1, int index_2)
{
    const double mean = Mean(index_1, index_2);

    int size = index_2 - index_1 + 1;
    double var = 0;
    for( int i = index_1 ; i <= index_2 ; i++){
        var += pow((this->ydata)[i]-mean,2);
    }
    var = var / (size-1);

    Msg.sprintf("Variance from index %d to %d of y-data (%d data) = %f, sigma = %f", index_1, index_2, size, var, sqrt(var));
    emit SendMsg(Msg);

    return var;

}

void Analysis::Regression(bool fitType, QVector<double> par)
{
    errFlag = 0;
    int xStart = this->startFitIndex;
    int xEnd = this->n-1;
    int fitSize = xStart - xEnd + 1;

    this->p = 2;//default is 2 parameters fit
    if (fitType) this->p = 4;

    this->DF = fitSize - this->p;
    this->par = QVector2ColVector(par);

    //============================Start regression
    Matrix Y(fitSize,1);
    for(int i = 1; i <= fitSize ; i++) {
        Y(i,1) = ydata[i + xStart - 1];
    }

    Matrix f(fitSize,1);
    for(int i = 1; i <= fitSize ; i++) {
        double x = xdata[i + xStart - 1];
        f(i,1) = fitFunction(fitType, x, par);
    }

    Matrix F(n,p); // F = grad(f)
    for(int i = 1; i <= n ; i++) {
        double x = xdata[i - 1 + xStart];
        F(i,1) = exp(-x/par[1]);
        F(i,2) = par[0] * x * exp(-x/par[1])/par[1]/par[1];
        if( fitType ) F(i,3) = exp(-x/par[3]);
        if( fitType ) F(i,4) = par[2] * x * exp(-x/par[3])/par[3]/par[3];
    }

    Matrix Ft = F.Transpose(); //printf("    Ft(%d,%d)\n", Ft.GetRows(), Ft.GetCols());
    Matrix FtF = Ft*F;        //printf("   FtF(%d,%d)\n", FtF.GetRows(), FtF.GetCols());

    Matrix CoVar;
    try{
        CoVar = FtF.Inverse();  //printf(" CoVar(%d,%d)\n", CoVar.GetRows(), CoVar.GetCols());
    }catch( Exception err){
        errFlag = 1;
        return;
    }

    //CoVar.Print();

    Matrix dY = Y - f;    //printf("    dY(%d,%d)\n", dY.GetRows(), dY.GetCols());
    Matrix FtdY = Ft*dY;  //printf("  FtdY(%d,%d)\n", FtdY.GetRows(), FtdY.GetCols());

    Matrix par_old(p,1);
    par_old(1,1) = par[0];
    par_old(2,1) = par[1];
    if( fitType ) par_old(3,1) = par[2];
    if( fitType ) par_old(4,1) = par[3];

    this->dpar = CoVar * FtdY;  //printf("  dpar(%d,%d)\n", dpar.GetRows(), dpar.GetCols());
    this->par = par_old + dpar;
    this->SSR = (dY.Transpose() * dY)(1,1);
    this->sigma = this->SSR / this->DF;

    this->error  = Matrix(p,1); for( int i = 1; i <= p ; i++){ (this->error)(i,1)  = sqrt(this->sigma * CoVar(i,i)); }
    this->tDis   = Matrix(p,1); for( int i = 1; i <= p ; i++){ (this->tDis)(i,1)   = (this->par)(i,1)/(this->error)(i,1); }
    this->pValue = Matrix(p,1); for( int i = 1; i <= p ; i++){ (this->pValue)(i,1) = cum_tDis30(- std::abs(tDis(i,1)));}

}

void Analysis::NonLinearFit(QVector<double> par)
{
    // regression of 4, fitType = 1;
    int count = 0 ;
    QString tmp;
    Msg = "Regression of 4-parameters: ";
    do{
        Regression(1, par);
        count ++;
        tmp.sprintf("%d ",count);
        Msg.append(tmp);
    }while( errFlag == 0 &&
            std::abs(dpar(1,1)) > 0.01 &&
            std::abs(dpar(2,1)) > 0.01 &&
            std::abs(dpar(3,1)) > 0.01 &&
            std::abs(dpar(4,1)) > 0.01 );

    if(errFlag){
        Msg.append("| Unable to calculated Covariance, terminated.");
        SendMsg(Msg);
        SendMsg(" ------ try 2-parameters fits. User may adjust the inital parameters.");
    }else{
        Msg.append("| Finsihed. Iteration converged.");
        SendMsg(Msg);
    }
    if( errFlag == 0){
        this->par.Transpose().PrintM("sol");
        this->error.Transpose().PrintM("error");
        this->pValue.Transpose().PrintM("pValue");
    }

    //Check should it go to 2-parameters fit
    bool try2parFit = 0;
    if( errFlag ||
        (this->pValue)(1,1) > 0.05 ||
        (this->pValue)(2,1) > 0.05 ||
        (this->pValue)(3,1) > 0.05 ||
        (this->pValue)(4,1) > 0.05 ){
        Msg = " +++++++++++++  Result rejected.";
        try2parFit = 1;
        count = 0;
        errFlag = 0;
    }

    if( try2parFit == 0 ) return;

    Msg = "Regression of 2-parameters: ";
    do{
        Regression(0, par);
        count ++;
        tmp.sprintf("%d ",count);
        Msg.append(tmp);
    }while( errFlag == 0 &&
            std::abs(dpar(1,1)) > 0.01 &&
            std::abs(dpar(2,1)) > 0.01 &&
            std::abs(dpar(3,1)) > 0.01 &&
            std::abs(dpar(4,1)) > 0.01 );

    if(errFlag){
        Msg.append("| Unable to calculated Covariance, terminated.");
        SendMsg(Msg);
        SendMsg(" ------ User may adjust the inital parameters.");
    }else{
        Msg.append("| Finsihed. Iteration converged.");
        SendMsg(Msg);
    }

    if( errFlag == 0){
        this->par.Transpose().PrintM("sol");
        this->error.Transpose().PrintM("error");
        this->pValue.Transpose().PrintM("pValue");
    }

}
