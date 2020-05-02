#ifndef STYLEPALETTE_H
#define STYLEPALETTE_H
#include <QObject>
#include <QWidget>
#include <QPalette>
#include <QColor>
#include <QLineEdit>
#include <QPushButton>
#include <QColorDialog>
#include <QString>
class stylePalette:public QWidget
{
    Q_OBJECT
public:
    stylePalette();
    ~stylePalette();
public slots:
    void slot_getColor(QColor);//获取调色板rgb值
    void slot_OpenColorPad();//打开调色板
signals:
    void signal_changeBackColor(QColor);//改变背景色信号函数
private:
    QColorDialog *m_pColorDialog;//调色板显示类
    int m_red;//颜色的三色，红绿蓝
    int m_green;
    int m_blue;
};

#endif // STYLEPALETTE_H
