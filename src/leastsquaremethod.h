#ifndef LEASTSQUAREMETHOD_H
#define LEASTSQUAREMETHOD_H

#include <QVector>

class LeastSquareMethod
{
public:
    void clear();
    void calc();
    void add(double x, double y);
    double getA();
    double getB();

private:
    double mA;
    double mB;
    QVector<double> mX;
    QVector<double> mY;
};

#endif // LEASTSQUAREMETHOD_H

