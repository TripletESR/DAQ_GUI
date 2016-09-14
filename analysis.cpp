#include "analysis.h"

Analysis::Analysis()
{
    Initialization();
}

Analysis::Analysis(const QVector<double> x, const QVector<double> y)
{
    Initialization();
    SetData(x,y);

}

Analysis::~Analysis()
{
}

void Analysis::Initialization()
{

    this->SSR = 999;
    this->sigma = 999;
    this->n = 0;
    this->p = 0;
    this->DF = 0;
    this->startFitIndex = 0;
    this->errFlag = 0;

    connect(&sol,    SIGNAL(SendMsg(QString)), this, SLOT(MsgConnector(QString)));
    connect(&dpar,   SIGNAL(SendMsg(QString)), this, SLOT(MsgConnector(QString)));
    connect(&error,  SIGNAL(SendMsg(QString)), this, SLOT(MsgConnector(QString)));
    connect(&tDis,   SIGNAL(SendMsg(QString)), this, SLOT(MsgConnector(QString)));
    connect(&pValue, SIGNAL(SendMsg(QString)), this, SLOT(MsgConnector(QString)));

}

void Analysis::SetData(const QVector<double> x, const QVector<double> y)
{
    // assume x.size() == y.size()
    this->xdata = x;
    this->ydata = y;

    this->n = this->xdata.size();
    Msg.sprintf("Input Data, size = %d", this->n);
    //SendMsg(Msg);

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

void Analysis::Regression(const bool fitType, QVector<double> par)
{

    //qDebug("Regression: %d ", fitType);

    errFlag = 0;
    int xStart = this->startFitIndex;
    int xEnd = this->n-1;
    int fitSize = xEnd - xStart + 1;

    this->p = 2;//default is 2 parameters fit
    if (fitType) this->p = 4;

    this->DF = fitSize - this->p;

    if( this->DF < 1){
        SendMsg("no data.");
        errFlag = 3;
        return;
    }
    //============================Start regression
    Matrix Y(fitSize,1);
    for(int i = 1; i <= fitSize ; i++) {
        Y(i,1) = this->ydata[i + xStart - 1];
    }

    Matrix f(fitSize,1);
    for(int i = 1; i <= fitSize ; i++) {
        double x = xdata[i + xStart - 1];
        f(i,1) = fitFunction(fitType, x, par);
    }

    Matrix F(fitSize,p); // F = grad(f)

    for(int i = 1; i <= fitSize ; i++) {
        double x = xdata[i - 1 + xStart];
        F(i,1) = exp(-x/par[1]);
        F(i,2) = par[0] * x * exp(-x/par[1])/par[1]/par[1];
        if( fitType ) F(i,3) = exp(-x/par[3]);
        if( fitType ) F(i,4) = par[2] * x * exp(-x/par[3])/par[3]/par[3];
    }

    Matrix Ft = F.Transpose();
    Matrix FtF = Ft * F;
    Matrix CoVar;
    try{
        CoVar = FtF.Inverse();
    }catch( Exception err){
        errFlag = 1;
        SendMsg("cannot cal inverse.");
        return;
    }

    CoVar.PrintM("CoVar");

    Matrix dY = Y - f;
    Matrix FtdY = Ft*dY;
    Matrix par_old(1,p);
    par_old(1,1) = par[0];
    par_old(1,2) = par[1];
    if( fitType ) par_old(1,3) = par[2];
    if( fitType ) par_old(1,4) = par[3];


    this->SSR = (dY.Transpose() * dY)(1,1);
    this->sigma = this->SSR / this->DF;

    qDebug() << "............................";

    qDebug("dY : %d, %d", dY.GetRows(), dY.GetCols());
    qDebug("FtdY : %d, %d", FtdY.GetRows(), FtdY.GetCols());

    this->dpar = (CoVar * FtdY).Transpose();

    this->dpar.PrintVector("dpar");

    this->sol =  par_old + dpar;
    this->error  = Matrix(1,p); for( int i = 1; i <= p ; i++){  (this->error)(1,i) = sqrt(this->sigma * CoVar(i,i)); }
    this->tDis   = Matrix(1,p); for( int i = 1; i <= p ; i++){   (this->tDis)(1,i) = (this->sol)(1,i)/(this->error)(1,i); }
    this->pValue = Matrix(1,p); for( int i = 1; i <= p ; i++){ (this->pValue)(1,i) = cum_tDis30(- std::abs(tDis(1,i)));}

    this->sol.PrintVector("sol");
    this->error.PrintVector("error");
    this->pValue.PrintVector("pValue");

}

void Analysis::NonLinearFit(QVector<double> par)
{
    // regression of 4, fitType = 1;
    int count = 0 ;
    QString tmp;
    Msg = "Regression of 4-parameters: ";
    do{
        count ++;
        if( count > 20) {
            errFlag = 2;
            break;
        }
        Regression(1, par);
        tmp.sprintf("%d ",count);
        Msg.append(tmp);
    }while( errFlag == 0 &&
            std::abs(dpar(1,1)) > 0.01 &&
            std::abs(dpar(1,2)) > 0.01 &&
            std::abs(dpar(1,3)) > 0.01 &&
            std::abs(dpar(1,4)) > 0.01 );

    if(errFlag == 1){
        Msg.append("| Unable to calculated Covariance, terminated.");
        SendMsg(Msg);

        SendMsg(" ------ try 2-parameters fits. User may adjust the inital parameters.");
    }else if(errFlag == 2){
        Msg.append("| Unable to converge after 20 trial, terminated.");
        SendMsg(Msg);
    }else if(errFlag == 0){
        Msg.append("| Finsihed. Iteration converged.");
        SendMsg(Msg);
    }else if (errFlag == 3) {
        return;
    }else{
        SendMsg("unknown error.");
        return;
    }

    if( errFlag == 0){
        this->sol.PrintVector("sol");
        this->error.PrintVector("error");
        this->pValue.PrintVector("pValue");
    }

    //Check should it go to 2-parameters fit
    bool try2parFit = 0;
    if( errFlag ||
        (this->pValue)(1,1) > 0.05 ||
        (this->pValue)(1,2) > 0.05 ||
        (this->pValue)(1,3) > 0.05 ||
        (this->pValue)(1,4) > 0.05 ){
        Msg = " +++++++++++++  Result rejected.";
        SendMsg(Msg);
        try2parFit = 1;
        count = 0;
        errFlag = 0;
    }

    if( try2parFit == 0 ) return;

    Msg = "Regression of 2-parameters: ";
    do{
        count ++;
        if( count > 20) {
            errFlag = 2;
            break;
        }
        Regression(0, par);
        tmp.sprintf("%d ",count);
        Msg.append(tmp);
        qDebug() << Msg ;
        dpar.PrintVector("dpar");

    }while( errFlag == 0 &&
            std::abs(dpar(1,1)) > 0.01 &&
            std::abs(dpar(1,2)) > 0.01 &&
            std::abs(dpar(1,3)) > 0.01 &&
            std::abs(dpar(1,4)) > 0.01 );

    if(errFlag == 1){
        Msg.append("| Unable to calculated Covariance, terminated.");
        SendMsg(Msg);
        SendMsg(" ------ User may adjust the inital parameters.");
    }else if(errFlag){
        Msg.append("| Unable to converge after 20 trials, terminated.");
        SendMsg(Msg);
    }else{
        Msg.append("| Finsihed. Iteration converged.");
        SendMsg(Msg);
    }

    if( errFlag == 0){
        this->sol.PrintVector("sol");
        this->error.PrintVector("error");
        this->pValue.PrintVector("pValue");
    }

}
