#include "alldatawindow.h"
#include "ui_alldatawindow.h"

allDataWindow::allDataWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::allDataWindow)
{
    ui->setupUi(this);
    QPalette p;
    p.setColor(QPalette::Background,Qt::white);
    setAutoFillBackground(true);
    setPalette(p);
}

void allDataWindow::addPlot(QString name, QPen pen)
{
    int index=findPlot(name);
    if(index==-1)
    {
        customplot.append(new QCustomPlot(this));
        customplot[customplot.size()-1]->addGraph();
        int index=customplot.size()-1;
        QCPGraph* graph=customplot[index]->graph();
        graph->setName(newName(name));
        graph->setPen(pen);
        customplot[index]->axisRect()->setupFullAxesBox();
        customplot[index]->legend->setSelectedFont(QFont("Microsoft YaHei", 9, QFont::Normal,false));
        customplot[index]->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items
        customplot[index]->xAxis2->setVisible(true);
        customplot[index]->xAxis2->setTickLabels(false);
        customplot[index]->yAxis2->setVisible(true);
        customplot[index]->yAxis2->setTickLabels(false);
        customplot[index]->xAxis->setRange(0,10000);
        customplot[index]->yAxis->setRange(-10,10);
        customplot[index]->xAxis->setTickLabels(false);
        customplot[index]->legend->setVisible(true);
        customplot[index]->legend->setFont(QFont("Microsoft YaHei", 9, QFont::Normal,false));
        customplot[index]->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
        customplot[index]->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(customplot[index], SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
        ui->mainLayout->addWidget(customplot[index]);
    }
}
void allDataWindow::removePlot(QString name)
{
    int index=findPlot(name);
    if(index>=0)
    {
        ui->mainLayout->removeWidget(customplot[index]);
        customplot[index]->clearGraphs();
        customplot[index]->clearItems();
        customplot[index]->clearPlottables();
        customplot[index]->deleteLater();
        customplot.removeAt(index);
        if(customplot.size()==0)
            close();
    }
}
int allDataWindow::findPlot(QString name)
{
    int index=0;bool foundGraph=false;
    QString plotName=newName(name);
    for(int i=0;i<customplot.size();i++)
    {
        if(customplot[i]->graph()->name()==plotName)
        {
            index=i;
            foundGraph=true;
            break;
        }
    }
    if(foundGraph)
        return index;
    else
        return -1;
}
void allDataWindow::changePlotPen(QString name, QPen pen)
{
    int index=findPlot(name);
    if(index>=0)
    {
        customplot[index]->graph()->setPen(pen);
        customplot[index]->replot();
    }
}

allDataWindow::~allDataWindow()
{
    delete ui;
}

QString allDataWindow::newName(QString name)
{
    return name+"(whole length)";
}
void allDataWindow::replotGraphs()
{
    for(int i=0;i<customplot.count();i++)
        customplot[i]->replot();
}
void allDataWindow::initData(QVector<double> data, QString name)
{
    int index=findPlot(name);
    qDebug()<<name<<" total size: "<<data.size();
    if(index!=-1)
    {
        QVector<double> xpos;
        for(int i=0;i<data.size();i++)
            xpos.append(i);
        customplot[index]->graph()->setData(xpos,data,true);
        customplot[index]->graph()->rescaleValueAxis(false,true);
        customplot[index]->replot();
    }
}
void allDataWindow::transferData(QVector<double> data, QString name)
{
    int index=findPlot(name);
    if(index!=-1)
    {
        QVector<double> xpos;
        int last_index=customplot[index]->graph()->dataCount()-1;
        double lastX=last_index>=0?customplot[index]->graph()->dataMainKey(last_index):0;
        for(int i=1;i<=data.size();i++)
            xpos.append(i+lastX);
        customplot[index]->graph()->addData(xpos,data);
        customplot[index]->graph()->rescaleValueAxis(false,true);
        if(customplot[index]->graph()->dataCount()>=10000)
        {
            int count=customplot[index]->graph()->dataCount();
            QCPGraph * mGraph=customplot[index]->graph();
            for(int i=1;i<=count/2;i++)
            {
                double mainKey=mGraph->dataMainKey(i);
                mGraph->data()->remove(mainKey-0.01,mainKey+0.01);
            }
            QVector<double> resample_x,resample_y;
            for(int i=0;i<mGraph->dataCount();i++)
            {
                resample_x.append(i);
                resample_y.append(mGraph->dataMainValue(i));
            }
            mGraph->setData(resample_x,resample_y,true);
        }
    }
}
