#include "datamanagerwindow.h"
#include "ui_datamanagerwindow.h"
#include <QPalette>
#include <QString>
#include <QLayout>
#include <QMessageBox>

dataManagerWindow::dataManagerWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::dataManagerWindow)
{
    ui->setupUi(this);
    QPalette p;
    p.setColor(QPalette::Background,Qt::white);
    setAutoFillBackground(true);
    setPalette(p);
    QVector<QString> name={"Top IMU","Bottom IMU","PID out","Ref Legs","Real Legs","Real gyro","Ref Leg Vel"};
    for(int i=0;i<totalNum;i++)
    {
        QHBoxLayout *Hlayout=new QHBoxLayout(this);
        groups.push_back(new QButtonGroup);
        for(int j=0;j<3;j++)
        {
            radios.push_back(new QRadioButton);
            radios.back()->setMinimumHeight(20);
            radios.back()->setFont(QFont("Microsoft YaHei", 9, QFont::Normal,false));
        }
        radios[i*3]->setText(name[i]);
        radios[i*3]->setMinimumWidth(120);
        Hlayout->addWidget(radios[i*3]);
        radios[i*3+1]->setText("Left Axis");
        radios[i*3+1]->setChecked(true);
        radios[i*3+2]->setText("Right Axis");
        groups.push_back(new QButtonGroup);
        groups.back()->addButton(radios[i*3]);
        groups.back()->setExclusive(false);
        groups.push_back(new QButtonGroup);
        spinIntBoxs.push_back(new QSpinBox);
        spinIntBoxs.back()->setValue(0);
        Hlayout->addWidget(spinIntBoxs.back());
        for(int j=0;j<2;j++)
        {
            groups.back()->addButton(radios[i*3+j+1]);
            Hlayout->addWidget(radios[i*3+j+1]);
        }
        ui->mainLayout->addLayout(Hlayout,i,0);
    }
    ui->mainLayout->setRowStretch(totalNum,1);
    btnBox=new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    ui->mainLayout->addWidget(btnBox,totalNum+1,0);
    connect(btnBox,&QDialogButtonBox::accepted,this,[this]()
    {
        int cnt=0;
        std::map<QString,dataManager> data;
        for(int i=0;i<totalNum;i++)
        {
            if(radios[i*3]->isChecked())
            {
                cnt++;
                dataManager d(radios[i*3+2]->isChecked(),spinIntBoxs[i]->value());
                data.insert(std::pair<QString,dataManager>(radios[i*3]->text(),d));
            }
        }
        if(cnt)
        {
            emit dataManagerSignal(data);
            close();
        }
        else
        {
            QMessageBox::information(this,"Warning","Invalid Input");
        }
    });
    connect(btnBox,&QDialogButtonBox::rejected,this,[this]()
    {
        close();
    });
}

dataManagerWindow::~dataManagerWindow()
{
    delete ui;
}
