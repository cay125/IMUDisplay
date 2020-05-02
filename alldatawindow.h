#ifndef ALLDATAWINDOW_H
#define ALLDATAWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"

namespace Ui {
class allDataWindow;
}

class allDataWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit allDataWindow(QWidget *parent = nullptr);
    ~allDataWindow();
    void addPlot(QString name, QPen pen);
    void removePlot(QString name);
    void changePlotPen(QString name, QPen pen);
    void replotGraphs();
    void initData(QVector<double>, QString);
    void transferData(QVector<double>,QString);
private:
    Ui::allDataWindow *ui;
    QVector<QCustomPlot*> customplot;
    int findPlot(QString name);
    QString newName(QString name);
};

#endif // ALLDATAWINDOW_H
