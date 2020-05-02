#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMessageBox>
#include <QLineSeries>
#include <QChartView>
#include <QLegendMarker>
#include <QLineSeries>
#include <QXYLegendMarker>
#include <QTcpSocket>
#include <QChart>
#include <map>
#include <QTimer>
#include <QLineEdit>
#include <QLabel>
#include <QMetaType>
#include <QVariant>
#include "qcustomplot.h"
#include "axistag.h"
#include "serialport.h"
#include "onlinevarian.h"
#include "stylepalette.h"
#include "fftwindow.h"
#include "fftloader.h"
#include "filesaver.h"
#include "alldatawindow.h"

QT_CHARTS_USE_NAMESPACE
namespace Ui {class MainWindow;class Status;}
#define XRANGE 10000
#define USE_ORIGINAL_DATA 1
class Status
{
public:
    Status()
    {
        isrunning=false;
        isconnected=false;
    }
    bool isrunning;
    bool isconnected;
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void initStates();
    fileSaver *saver;
    onlineVarian *onlineVarToTxt[6];
    allDataWindow *allwindow;
    Ui::MainWindow *ui;
    QPalette p;
    QVector<QCustomPlot*> customplot;
    QVector<AxisTag*> mTags;
    QVector<AxisTag*> mAveTags;
    QVector<QLineEdit*> dataEdit;
    QVector<QLabel*> dataLabel;
    QLabel *varianceLabel[21];
    QTimer *timer_plot;
    QTcpSocket *tcpClient;
    SerialPort *uart;
    quint16 count;
    Status* status;
    int totalCharts=2;
    int totallines=4;
    QVector<int> PData;
    QVector<long long> PDataBuffer;
    QVector<QVector<double>> OriginalDataVec;
    QVector<QVector<double>> PDataVec;
    QVector<int> chartLine[6];
    QVector<QCPGraph*> mGraphs;
    std::map<QString,int> findLineByName;
    std::map<int,int> line2Chart;
    QVector<QString> SeriesUnit;
    QVector<QString> SeriesName;
    int mainGraph[6]={0};
    int plotSelect=0,dataCnt=0;
    double dataTotalSum[21]={0},dataTotalVariance[21]={0};
    double data_calib[21]={0};
    int dx=0;double dx_len=2;
    int flashRate=1;
    int receive_data_cnt=0;
    QTimer *timer_data;
    double gra_accel=1;
    double angle_xyz[3]={0};
    onlineVarian *onlineVar[6];
    stylePalette *linePalette;
    fftWindow *fftwin;
    fftLoader *fftloader;
    QThread *fft_thread;
    QVector<double> fftData[6];
    bool isfftTransfer[6]={false};
    bool isShowALLData[6]={false};
private slots:
    void timerSlot_data();
    void timerSlot_customplot();
    void ReadError(QAbstractSocket::SocketError);
    void on_btnOpenGL_clicked();
    void on_btnConnect_clicked();
    void receiveDataSlot(QByteArray);
    void connectedSlot();
    void on_btnStart_clicked();
    void contextMenuRequest(QPoint);
    void legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*);
    void moveLegend();
    void removeSelectedGraph();
    void removeAllGraphs();
    void selectionChanged();
    void applyMainGraph();
    void setLineVisible();

    void on_btnFlash_clicked();

    void on_speedSlider_valueChanged(int value);
    void paletteColorSlot(QColor);
    void addFFTplotSlot();
    void addAllDataSlot();

signals:
    void startToConnectSignal(QString,int,int);
    void uartCloseSignal();
    void FFTstart_signal(QVariant,QString);

protected:
    void closeEvent(QCloseEvent *event);

};

#endif // MAINWINDOW_H
