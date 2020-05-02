#include "onlinevarian.h"
#include <QDebug>

onlineVarian::onlineVarian()
{
}
void onlineVarian::clearData()
{
    dataTotalSum=0;
    dataCnt=0;
    currentVar=0;
    currentMean=0;
}
double onlineVarian::addData(QVector<double> input)
{
    if(input.size()==0)
        return currentVar;
    double sumInput=0;
    for(int i=0;i<input.size();i++)
        sumInput+=input[i];
    double meanInput=sumInput/input.size();
    double varianceInput=0;
    for(int i=0;i<input.size();i++)
        varianceInput+=(meanInput-input[i])*(meanInput-input[i]);
    varianceInput/=input.size();
    if(dataCnt==0)
    {
        currentVar=varianceInput;
    }
    else
    {
        double meanNew=(dataTotalSum+sumInput)/(dataCnt+input.size());
        double t1=dataCnt*(currentVar+(meanNew-currentMean)*(meanNew-currentMean));
        double t2=input.size()*(varianceInput+(meanNew-meanInput)*(meanNew-meanInput));
        currentVar=(t1+t2)/(dataCnt+input.size());
    }
    dataCnt+=input.size();
    dataTotalSum+=sumInput;
    currentMean=dataTotalSum/dataCnt;
    return currentVar;
}
double onlineVarian::removeData(QVector<double> input)
{
    if(input.size()==0)
        return currentVar;
    double sumInput=0;
    for(int i=0;i<input.size();i++)
        sumInput+=input[i];
    double meanInput=sumInput/input.size();
    double varianceInput=0;
    for(int i=0;i<input.size();i++)
        varianceInput+=(meanInput-input[i])*(meanInput-input[i]);
    varianceInput/=input.size();
    double meanNew=(dataTotalSum-sumInput)/(dataCnt-input.size());
    double t1=dataCnt*(currentVar+(meanNew-currentMean)*(meanNew-currentMean));
    double t2=input.size()*(varianceInput+(meanNew-meanInput)*(meanNew-meanInput));
    currentVar=(t1-t2)/(dataCnt-input.size());
    dataCnt-=input.size();
    dataTotalSum-=sumInput;
    currentMean=dataTotalSum/dataCnt;
    return currentVar;
}
