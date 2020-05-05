#include "serialport.h"
#include <QSerialPortInfo>
#include <chrono>
SerialPort::SerialPort(QObject *parent) : QObject(parent)
{
    worker_thread = new QThread();
    port = new QSerialPort();
    this->moveToThread(worker_thread);
    port->moveToThread(worker_thread);
    worker_thread->start();  //启动线程
}

SerialPort::~SerialPort()
{
    port->close();
    port->deleteLater();
    worker_thread->quit();
    worker_thread->wait();
    worker_thread->deleteLater();
}

void SerialPort::init_port()
{
    port->setDataBits(QSerialPort::Data8);             //数据位
    port->setStopBits(QSerialPort::OneStop);           //停止位
    port->setParity(QSerialPort::NoParity);            //奇偶校验
    port->setFlowControl(QSerialPort::NoFlowControl);  //流控制
}
void SerialPort::startToConnectSlot(QString portname, int baudrate, int parity)
{
    init_port();
    port->setPortName(portname);
    port->setBaudRate(baudrate);
    if(parity==0)
        port->setParity(QSerialPort::OddParity);
    else if(parity==1)
        port->setParity(QSerialPort::EvenParity);
    else
        port->setParity(QSerialPort::NoParity);
    if (port->open(QIODevice::ReadWrite))
    {
        qDebug() << "Port have been opened";
        connect(port, SIGNAL(readyRead()), this, SLOT(handle_data())); //Qt::DirectConnection
        emit connectedSignal();
    }
    else
    {
        qDebug() << "open uart failed";
    }
}
void SerialPort::uartCloseSlot()
{
    port->close();
    qDebug() << "Port have been closed";
}

void SerialPort::handle_data()
{
    static bool first_flag=false;
    static std::chrono::steady_clock::time_point t_time=std::chrono::steady_clock::now();
    static std::pair<int,recieveType> state(0, recieveType::angle);
    auto data=port->readAll();
    //qDebug()<<"worker thread id: "<<QThread::currentThreadId();
    for(int i=0;i<data.size();i++)
    {
        uchar num=static_cast<uchar>(data.at(i));
        if(state.first==0 && num==0x55)
        {
            pointData.clear();
            state.first=1;
        }
        else if(state.first==1)
        {
            if(num!=0x52 && num!=0x53)
            {
                state.first=0;
            }
            else if(num==0x52)
            {
                state.first++;
                pointData.append(recieveType::gyro);
                state.second=recieveType::gyro;
            }
            else if(num==0x53)
            {
                state.first++;
                pointData.append(recieveType::angle);
                state.second=recieveType::angle;
            }
        }
        else if(state.first>1)
        {
            state.first++;
            pointData.append(data.at(i));
            if(state.first>=8)
            {
                state.first=0;
                if(state.second==recieveType::angle)
                    emit receiveDataSignal(pointData);
                if(!first_flag)
                {
                    first_flag=true;
                    t_time=std::chrono::steady_clock::now();
                }
                else
                {
                    auto current_time=std::chrono::steady_clock::now();
                    std::chrono::duration<double, std::milli> dTimeSpan = std::chrono::duration<double,std::milli>(current_time-t_time);
                    qDebug()<<"ms count: "<<dTimeSpan.count();
                    t_time=current_time;
                }
            }
        }
    }
}

void SerialPort::write_data()
{
    qDebug() << "write_id is:" << QThread::currentThreadId();
    port->write("data", 4);
}
