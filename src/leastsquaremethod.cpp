#include "leastsquaremethod.h"

void LeastSquareMethod::clear()
{
    mA = 0;
    mB = 0;
    mX.clear();
    mY.clear();
}

void LeastSquareMethod::calc()
{
    int n = mX.size();
    if (n == 0) return;

    double sumX = 0;
    double sumY = 0;
    double sumXY = 0;
    double sumXX = 0;
    for (int i = 0; i < n; i++) {
        sumX += mX[i];
        sumY += mY[i];
        sumXX += mX[i]*mX[i];
        sumXY += mX[i]*mY[i];
    }
    mA = (double(n) * sumXY - sumX * sumY) / (double(n) * sumXX - sumX * sumX);
    mB = (sumY - mA * sumX) / double(n);
}

void LeastSquareMethod::add(double x, double y)
{
    mX.append(x);
    mY.append(y);
}

double LeastSquareMethod::getA()
{
    return mA;
}

double LeastSquareMethod::getB()
{
    return mB;
}

int LeastSquareMethod::getN()
{
    return mX.size();
}
