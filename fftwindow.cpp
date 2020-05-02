#include "fftwindow.h"
#include "ui_fftwindow.h"

fftWindow::fftWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::fftWindow)
{
    ui->setupUi(this);
    p.setColor(QPalette::Background,Qt::white);
    setAutoFillBackground(true);
    setPalette(p);
}
void fftWindow::addPlot(QString name, QPen pen, int fftNum)
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
        customplot[index]->xAxis->setRange(0,2*3.1416);
        customplot[index]->yAxis->setRange(-10,10);
        //customplot[index]->xAxis->setTickLabels(false);
        customplot[index]->xAxis->setTicker(QSharedPointer<QCPAxisTickerPi>(new QCPAxisTickerPi));
        customplot[index]->legend->setVisible(true);
        customplot[index]->legend->setFont(QFont("Microsoft YaHei", 9, QFont::Normal,false));
        customplot[index]->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
        customplot[index]->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(customplot[index], SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
        ui->mainLayout->addWidget(customplot[index]);
    }
}

fftWindow::~fftWindow()
{
    delete ui;
}

void fftWindow::FFTfinished_slot(QVariant datatransfer, QString name)
{
    int index=findPlot(name);
    if(index>=0)
    {
        QVector<double> fftOut=datatransfer.value<QVector<double>>();
        QVector<double> xpos;
        for(int i=0;i<fftOut.size();i++)
            xpos.append(i*2*3.1416/fftOut.size());
        // to make sure display length is [0,2pi]
        xpos.append(2*3.1416);
        fftOut.append(fftOut[0]);
        customplot[index]->graph()->setData(xpos,fftOut,true);
        customplot[index]->graph()->rescaleValueAxis(false,true);
        customplot[index]->xAxis->rescale();
        customplot[index]->replot();
    }
}
void fftWindow::removePlot(QString name)
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
int fftWindow::findPlot(QString name)
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
void fftWindow::closeEvent(QCloseEvent *event)
{
    qDebug()<<"fftwindows closed";
}
void fftWindow::changePlotPen(QString name, QPen pen)
{
    int index=findPlot(name);
    if(index>=0)
    {
        customplot[index]->graph()->setPen(pen);
        customplot[index]->replot();
    }
}
void fftWindow::changeFFTNum()
{
    bool ok=false;
    int input = QInputDialog::getInt(this,"fft num","input",256,4,8192,1,&ok);
    if( ok && input>=4 && input<8192)
        emit fftNum_signal(input);
}
void fftWindow::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    QCustomPlot* custom_chart = qobject_cast<QCustomPlot*> (sender());
    for(int i=0;i<customplot.size();i++)
    {
        if(custom_chart==customplot[i])
        {
            plotSelect = i;
            break;
        }
    }
    if (custom_chart->legend->selectTest(pos, false) >= 0) // context menu on legend requested
    {
        menu->addAction("modify fft points num",this,SLOT(changeFFTNum()));
    }
    else  // general context menu on graphs requested
    {
        if (custom_chart->graphCount() > 0)
          menu->addAction("modify fft points num", this, SLOT(changeFFTNum()));
    }
    menu->popup(custom_chart->mapToGlobal(pos));
}
QString fftWindow::newName(QString name)
{
    return name+"(FFT)";
}
