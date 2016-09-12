#include "analysis.h"

Analysis::Analysis(QObject *parent) : QObject(parent)
{

}

Analysis::Analysis(const QVector<double> x, const QVector<double> y)
{
    // assume x.size() == y.size()
    //this->xdata = x;
    //this->ydata = y;

    //this->n = this->xdata.size();

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

Matrix *Analysis::Regression(bool fitType, int startFitIndex, QVector<double> par)
{
    int xStart = startFitIndex;
    int xEnd = this->n;
    int fitSize = xStart - xEnd + 1;

    this->p = 2;//default is 2 parameters fit
    if (fitType) this->p = 4;

    this->DF = fitSize - this->p;

    this->par = par;

    //All Matrix started from below
    Matrix *output = new Matrix[6]; // 0 = par; 1 = dpar; 2 = sigma ; 3 = t-dis, 4 = p-value, 5 = SSR
    output[0] = Matrix(p,1);
    output[1] = Matrix(p,1);
    output[2] = Matrix(p,1);
    output[3] = Matrix(p,1);
    output[4] = Matrix(p,1);
    output[5] = Matrix(1,1);

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
        (output[4])(1,1) = 9999; // set the p-Value to be 9999;
        return output;
    }

    //CoVar.Print();

    Matrix dY = Y - f;    //printf("    dY(%d,%d)\n", dY.GetRows(), dY.GetCols());
    Matrix FtdY = Ft*dY;  //printf("  FtdY(%d,%d)\n", FtdY.GetRows(), FtdY.GetCols());

    Matrix par_old(p,1);
    par_old(1,1) = par[0];
    par_old(2,1) = par[1];
    if( fitType ) par_old(3,1) = par[2];
    if( fitType ) par_old(4,1) = par[3];

    Matrix dpar = CoVar * FtdY;  //printf("  dpar(%d,%d)\n", dpar.GetRows(), dpar.GetCols());
    Matrix par_new = par_old + dpar;
    Matrix SSR = dY.Transpose() * dY;

    this->SSR = SSR(1,1);
    this->sigma = this->SSR / this->DF;

    Matrix sigma(p,1); for( int i = 1; i <= p ; i++){ sigma(i,1) = sqrt(this->sigma * CoVar(i,i)); }
    Matrix tDis(p,1); for( int i = 1; i <= p ; i++){ tDis(i,1) = par_new(i,1)/sigma(i,1); }
    Matrix ApValue(p,1); for( int i = 1; i <= p ; i++){ ApValue(i,1) = cum_tDis30(- std::abs(tDis(i,1)));}

    output[0] = par_new;
    output[1] = dpar;
    output[2] = sigma;
    output[3] = tDis;
    output[4] = ApValue;
    output[5] = SSR/DF;

    this->par = RowVector2QVector(par_new);
    this->error = RowVector2QVector(sigma);
    this->pValue = RowVector2QVector(ApValue);

    return output;

}

Matrix *Analysis::NonLinearFit(int startFitIndex, QVector<double> iniPar)
{

}
