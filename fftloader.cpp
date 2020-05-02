#include "fftloader.h"
#include "fftw3.h"
#include <QThread>
#include <QDebug>
fftLoader::fftLoader(QObject *parent,int _N) : QObject(parent)
{
    N=_N;
    in1_c = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* N);
    out1_c = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* N);
    p = fftw_plan_dft_1d(N, in1_c, out1_c, FFTW_FORWARD, FFTW_ESTIMATE);
    qDebug()<<"fftloader work on thread id =  "<<QThread::currentThreadId();
}


void fftLoader::FFTstart_slot(QVariant dataTransfer, QString name)
{
    QVector<double> input=dataTransfer.value<QVector<double>>();
    //qDebug()<<"fftloader work on thread id =  "<<QThread::currentThreadId()<<" "<<input.size();
//    if(input.size()<N)
//    {
//        fftw_free(in1_c);
//        fftw_free(out1_c);//释放内存
//        fftw_destroy_plan(p);
//        N=input.size();
//        in1_c = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* N);
//        out1_c = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* N);
//        p = fftw_plan_dft_1d(N, in1_c, out1_c, FFTW_FORWARD, FFTW_ESTIMATE);
//    }
    int minLen=input.size()<N?input.size():N;
    for(int i=0;i<minLen;i++)
    {
        in1_c[i][0]=input[i];
        in1_c[i][1]=0;
    }
    for(int i=minLen;i<N;i++)
    {
        in1_c[i][0]=0;
        in1_c[i][1]=0;
    }
    fftw_execute(p);
    QVector<double> output;
    for(int i=0;i<N;i++)
        output.append(sqrt(out1_c[i][0]*out1_c[i][0]+out1_c[i][1]*out1_c[i][1])/N);
    QVariant fftout_transfer;
    fftout_transfer.setValue(output);
    emit FFTfinished_signal(fftout_transfer, name);
}
void fftLoader::fftNum_slot(int num)
{
    qDebug()<<"modify fft num as: "<<num;
    N=num;
    fftw_free(in1_c);
    fftw_free(out1_c);//释放内存
    fftw_destroy_plan(p);
    in1_c = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* N);
    out1_c = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* N);
    p = fftw_plan_dft_1d(N, in1_c, out1_c, FFTW_FORWARD, FFTW_ESTIMATE);
}
