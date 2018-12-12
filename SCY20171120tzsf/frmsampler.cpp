#include "frmsampler.h"
#include "ui_frmsampler.h"
#include "api/myapp.h"
#include "api/myhelper.h"
#include <QSqlQuery>
//extern Sampler sampler;
extern QextSerialPort *myCom[6];
frmSampler::frmSampler(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmSampler)
{
    ui->setupUi(this);
    myHelper::FormInCenter(this,myApp::DeskWidth,myApp::DeskHeigth);//窗体居中显示
    this->InitStyle();
    this->InitForm();
}

frmSampler::~frmSampler()
{
    delete ui;
}

void frmSampler::InitStyle()//此处作用？
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
}
int port=99;
void frmSampler::InitForm()
{
    QSqlQuery query;
    query.exec("select * from [ParaInfo]  where [Name] ='采样仪'");
    if(query.first()){
    port=query.value(3).toString().right(1).toInt()-2;
    }
    ui->txtSampleFlow->setText(QString::number(400));                                                                                                                                  //采样量
    ui->comboBox_SampleStartBottle->setCurrentIndex(0);//开始瓶号

}



//获取采样瓶状态
bool frmSampler::GetSamplerStatus()
{
    int bottle_1=0;
    int bottle_2=0;
    int bottle_3=0;
    int bottle_4=0;
    int bottle_5=0;
    int bottle_6=0;
    int bottle_7=0;
    int bottle_8=0;
    int bottle_9=0;
    int bottle_10=0;
    int bottle_11=0;
    int bottle_12=0;
    int bottle_13=0;
    int bottle_14=0;
    int bottle_15=0;
    int bottle_16=0;
    int bottle_17=0;
    int bottle_18=0;
    int bottle_19=0;
    int bottle_20=0;
    int bottle_21=0;
    int bottle_22=0;
    int bottle_23=0;
    int bottle_24=0;
    QByteArray readbuf;
//    int port=ui->comboBox_SamplePort->currentText().toInt()-2;
    if(port==99)return false;
    myCom[port]->readAll();
    myCom[port]->write("DR@81HB");
    myCom[port]->flush();
    sleep(2);
    readbuf=myCom[port]->readAll();
    if(readbuf.length()>=103&&readbuf.startsWith('D')&&readbuf.endsWith('B'))
    {
       bottle_1=readbuf.mid(5,4).toInt();
       bottle_2=readbuf.mid(9,4).toInt();
       bottle_3=readbuf.mid(13,4).toInt();
       bottle_4=readbuf.mid(17,4).toInt();
       bottle_5=readbuf.mid(21,4).toInt();
       bottle_6=readbuf.mid(25,4).toInt();
       bottle_7=readbuf.mid(29,4).toInt();
       bottle_8=readbuf.mid(33,4).toInt();
       bottle_9=readbuf.mid(37,4).toInt();
       bottle_10=readbuf.mid(41,4).toInt();
       bottle_11=readbuf.mid(45,4).toInt();
       bottle_12=readbuf.mid(49,4).toInt();
       bottle_13=readbuf.mid(53,4).toInt();
       bottle_14=readbuf.mid(57,4).toInt();
       bottle_15=readbuf.mid(61,4).toInt();
       bottle_16=readbuf.mid(65,4).toInt();
       bottle_17=readbuf.mid(69,4).toInt();
       bottle_18=readbuf.mid(73,4).toInt();
       bottle_19=readbuf.mid(77,4).toInt();
       bottle_20=readbuf.mid(81,4).toInt();
       bottle_21=readbuf.mid(85,4).toInt();
       bottle_22=readbuf.mid(89,4).toInt();
       bottle_23=readbuf.mid(93,4).toInt();
       bottle_24=readbuf.mid(97,4).toInt();

       ui->bottle_status_1->setText(QString::number(bottle_1,10));
       ui->bottle_status_2->setText(QString::number(bottle_2,10));
       ui->bottle_status_3->setText(QString::number(bottle_3,10));
       ui->bottle_status_4->setText(QString::number(bottle_4,10));
       ui->bottle_status_5->setText(QString::number(bottle_5,10));
       ui->bottle_status_6->setText(QString::number(bottle_6,10));
       ui->bottle_status_7->setText(QString::number(bottle_7,10));
       ui->bottle_status_8->setText(QString::number(bottle_8,10));
       ui->bottle_status_9->setText(QString::number(bottle_9,10));
       ui->bottle_status_10->setText(QString::number(bottle_10,10));
       ui->bottle_status_11->setText(QString::number(bottle_11,10));
       ui->bottle_status_12->setText(QString::number(bottle_12,10));
       ui->bottle_status_13->setText(QString::number(bottle_13,10));
       ui->bottle_status_14->setText(QString::number(bottle_14,10));
       ui->bottle_status_15->setText(QString::number(bottle_15,10));
       ui->bottle_status_16->setText(QString::number(bottle_16,10));
       ui->bottle_status_17->setText(QString::number(bottle_17,10));
       ui->bottle_status_18->setText(QString::number(bottle_18,10));
       ui->bottle_status_19->setText(QString::number(bottle_19,10));
       ui->bottle_status_20->setText(QString::number(bottle_20,10));
       ui->bottle_status_21->setText(QString::number(bottle_21,10));
       ui->bottle_status_22->setText(QString::number(bottle_22,10));
       ui->bottle_status_23->setText(QString::number(bottle_23,10));
       ui->bottle_status_24->setText(QString::number(bottle_24,10));
        return true;
    }
    else return false;
}


void frmSampler::on_btn_Cancel_clicked()
{
    myApp::SampleMode=0;
    this->close();
}

void frmSampler::on_btn_GetSamplerStatus_clicked()
{
    GetSamplerStatus();
}

void frmSampler::on_btn_SyncSampling_clicked()
{
    QString sendbuf;
    QString readbuf;
    int sample_capacity=ui->txtSampleFlow->text().toInt() ;
    int current_bottle=ui->comboBox_SampleStartBottle->currentText().toInt();
    if(port==99)return;
    myCom[port]->write("DR@35001040200550600000000000000000HB");
    myCom[port]->flush();
    //qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toAscii();
    sleep(10);
    readbuf=myCom[port]->readAll();
     sendbuf=QString( "DR@1506012401%1%200000000000000000000002000000060HB").arg(sample_capacity,3,10,QChar('0')).arg(current_bottle,2,10,QChar('0'));
    myCom[port]->write(sendbuf.toAscii());
    //qDebug()<<sendbuf;
     myCom[port]->flush();
     myHelper::Sleep(10);
    readbuf=myCom[port]->readAll();
    myCom[port]->write("DR@8001HB");
    myCom[port]->flush();
    myHelper::Sleep(2);
    readbuf=myCom[port]->readAll();
    if(readbuf=="DR@80OKHB"){
        myHelper::ShowMessageBoxInfo("同步采样成功");
    }
    else{
        myHelper::ShowMessageBoxError("同步采样失败");
    }
}


void frmSampler::on_btn_OverSampling_clicked()
{
    QString readbuf;
    if(port==99)return;
    myCom[port]->readAll();
    myCom[port]->write("DR@8002HB");
    myCom[port]->flush();
    myHelper::Sleep(2);
    readbuf=myCom[port]->readAll();
    //qDebug()<<readbuf;
    if(readbuf=="DR@80OKHB"){
        myHelper::ShowMessageBoxInfo("超标留样成功");
    }
    else{
        myHelper::ShowMessageBoxError("超标留样失败");
    }

}
