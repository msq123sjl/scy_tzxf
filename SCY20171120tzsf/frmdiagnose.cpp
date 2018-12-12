#include "frmdiagnose.h"
#include "ui_frmdiagnose.h"
#include "api/myapi.h"

extern QList<QString> list;

frmdiagnose::frmdiagnose(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmdiagnose)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->InitForm();

    showTimer=new QTimer(this);
    showTimer->setInterval(2000);
    connect(showTimer,SIGNAL(timeout()),this,SLOT(Append()));
    showTimer->start();

}

frmdiagnose::~frmdiagnose()
{
//    delete showTimer;
    delete ui;
}

void frmdiagnose::InitForm()
{
    currentMsgCount = 0;
    maxMsgCount = 20;

}

void frmdiagnose::Append()
{
    foreach(QString str,list){
        if(currentMsgCount>=maxMsgCount){
            ui->txtMain->clear();
            currentMsgCount=0;
        }
        if(str.left(2)=="Tx"){
            ui->txtMain->setTextColor(QColor("dodgerblue"));
        }else if(str.left(2)=="Rx"){
            ui->txtMain->setTextColor(QColor("red"));
        }else{
            ui->txtMain->setTextColor(QColor("darkgreen"));
        }
        ui->txtMain->append(str);
        list.removeFirst();
        currentMsgCount++;
    }

}

void frmdiagnose::on_btnCancel_clicked()
{
    showTimer->stop();
//    delete showTimer;
    this->close();
}

void frmdiagnose::on_btnClear_clicked()
{
    ui->txtMain->clear();
}
