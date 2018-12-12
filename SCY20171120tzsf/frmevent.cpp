#include "frmevent.h"
#include "ui_frmevent.h"
#include "api/myhelper.h"
#include "api/myapi.h"

#define LineNums   10

frmevent::frmevent(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmevent)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    myHelper::FormInCenter(this,myApp::DeskWidth,myApp::DeskHeigth);//窗体居中显示
    this->InitStyle();
    this->InitForm();
}

frmevent::~frmevent()
{
    delete Sqlite;
    delete ui;
}

void frmevent::InitStyle()
{  
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
}

//初始化数据表格并绑定数据
void frmevent::InitForm()
{
    for(int i=2010;i<=2030;i++){
        ui->start_year->addItem(QString::number(i));
        ui->end_year->addItem(QString::number(i));
    }
    for(int i=1;i<=12;i++){
        ui->start_month->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
        ui->end_month->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
    }
    for(int i=1;i<=31;i++){
        ui->start_day->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
        ui->end_day->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
    }
    for(int i=0;i<24;i++){
        ui->start_hour->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
        ui->end_hour->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
    }

    QStringList Now =QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm").split("-");
    ui->start_year->setCurrentIndex(ui->start_year->findText(Now[0]));
    ui->start_month->setCurrentIndex(ui->start_month->findText(Now[1]));
    ui->start_day->setCurrentIndex(ui->start_day->findText(Now[2]));
    ui->start_hour->setCurrentIndex(0);

    ui->end_year->setCurrentIndex(ui->end_year->findText(Now[0]));
    ui->end_month->setCurrentIndex(ui->end_month->findText(Now[1]));
    ui->end_day->setCurrentIndex(ui->end_day->findText(Now[2]));
    ui->end_hour->setCurrentIndex(ui->end_hour->findText(Now[3]));

    Sqlite=new mySqliteAPI(this);//实例化数据库操作对象,以备调用
    //设置需要显示数据的表格和翻页的按钮
    Sqlite->SetControl(ui->tableEvent,ui->labInfo,ui->btnFirst,ui->btnPre,ui->btnNext,ui->btnLast);
    ui->tableEvent->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue}");
    ui->tableEvent->verticalHeader()->setStyleSheet("QHeaderView::section{background:skyblue}");

}

void frmevent::on_btnOk_clicked()
{
    QString startTime;
    QString endTime;
    //构建SQL语句
    startTime=QString("%1-%2-%3 %4:00")
            .arg(ui->start_year->currentText())
            .arg(ui->start_month->currentText())
            .arg(ui->start_day->currentText())
            .arg(ui->start_hour->currentText());

    endTime=QString("%1-%2-%3 %4:00")
            .arg(ui->end_year->currentText())
            .arg(ui->end_month->currentText())
            .arg(ui->end_day->currentText())
            .arg(ui->end_hour->currentText());
        QString sql="where 1=1";

        if (startTime>endTime) {
            myHelper::ShowMessageBoxError("开始时间不能大于结束时间!");
            return;
        }
        sql+=" and datetime([TriggerTime])>='"+startTime+"'";
        sql+=" and datetime([TriggerTime])<='"+endTime+"'";

        //绑定数据到表格
        //初始化表格列名和列宽
        columnNames[0]="时间";
        columnNames[1]="类型";
        columnNames[2]="记录信息";
        columnNames[3]="操作者";

        columnWidths[0]=190;
        columnWidths[1]=130;
        columnWidths[2]=290;
        columnWidths[3]=150;
        Sqlite->BindDataSelect("EventInfo","TriggerTime","desc",sql,LineNums,columnNames,columnWidths);
}

void frmevent::on_btnCancel_clicked()
{
    this->close();
}
