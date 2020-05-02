#include "filesaver.h"
#include <QTextStream>
#include <QDebug>
fileSaver::fileSaver(QString path, bool _saveFlag, QObject *parent) : QObject(parent)
{
    file.setFileName(path);
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    SaveFlag=_saveFlag;
    write_times=0;
}

void fileSaver::writeText(QVector<QString> text)
{
    if(SaveFlag)
    {
        int len=text.size();
        QTextStream out(&file);
        for(int i=0;i<len;i++)
            out<<text[i]<<" ";
        out<<'\r'<<'\n';
    }
    if(write_times>100)
    {
        write_times=0;
        file.close();
        file.open(QIODevice::WriteOnly | QIODevice::Append);
    }
}

void fileSaver::isSave_slot(bool _saveFlag)
{
    SaveFlag=_saveFlag;
    qDebug()<<"current save state is: "<<SaveFlag;
}

QVector<double> fileSaver::getdataFromTxt(int index)
{
    file.seek(0);
    QTextStream stream(&file);
    QString line;
    QVector<double> data;
    while(!stream.atEnd())
    {
        line=stream.readLine();
        QStringList list=line.split(' ');
        data.append(list[index].toDouble());
        if(data.count()>=10000)
            for(int i=1;i<=data.count()/2;i++)
                data.removeAt(i);
    }
    file.seek(file.size());
    return data;
}
