#ifndef DATAMANAGERWINDOW_H
#define DATAMANAGERWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QButtonGroup>
#include <QSpinBox>
#include <QRadioButton>
#include <QDialogButtonBox>

namespace Ui {
class dataManagerWindow;
}

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
    int totalNum=7;
};

#endif // DATAMANAGERWINDOW_H
