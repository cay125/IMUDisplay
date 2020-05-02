#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QDebug>
#include <QObject>
#include <QThread>
#include <QMainWindow>
#include <QString>

enum recieveType
{
    gyro='0',angle
};

class SerialPort : public QObject
{
  Q_OBJECT
public:
  explicit SerialPort(QObject *parent = nullptr);
  ~SerialPort();

  void init_port();  //初始化串口
//  void start_port(QString,int);

public slots:
  void handle_data();  //处理接收到的数据
  void write_data();     //发送数据
  void startToConnectSlot(QString,int,int);
  void uartCloseSlot();

signals:
  //接收数据
  void receiveDataSignal(QByteArray);
  void connectedSignal();

private:
  QByteArray pointData;
  QThread *worker_thread;
  QSerialPort *port;
};



#endif // SERIALPORT_H
