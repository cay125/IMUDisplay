#ifndef FFTLOADER_H
#define FFTLOADER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>
#include "fftw3.h"

class fftLoader : public QObject
{
    Q_OBJECT
public:
    int N=0;
    fftLoader(QObject *parent = NULL, int _N=256);
public slots:
    void FFTstart_slot(QVariant, QString);
    void fftNum_slot(int);
signals:
    void FFTfinished_signal(QVariant, QString);
private:
    fftw_plan p;
    fftw_complex *in1_c, *out1_c;
};

#endif // FFTLOADER_H
