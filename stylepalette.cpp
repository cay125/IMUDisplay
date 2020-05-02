#include "stylepalette.h"

stylePalette::stylePalette()
{
    m_pColorDialog=NULL;
}

stylePalette::~stylePalette()
{
    if(NULL !=m_pColorDialog)
    {
        delete m_pColorDialog;
        m_pColorDialog=NULL;
    }
}

//获取cor的RGB值，并发射信号signal_changeBackColor()
void stylePalette::slot_getColor( QColor cor)
{
    m_red=cor.red();
    m_green=cor.green();
    m_blue=cor.blue();
    emit signal_changeBackColor(cor);
}

//打开调色板槽函数
void stylePalette::slot_OpenColorPad()
{
    m_pColorDialog = new QColorDialog;
    connect(m_pColorDialog,SIGNAL(colorSelected(QColor)),this,SLOT(slot_getColor(QColor)));
    m_pColorDialog->exec();
}
