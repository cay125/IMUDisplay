#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serialport.h"
#include <QSerialPortInfo>
#include "switchcontrol.h"
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),status(new Status),ui(new Ui::MainWindow),timer_plot(new QTimer),timer_data(new QTimer),linePalette(new stylePalette),fftwin(new fftWindow(parent)),fftloader(new fftLoader()),allwindow(new allDataWindow(parent))
{
    //showMaximized();
    //set white background color
    ui->setupUi(this);
    p.setColor(QPalette::Background,Qt::white);
    setAutoFillBackground(true);
    setPalette(p);
    qDebug()<< "mainwindow work on thread id = " << QThread::currentThreadId();
    fft_thread=new QThread();
    fftloader->moveToThread(fft_thread);
    fft_thread->start();
    for(int i=0;i<6;i++)
    {
        onlineVar[i] = new onlineVarian();
        onlineVarToTxt[i] = new onlineVarian();
    }
    SwitchControl *fileSwitchControl = new SwitchControl(this);
    // set switchcontrol style
    fileSwitchControl->setToggle(true);
    fileSwitchControl->setCheckedColor(QColor(0, 160, 230));
    ui->speedSlider->setMinimumWidth(160);
    ui->hLayout->addWidget(fileSwitchControl);
    saver=new fileSaver("record.txt", fileSwitchControl->isToggled());
    connect(fileSwitchControl, SIGNAL(toggled(bool)), saver, SLOT(isSave_slot(bool)));
    SeriesName.push_back("top pitch");
    SeriesName.push_back("top roll");
    SeriesName.push_back("bottom pitch");
    SeriesName.push_back("bottom roll");
    for(int i=0;i<totallines;i++)
    {
        dataEdit.push_back(new QLineEdit());
        dataEdit[i]->setMaximumWidth(70);
        dataEdit[i]->setReadOnly(true);
        dataEdit[i]->setText("NULL");

        dataLabel.push_back(new QLabel());
        dataLabel[i]->setMinimumHeight(20);
        dataLabel[i]->setText(SeriesName[i]);
        dataLabel[i]->setFont(QFont("Microsoft YaHei", 9, QFont::Normal,false));

        ui->lineLayout->addWidget(dataEdit[i]);
        ui->labelLayout->addWidget(dataLabel[i]);
    }
    ui->lineLayout->addStretch();
    ui->labelLayout->addStretch();

    ui->btnStart->setEnabled(false);

    // add portname to combobox
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        qDebug()<<"serialPortName:"<<info.portName();
        ui->ComBox->addItem(info.portName());
        ui->ComBox->setCurrentIndex(0);
    }
    ui->ComBox->setFont(QFont("Microsoft YaHei", 9, QFont::Normal,false));

    //add Baudrate to combobox
    ui->BaudBox->addItem("9600");
    ui->BaudBox->addItem("19200");
    ui->BaudBox->addItem("38400");
    ui->BaudBox->addItem("115200");
    ui->BaudBox->addItem("230400");
    ui->BaudBox->setCurrentIndex(4);
    ui->BaudBox->setFont(QFont("Microsoft YaHei", 9, QFont::Normal,false));
    ui->ParityBox->addItem("Odd");
    ui->ParityBox->addItem("Even");
    ui->ParityBox->addItem("None");
    ui->ParityBox->setCurrentIndex(2);
    ui->FlashEdit->setText("1");
    ui->FlashEdit->setValidator(new QIntValidator(0,1000));
    ui->FlashEdit->setFont(QFont("Microsoft YaHei", 9, QFont::Normal,false));
    ui->GraEdit->setText("1");
    ui->GraEdit->setValidator(new QDoubleValidator());
    ui->GraEdit->setFont(QFont("Microsoft YaHei", 9, QFont::Normal,false));
    ui->speedSlider->setRange(2,50);
    ui->speedSlider->setValue(2);
    dx_len=ui->speedSlider->value();

    uart=new SerialPort();
    connect(this,&MainWindow::startToConnectSignal,uart,&SerialPort::startToConnectSlot);
    connect(uart,&SerialPort::connectedSignal,this,&MainWindow::connectedSlot,Qt::QueuedConnection);
    connect(this,&MainWindow::uartCloseSignal,uart,&SerialPort::uartCloseSlot);

    QPen pen[9];
    pen[0].setColor(QColor(0xfa, 0x78, 0x00));
    pen[1].setColor(QColor(0x00, 0x84, 0x3c));
    pen[2].setColor(QColor(0xff, 0x00, 0x00));
    pen[8].setColor(QColor(0x00, 0x66, 0xff));
    pen[7].setColor(QColor(0xff, 0x77, 0xff));
    pen[5].setColor(QColor(0xda, 0xa5, 0x20));
    pen[6].setColor(QColor(0xff, 0x14, 0x93));
    pen[4].setColor(QColor(0x80, 0x80, 0x00));
    pen[3].setColor(QColor(0x22, 0x8b, 0x22));
    for(int i=0;i<totalCharts;i++)
        customplot.push_back(new QCustomPlot(this));
    for(int i=0;i<totallines;i++)
        mGraphs.push_back(nullptr);
    findLineByName["top_pitch"]=0;
    findLineByName["top_roll"]=1;
    findLineByName["bottom_pitch"]=2;
    findLineByName["bottom_roll"]=3;

    for(int i=0;i<totallines;i++)
        SeriesUnit.push_back("deg");
    line2Chart[0]=0;
    line2Chart[1]=0;
    line2Chart[2]=1;
    line2Chart[3]=1;
    QVector<int> cnt(totalCharts,0);
    for(int i=0;i<totallines;i++)
    {
        customplot[line2Chart[i]]->addGraph();
        mGraphs[i]=customplot[line2Chart[i]]->graph();
        mGraphs[i]->setName(SeriesName[i]);
        mGraphs[i]->setPen(pen[cnt[line2Chart[i]]++]);
        QCPSelectionDecorator *decorator=new QCPSelectionDecorator();
        QPen tpen;
        tpen.setColor(mGraphs[i]->pen().color());
        tpen.setWidth(2);
        decorator->setPen(tpen);
        mGraphs[i]->setSelectionDecorator(decorator);
        mTags.push_back(new AxisTag(mGraphs[i]->valueAxis()));
        mTags[i]->setPen(mGraphs[i]->pen());
        mTags[i]->setText("0");
        mAveTags.push_back(new AxisTag(mGraphs[i]->valueAxis()));
        mAveTags[i]->setPen(QPen(QColor(0,0,255)));
        mAveTags[i]->setText("0");
    }
    for(int i=0;i<totalCharts;i++)
    {
        customplot[i]->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(80); // add some padding to have space for tags
        customplot[i]->setInteractions(QCP::iSelectLegend | QCP::iSelectPlottables);
        customplot[i]->axisRect()->setupFullAxesBox();
        customplot[i]->legend->setSelectedFont(QFont("Microsoft YaHei", 9, QFont::Normal,false));
        customplot[i]->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items
        // connect some interaction slots:
        connect(customplot[i], SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
        customplot[i]->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(customplot[i], SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

        customplot[i]->legend->setVisible(true);
        customplot[i]->xAxis2->setVisible(true);
        customplot[i]->xAxis2->setTickLabels(false);
        customplot[i]->yAxis2->setVisible(true);
        customplot[i]->yAxis2->setTickLabels(false);
        customplot[i]->xAxis->setRange(0,XRANGE);
        customplot[i]->yAxis->setRange(-10,10);
        customplot[i]->xAxis->setTickLabels(false);
        customplot[i]->legend->setVisible(true);
        customplot[i]->legend->setFont(QFont("Microsoft YaHei", 9, QFont::Normal,false));
        customplot[i]->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
        ui->mainLayout->addWidget(customplot[i],i,0);
    }
    PData.resize(totallines);
    PDataVec.resize(totallines);
    PDataBuffer.resize(totallines);
    OriginalDataVec.resize(totallines);

    connect(timer_plot, SIGNAL(timeout()), this, SLOT(timerSlot_customplot()));
    connect(timer_data, SIGNAL(timeout()), this, SLOT(timerSlot_data()));
    connect(linePalette,SIGNAL(signal_changeBackColor(QColor)),this,SLOT(paletteColorSlot(QColor)));
    connect(this, SIGNAL(FFTstart_signal(QVariant,QString)), fftloader,SLOT(FFTstart_slot(QVariant,QString)));
    connect(fftloader,SIGNAL(FFTfinished_signal(QVariant,QString)),fftwin,SLOT(FFTfinished_slot(QVariant,QString)));
    connect(fftwin,SIGNAL(fftNum_signal(int)),fftloader,SLOT(fftNum_slot(int)));

    timer_data->setInterval(static_cast<int>((1000.0/flashRate)));
    timer_data->start();
    timer_plot->setInterval(30);

    //initStates();
}
void MainWindow::selectionChanged()
{
    QCustomPlot* custom_chart = qobject_cast<QCustomPlot*> (sender());
    // synchronize selection of graphs with selection of corresponding legend items:
    for (int i=0; i<custom_chart->graphCount(); i++)
    {
        QCPGraph *graph = custom_chart->graph(i);
        QCPPlottableLegendItem *item = custom_chart->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected())
        {
            item->setSelected(true);
            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        }
    }
}
void MainWindow::legendDoubleClick(QCPLegend* legend,QCPAbstractLegendItem* item)
{
    // Rename a graph by double clicking on its legend item
    Q_UNUSED(legend)
    if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
    {
//        QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
//        plItem->plottable()->setVisible(!plItem->plottable()->visible());
//        plItem->setTextColor(QColor(0,0,0,plItem->plottable()->visible()?255:100));
//        plItem->setSelected(false);
        item->setSelected(false);
    }
}
void MainWindow::removeSelectedGraph()
{
    if(customplot[plotSelect]->selectedGraphs().size()>0)
    {
        customplot[plotSelect]->selectedGraphs().first()->setVisible(false);
    }
}
void MainWindow::removeAllGraphs()
{
    for(int i=0;i<chartLine[plotSelect].size();i++)
    {
        if(mainGraph[plotSelect]!=i)
        {
            mGraphs[chartLine[plotSelect][i]]->setVisible(false);
            QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(customplot[plotSelect]->legend->item(i));
            plItem->setTextColor(QColor(180,180,180));
        }
    }
}
void MainWindow::applyMainGraph()
{
    if (QAction* contextAction = qobject_cast<QAction*>(sender()))
    {
        mainGraph[plotSelect] = contextAction->data().toInt();
        mGraphs[chartLine[plotSelect][mainGraph[plotSelect]]]->setVisible(true);
        QCPPlottableLegendItem *item = customplot[plotSelect]->legend->itemWithPlottable(mGraphs[chartLine[plotSelect][mainGraph[plotSelect]]]);
        item->setTextColor(QColor(0,0,0));
        mTags[plotSelect]->setPen(mGraphs[chartLine[plotSelect][mainGraph[plotSelect]]]->pen());
    }
}
void MainWindow::setLineVisible()
{
    if (QAction* contextAction = qobject_cast<QAction*>(sender()))
    {
        int index = contextAction->data().toInt();
        mGraphs[chartLine[plotSelect][index]]->setVisible(!mGraphs[chartLine[plotSelect][index]]->visible());
        int colorRGB=mGraphs[chartLine[plotSelect][index]]->visible()?0:180;
        customplot[plotSelect]->legend->item(index)->setTextColor(QColor(colorRGB,colorRGB,colorRGB));
//        if(!mGraphs[chartLine[plotSelect][index]]->visible())
//        {
//            customplot[plotSelect]->legend->item(index)->setSelected(false);
//            customplot[plotSelect]->legend->item(index)->setSelectable(false);
//        }
    }
}
void MainWindow::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    QCustomPlot* custom_chart = qobject_cast<QCustomPlot*> (sender());
    for(int i=0;i<totalCharts;i++)
    {
        if(custom_chart==customplot[i])
        {
            plotSelect = i;
            break;
        }
    }
    if (custom_chart->legend->selectTest(pos, false) >= 0) // context menu on legend requested
    {
        for(int i=0;i<chartLine[plotSelect].size();i++)
        {
            if(custom_chart->legend->item(i)->selectTest(pos,false)>=0)
            {
                if(mainGraph[plotSelect]!=i)
                {
                    menu->addAction("Apply item as main chart", this, SLOT(applyMainGraph()))->setData(i);
                    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(custom_chart->legend->item(i));
                    QString setVisibleItem=plItem->plottable()->visible()?"Remove line from chart":"Add line to chart";
                    menu->addAction(setVisibleItem,this,SLOT(setLineVisible()))->setData(i);
                }
                break;
            }
        }
        menu->addAction("Change chart color",linePalette,SLOT(slot_OpenColorPad()));
        menu->addAction((isShowALLData[plotSelect] && allwindow->isVisible())? "Hide whole length signal" : "Show whole length signal", this, SLOT(addAllDataSlot()));
        menu->addAction((isfftTransfer[plotSelect] && fftwin->isVisible())? "Hide signal FFT" : "Show signal FFT",this,SLOT(addFFTplotSlot()));
        menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
        menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
        menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
        menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
        menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
    }
    else  // general context menu on graphs requested
    {
        if (custom_chart->selectedGraphs().size() > 0)
          menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
        if (custom_chart->graphCount() > 0)
          menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));
    }
    menu->popup(custom_chart->mapToGlobal(pos));
}
void MainWindow::moveLegend()
{
    if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
    {
        bool ok;
        int dataInt = contextAction->data().toInt(&ok);
        if (ok)
        {
            customplot[plotSelect]->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
            if(!status->isrunning)
                customplot[plotSelect]->replot();
        }
    }
}
void MainWindow::receiveDataSlot(QByteArray data)
{
    auto type=static_cast<recieveType>(data.at(0));
    int16_t top_angleX=0,top_angleY=0,top_angleZ=0;
    if(type==recieveType::angle)
    {
        top_angleX=static_cast<int16_t>(((static_cast<uint8_t>(data.at(2))<<8)|static_cast<uint8_t>(data.at(1))));
        top_angleY=static_cast<int16_t>(((static_cast<uint8_t>(data.at(4))<<8)|static_cast<uint8_t>(data.at(3))));
        top_angleZ=static_cast<int16_t>(((static_cast<uint8_t>(data.at(6))<<8)|static_cast<uint8_t>(data.at(5))));
    }
    OriginalDataVec[0].push_back(top_angleX/32768.0*180.0);
    OriginalDataVec[1].push_back(top_angleY/32768.0*180.0);
    PData[0]=top_angleX;
    PData[1]=top_angleY;
    for(int i=0;i<2;i++)
        PDataBuffer[i]+=PData[i];
    receive_data_cnt++;
}
void MainWindow::ReadError(QAbstractSocket::SocketError)
{
    tcpClient->disconnectFromHost();
    ui->btnConnect->setText(tr("Connect"));
    QMessageBox msgBox;
    msgBox.setText(tr("failed to connect server because %1").arg(tcpClient->errorString()));
    msgBox.exec();
}
void MainWindow::timerSlot_data()
{
    if(receive_data_cnt>0)
    {
        for(int i=0;i<2;i++)
        {
           PDataVec[i].append(1.0*PDataBuffer[i]/receive_data_cnt/32768.0*180.0);
           PDataBuffer[i]=0;
        }
        QVector<QString> dataToTxt;
        for(int i=0;i<2;i++)
        {
            dataToTxt.append(QString::number(PDataVec[i].back(),'f',4));
        }
        saver->writeText(dataToTxt);
        receive_data_cnt=0;
    }
}
void MainWindow::timerSlot_customplot()
{
    static int dataTextUpdateCnt=0;
    dataTextUpdateCnt++;
    int index=mGraphs[0]->dataCount()-1;
    double lastX=index>=0?mGraphs[0]->dataMainKey(index):0;
#if USE_ORIGINAL_DATA
    auto &dataProcess=OriginalDataVec;
#else
    auto &dataProcess=PDataVec;
#endif
    for(int cnt=0;cnt<totalCharts;cnt++)
    {
        // calculate and add a new data point to each graph:
        QVector<double> xPos;
        int len=dataProcess[cnt].size();
        if(len!=0)
        {
            for(int j=1;j<=len;j++)
            {
                xPos.append(j*dx_len/len+lastX);
                fftData[cnt].append(dataProcess[cnt][j-1]);
                if(fftData[cnt].size()>=fftloader->N)
                {
                    if(isfftTransfer[cnt])
                    {
                        QVariant dataTransfer;
                        dataTransfer.setValue(fftData[cnt]);
                        emit FFTstart_signal(dataTransfer,mGraphs[cnt]->name());
                    }
                    fftData[cnt].clear();
                }
            }
            mGraphs[cnt]->addData(xPos,dataProcess[cnt]);
            onlineVar[cnt]->addData(dataProcess[cnt]);
            //onlineVarToTxt[cnt]->addData(PDataVec[cnt]);
            if(isShowALLData[cnt])
                allwindow->transferData(dataProcess[cnt],mGraphs[cnt]->name());
        }
        else
        {
            //mGraphs[cnt]->addData(lastX+dx_len,index>=0?mGraphs[cnt]->dataMainValue(index):0);
        }

        if((lastX+dx_len) > XRANGE && len!=0)
        {
            QVector<double> removeX;
            int j=0;
            while(mGraphs[cnt]->dataMainKey(j)<(lastX+dx_len-XRANGE))
            {
                //mGraphs[cnt]->data()->remove(mGraphs[cnt]->dataMainKey(0));
                removeX.append(mGraphs[cnt]->dataMainValue(j));
                j++;
            }
            onlineVar[cnt]->removeData(removeX);
            mGraphs[cnt]->data()->removeBefore(lastX+dx_len-XRANGE);
        }

    }

    if(dataTextUpdateCnt>=3)
    {
        for(int cnt=0;cnt<totallines;cnt++)
        {
            if(dataProcess[cnt].size()!=0)
                dataEdit[cnt]->setText(QString::number(dataProcess[cnt].back(),'f',4));
        }
        dataTextUpdateCnt=0;
    }
    for(int i=0;i<totallines;i++)
    {
        if(dataProcess[i].size()!=0)
        {
            mGraphs[i]->rescaleValueAxis(false,true);
            double graphValue;
            if((lastX+dx_len)>XRANGE)
            {
                customplot[line2Chart[i]]->xAxis->rescale();
                customplot[line2Chart[i]]->xAxis->setRange(customplot[line2Chart[i]]->xAxis->range().upper, XRANGE, Qt::AlignRight);
            }
            graphValue=mGraphs[i]->dataMainValue(mGraphs[i]->dataCount()-1);
            graphValue=mGraphs[i]->visible()?graphValue:0;
            mTags[i]->updatePosition(graphValue);
            mTags[i]->setText(QString::number(graphValue,'f'));

            mAveTags[i]->updatePosition(onlineVar[i]->currentMean);
            mAveTags[i]->setText(QString::number(onlineVar[i]->currentMean,'f'));
            //customplot[i]->replot();
        }
    }
    for(int i=0;i<totalCharts;i++)
        customplot[i]->replot();
    for(int cnt=0;cnt<dataProcess.size();cnt++)
        dataProcess[cnt].clear();

    allwindow->replotGraphs();
}
void MainWindow::on_btnOpenGL_clicked()
{
//    static bool flag=true;
//    for(int i=0;i<21;i++)
//        mSeries[i]->setUseOpenGL(flag);
//    flag=1-flag;
//    for(int i=0;i<6;i++)
//        customplot[i]->setOpenGl(!customplot[i]->openGl());
    gra_accel=ui->GraEdit->text().toDouble();
}
void MainWindow::on_btnConnect_clicked()
{
    if(status->isconnected == false)
    {
        emit startToConnectSignal(ui->ComBox->currentText(),ui->BaudBox->currentText().toInt(),ui->ParityBox->currentIndex());
    }
    else
    {
        ui->btnConnect->setText("Connect");
        ui->btnStart->setEnabled(false);
        ui->ComBox->setEnabled(true);
        ui->BaudBox->setEnabled(true);
        ui->ParityBox->setEnabled(true);
        timer_plot->stop();
        ui->btnStart->setText("Start");
        status->isconnected=false;
        status->isrunning=false;
        emit uartCloseSignal();
    }
}
void MainWindow::connectedSlot()
{
    status->isconnected=true;
    ui->btnConnect->setText("Disconnect");
    ui->btnStart->setEnabled(true);
    ui->ComBox->setEnabled(false);
    ui->BaudBox->setEnabled(false);
    ui->ParityBox->setEnabled(false);
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnStart_clicked()
{
    if(status->isrunning==false)
    {
        for(int i=0;i<21;i++)
        {
            if(i<3)
            {
                mGraphs[i]->data()->clear();
            }
            PDataVec[i].clear();
            PData[i]=0;
            PDataBuffer[i]=0;
            dataTotalSum[i]=0;
            dataTotalVariance[i]=0;
            data_calib[i]=0;
        }
        receive_data_cnt=0;
        dataCnt=0;
        for(int i=0;i<3;i++)
            angle_xyz[i]=0;
        for(int i=0;i<3;i++)
        {
            onlineVar[i]->clearData();
            fftData[i].clear();
            customplot[i]->xAxis->setRange(0,XRANGE);
            customplot[i]->yAxis->setRange(-10,10);
            customplot[i]->setInteraction(QCP::iRangeZoom,false);
            customplot[i]->setInteraction(QCP::iRangeDrag,false);
        }
        ui->btnStart->setText("Stop");
        status->isrunning=true;
        connect(uart,&SerialPort::receiveDataSignal,this,&MainWindow::receiveDataSlot, Qt::QueuedConnection);
        timer_plot->start();
    }
    else
    {
        disconnect(uart,&SerialPort::receiveDataSignal,this,&MainWindow::receiveDataSlot);
        ui->btnStart->setText("Start");
        status->isrunning=false;
        timer_plot->stop();
        for(int i=0;i<totalCharts;i++)
        {
            customplot[i]->setInteraction(QCP::iRangeZoom,true);
            customplot[i]->setInteraction(QCP::iRangeDrag,true);
        }
    }
}


void MainWindow::on_btnFlash_clicked()
{
    int trate=ui->FlashEdit->text().toInt();
    if(trate>0 && trate<=1000)
    {
        flashRate=trate;
        timer_data->setInterval((int)(1000.0/flashRate));
    }
    else
    {
        QMessageBox::information(this,"Warning","Invalid Input(flash rate should be larger than 0 and smaller than 1000)");
    }
}


void MainWindow::on_speedSlider_valueChanged(int value)
{
    dx_len=value;
}
void MainWindow::paletteColorSlot(QColor color)
{
    QPen pen(color);
    mGraphs[plotSelect]->setPen(pen);
    mTags[plotSelect]->setPen(pen);
    if(isfftTransfer[plotSelect])
        fftwin->changePlotPen(mGraphs[plotSelect]->name(),pen);
    if(isShowALLData[plotSelect])
        allwindow->changePlotPen(mGraphs[plotSelect]->name(),pen);
    QCPSelectionDecorator *decorator=new QCPSelectionDecorator();
    pen.setWidth(2);
    decorator->setPen(pen);
    mGraphs[plotSelect]->setSelectionDecorator(decorator);
    if(status->isrunning==false)
        customplot[plotSelect]->replot();
}
void MainWindow::addFFTplotSlot()
{
    if(!isfftTransfer[plotSelect])
    {
        fftwin->addPlot(mGraphs[plotSelect]->name(), mGraphs[plotSelect]->pen(), 512);
        isfftTransfer[plotSelect]=true;
        fftwin->show();
        fftwin->activateWindow();
    }
    else
    {
        if(fftwin->isVisible())
        {
            fftwin->removePlot(mGraphs[plotSelect]->name());
            isfftTransfer[plotSelect]=false;
        }
        else
        {
            fftwin->show();
        }
    }
}
void MainWindow::addAllDataSlot()
{
    if(!isShowALLData[plotSelect])
    {
        allwindow->addPlot(mGraphs[plotSelect]->name(), mGraphs[plotSelect]->pen());
        isShowALLData[plotSelect]=true;
        allwindow->show();
        allwindow->activateWindow();
    }
    else
    {
        if(allwindow->isVisible())
        {
            allwindow->removePlot(mGraphs[plotSelect]->name());
            isShowALLData[plotSelect]=false;
        }
        else
        {
            allwindow->show();
        }
    }
    QVector<double> data = saver->getdataFromTxt(plotSelect);
    allwindow->initData(data,mGraphs[plotSelect]->name());
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    QFile fileToColor("configs.ini");
    fileToColor.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&fileToColor);
    for(int i=0;i<3;i++)
    {
        QColor color=mGraphs[i]->pen().color();
        int red=color.red(),green=color.green(),blue=color.blue();
        stream<<red<<" "<<green<<" "<<blue<<endl;
    }
    stream<<ui->speedSlider->value()<<endl;
    fileToColor.close();
    fftwin->close();
    allwindow->close();
}
void MainWindow::initStates()
{
    QFileInfo info("configs.ini");
    if(!info.exists())
    {
        qDebug()<<"config file doesn't exist";
    }
    else
    {
        qDebug()<<"config file exists";
        QFile fileToColor("configs.ini");
        fileToColor.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream stream(&fileToColor);
        QString line;
        for(int i=0;i<3;i++)
        {
            line=stream.readLine();
            QStringList list=line.split(' ');
            QPen pen(QColor(list[0].toInt(),list[1].toInt(),list[2].toInt()));
            mGraphs[i]->setPen(pen);
            mTags[i]->setPen(pen);
            QCPSelectionDecorator *decorator=new QCPSelectionDecorator();
            pen.setWidth(2);
            decorator->setPen(pen);
            mGraphs[i]->setSelectionDecorator(decorator);
            customplot[i]->replot();
        }
        line=stream.readLine();
        ui->speedSlider->setValue(line.toInt());
        fileToColor.close();
    }
}
