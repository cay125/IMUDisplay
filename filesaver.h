#ifndef FILESAVER_H
#define FILESAVER_H
#include <QString>
#include <QFile>
#include <QVector>
#include <QObject>
class fileSaver : public QObject
{
    Q_OBJECT
public:
    fileSaver(QString, bool, QObject *parent = NULL);
    void writeText(QVector<QString>);
    QVector<double> getdataFromTxt(int index);
    bool SaveFlag;
private:
    QFile file;
    int write_times;
public slots:
    void isSave_slot(bool);
};

#endif // FILESAVER_H
