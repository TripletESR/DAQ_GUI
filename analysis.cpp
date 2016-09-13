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

void Analysis::Initialization()
{
    this->SSR = 999;
    this->sigma = 999;
    this->n = 0;
    this->p = 0;
    this->DF = 0;
    this->startFitIndex = 0;
    this->errFlag = 0;

    connect(&par,    SIGNAL(SendMsg(QString)), this, SLOT(MsgConnector(QString)));
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
    errFlag = 0;
    int xStart = this->startFitIndex;
    int xEnd = this->n-1;
    int fitSize = xEnd - xStart + 1;

    this->p = 2;//default is 2 parameters fit
    if (fitType) this->p = 4;

    this->DF = fitSize - this->p;
    //this->par = QVector2ColVector(par).Transpose();

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

    qDebug("f : %d, %d" , f.GetRows(), f.GetCols());

    Matrix F(fitSize,p); // F = grad(f)
    qDebug("F : %d, %d" , F.GetRows(), F.GetCols());

    for(int i = 1; i <= fitSize ; i++) {
        double x = xdata[i - 1 + xStart];
        F(i,1) = exp(-x/par[1]);
        F(i,2) = par[0] * x * exp(-x/par[1])/par[1]/par[1];
        if( fitType ) F(i,3) = exp(-x/par[3]);
        if( fitType ) F(i,4) = par[2] * x * exp(-x/par[3])/par[3]/par[3];
        //qDebug("F : %d %f (%f, %f)", i, x, F(i,1), F(i,2));
    }

    Matrix Ft = F.Transpose();
    qDebug("Ft : %d, %d" , Ft.GetRows(), Ft.GetCols());

    Matrix FtF = Ft * F;
    FtF.PrintM("FtF");

    qDebug() << FtF.Det();


    Matrix CoVar;
    try{
        CoVar = FtF.Inverse();
    }catch( Exception err){
        errFlag = 1;
        return;
    }

    CoVar.PrintM("CoVar");

    Matrix dY = Y - f;    //printf("    dY(%d,%d)\n", dY.GetRows(), dY.GetCols());
    Matrix FtdY = Ft*dY;  //printf("  FtdY(%d,%d)\n", FtdY.GetRows(), FtdY.GetCols());

    Matrix par_old(1,p);
    par_old(1,1) = par[0];
    par_old(1,2) = par[1];
    if( fitType ) par_old(1,3) = par[2];
    if( fitType ) par_old(1,4) = par[3];

    this->dpar = (CoVar * FtdY).Transpose();  //printf("  dpar(%d,%d)\n", dpar.GetRows(), dpar.GetCols());
    this->par = par_old + dpar;
    this->SSR = (dY.Transpose() * dY)(1,1);
    this->sigma = this->SSR / this->DF;

    this->error  = Matrix(1,p); for( int i = 1; i <= p ; i++){  (this->error)(1,i) = sqrt(this->sigma * CoVar(i,i)); }
    this->tDis   = Matrix(1,p); for( int i = 1; i <= p ; i++){   (this->tDis)(1,i) = (this->par)(1,i)/(this->error)(1,i); }
    this->pValue = Matrix(1,p); for( int i = 1; i <= p ; i++){ (this->pValue)(1,i) = cum_tDis30(- std::abs(tDis(1,i)));}

    this->par.PrintM("sol");
    this->error.PrintM("error");
    this->pValue.PrintM("pValue");

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
            std::abs(dpar(1,2)) > 0.01 &&
            std::abs(dpar(1,3)) > 0.01 &&
            std::abs(dpar(1,4)) > 0.01 );

    if(errFlag){
        Msg.append("| Unable to calculated Covariance, terminated.");
        SendMsg(Msg);
        SendMsg(" ------ try 2-parameters fits. User may adjust the inital parameters.");
    }else{
        Msg.append("| Finsihed. Iteration converged.");
        SendMsg(Msg);
    }
    if( errFlag == 0){
        this->par.PrintM("sol");
        this->error.PrintM("error");
        this->pValue.PrintM("pValue");
    }

    //Check should it go to 2-parameters fit
    bool try2parFit = 0;
    if( errFlag ||
        (this->pValue)(1,1) > 0.05 ||
        (this->pValue)(1,2) > 0.05 ||
        (this->pValue)(1,3) > 0.05 ||
        (this->pValue)(1,4) > 0.05 ){
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
            std::abs(dpar(1,2)) > 0.01 &&
            std::abs(dpar(1,3)) > 0.01 &&
            std::abs(dpar(1,4)) > 0.01 );

    if(errFlag){
        Msg.append("| Unable to calculated Covariance, terminated.");
        SendMsg(Msg);
        SendMsg(" ------ User may adjust the inital parameters.");
    }else{
        Msg.append("| Finsihed. Iteration converged.");
        SendMsg(Msg);
    }

    if( errFlag == 0){
        this->par.PrintM("sol");
        this->error.PrintM("error");
        this->pValue.PrintM("pValue");
    }

}
