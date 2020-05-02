#ifndef ONLINEVARIAN_H
#define ONLINEVARIAN_H

#include <QVector>
class onlineVarian
{
public:
    onlineVarian();
    double currentVar=0;
    double currentMean=0;
    double addData(QVector<double> input);
    double removeData(QVector<double> input);
    void clearData();
private:
    double dataTotalSum=0;
    long long dataCnt=0;
};

#endif // ONLINEVARIAN_H
