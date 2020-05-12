#ifndef DATAMANAGERWINDOW_H
#define DATAMANAGERWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QButtonGroup>
#include <QSpinBox>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <map>

namespace Ui {
class dataManagerWindow;
}

class dataManager
{
public:
    bool isRightAxis=false;
    int chartIndex=0;
    dataManager(bool _isRightAxis,int _chartIndex){isRightAxis=_isRightAxis;chartIndex=_chartIndex;}
};
class dataManagerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit dataManagerWindow(QWidget *parent = nullptr);
    ~dataManagerWindow();

private:
    Ui::dataManagerWindow *ui;
    QVector<QButtonGroup*> groups;
    QVector<QRadioButton*> radios;
    QVector<QSpinBox*> spinIntBoxs;
    QDialogButtonBox* btnBox;
    int totalNum=8;
signals:
    void dataManagerSignal(std::map<QString,dataManager>);
};

#endif // DATAMANAGERWINDOW_H
