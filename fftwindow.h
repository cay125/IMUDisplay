#ifndef FFTWINDOW_H
#define FFTWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QVector>
#include <QString>
#include <QVariant>
#include "qcustomplot.h"

namespace Ui {
class fftWindow;
}

class fftWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit fftWindow(QWidget *parent = nullptr);
    ~fftWindow();
    void addPlot(QString name, QPen pen,int fftNum);
    void removePlot(QString name);
    void changePlotPen(QString name, QPen pen);

signals:
    void fftNum_signal(int);

public slots:
    void FFTfinished_slot(QVariant, QString);

private:
    Ui::fftWindow *ui;
    QPalette p;
    QVector<QCustomPlot*> customplot;
    int plotSelect=0;
    int findPlot(QString name);
    QString newName(QString name);

private slots:
    void contextMenuRequest(QPoint);
    void changeFFTNum();

protected:
    void closeEvent(QCloseEvent *event);

};

#endif // FFTWINDOW_H
