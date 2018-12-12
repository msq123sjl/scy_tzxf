#include "frmdata.h"
#include "ui_frmdata.h"
#include "api/myhelper.h"

#define LineNums   9

frmdata::frmdata(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmdata)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    myHelper::FormInCenter(this,myApp::DeskWidth,myApp::DeskHeigth);//窗体居中显示
    this->InitStyle();
    this->InitForm();
}

frmdata::~frmdata()
{
    delete Sqlite;
    delete ui;
}

void frmdata::InitStyle()
{    
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
}

//初始化数据表格并绑定数据
void frmdata::InitForm()
{
    //加载年月日时分下拉框
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
    for(int i=0;i<60;i++){
        ui->start_minute->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
        ui->end_minute->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
    }

    QStringList Now =QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm").split("-");
    ui->start_year->setCurrentIndex(ui->start_year->findText(Now[0]));
    ui->start_month->setCurrentIndex(ui->start_month->findText(Now[1]));
    ui->start_day->setCurrentIndex(ui->start_day->findText(Now[2]));
    ui->start_hour->setCurrentIndex(0);
    ui->start_minute->setCurrentIndex(0);

    ui->end_year->setCurrentIndex(ui->end_year->findText(Now[0]));
    ui->end_month->setCurrentIndex(ui->end_month->findText(Now[1]));
    ui->end_day->setCurrentIndex(ui->end_day->findText(Now[2]));
    ui->end_hour->setCurrentIndex(ui->end_hour->findText(Now[3]));
    ui->end_minute->setCurrentIndex(ui->end_minute->findText(Now[4]));

    QSqlQuery query;
    query.exec("select [Name] from [ParaInfo]");
    while(query.next()){
        ui->comboBoxPara->addItem(query.value(0).toString());
    }
    Sqlite=new mySqliteAPI(this);//实例化数据库操作对象,以备调用
    //设置需要显示数据的表格和翻页的按钮
    Sqlite->SetControl(ui->tableData,ui->labInfo,ui->btnFirst,ui->btnPre,ui->btnNext,ui->btnLast);

    //设置表头背景色
    ui->tableData->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue}");
    ui->tableData->verticalHeader()->setStyleSheet("QHeaderView::section{background:skyblue}");
    //表头信息显示居中
    ui->tableData->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);
}

//查询数据
void frmdata::on_btnOk_clicked()
{
    if(ui->comboBoxPara->currentText()==""){
        myHelper::ShowMessageBoxError("因子名称不能为空!");
        return;
    }
    //构建SQL语句
    QSqlQuery query;
    QString sql;
    QString Code;
    sql="select [Code] from [ParaInfo] where [Name]='"+ui->comboBoxPara->currentText()+"'";
    query.setForwardOnly(true);
    query.exec(sql);
    if(query.next()){
    Code=query.value(0).toString();
    }
    sql="where 1=1";
    QString startTime;
    QString endTime;
    //绑定数据到表格
    //实时数据
    switch (ui->comboBoxDataType->currentIndex())
    {
    case 0://实时数据
        startTime=QString("%1-%2-%3 %4:%5")
                .arg(ui->start_year->currentText())
                .arg(ui->start_month->currentText())
                .arg(ui->start_day->currentText())
                .arg(ui->start_hour->currentText())
                .arg(ui->start_minute->currentText());
        endTime=QString("%1-%2-%3 %4:%5")
                .arg(ui->end_year->currentText())
                .arg(ui->end_month->currentText())
                .arg(ui->end_day->currentText())
                .arg(ui->end_hour->currentText())
                .arg(ui->end_minute->currentText());
        if (startTime>endTime) {
            myHelper::ShowMessageBoxError("开始时间不能大于结束时间!");
            return;
        }
        if (startTime.left(10)!=endTime.left(10)) {
            myHelper::ShowMessageBoxError("设置开始时间和结束时间在同一天!");
            return;
        }
        columnNames[0]="数据采集时间";
        columnNames[1]="采集值";
        columnNames[2]="数据标记";
        columnNames[3]=" ";
        columnNames[4]=" ";

        columnWidths[0]=280;
        columnWidths[1]=255;
        columnWidths[2]=225;
        columnWidths[3]=0;
        columnWidths[4]=0;
        sql+=" and datetime([GetTime])>='"+startTime+"'";
        sql+=" and datetime([GetTime])<='"+endTime+"'";
        Sqlite->BindDataSelect(QString("Rtd_%1_%2%3%4").arg(Code).arg(ui->start_year->currentText())
                               .arg(ui->start_month->currentText()).arg(ui->start_day->currentText()),
                               "GetTime","desc",sql,LineNums,columnNames,columnWidths);
        break;
    case 1://分钟数据
        startTime=QString("%1-%2-%3 %4:%5")
                .arg(ui->start_year->currentText())
                .arg(ui->start_month->currentText())
                .arg(ui->start_day->currentText())
                .arg(ui->start_hour->currentText())
                .arg(ui->start_minute->currentText());
        endTime=QString("%1-%2-%3 %4:%5")
                .arg(ui->end_year->currentText())
                .arg(ui->end_month->currentText())
                .arg(ui->end_day->currentText())
                .arg(ui->end_hour->currentText())
                .arg(ui->end_minute->currentText());
        if (startTime>endTime) {
            myHelper::ShowMessageBoxError("开始时间不能大于结束时间!");
            return;
        }
        columnNames[0]="数据采集时间";
        columnNames[1]="最大值";
        columnNames[2]="最小值";
        columnNames[3]="平均值";
        columnNames[4]="累积值";

        columnWidths[0]=200;
        columnWidths[1]=145;
        columnWidths[2]=145;
        columnWidths[3]=145;
        columnWidths[4]=145;
        sql+=" and datetime([GetTime])>='"+startTime+"'";
        sql+=" and datetime([GetTime])<='"+endTime+"'";
        Sqlite->BindDataSelect(QString("Mins_%1").arg(Code),"GetTime","desc",sql,LineNums,columnNames,columnWidths);
        break;
    case 2://小时数据
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
        if (startTime>endTime) {
            myHelper::ShowMessageBoxError("开始时间不能大于结束时间!");
            return;
        }
        columnNames[0]="数据采集时间";
        columnNames[1]="最大值";
        columnNames[2]="最小值";
        columnNames[3]="平均值";
        columnNames[4]="累积值";

        columnWidths[0]=200;
        columnWidths[1]=145;
        columnWidths[2]=145;
        columnWidths[3]=145;
        columnWidths[4]=145;
        sql+=" and datetime([GetTime])>='"+startTime+"'";
        sql+=" and datetime([GetTime])<='"+endTime+"'";
        Sqlite->BindDataSelect(QString("Hour_%1").arg(Code),"GetTime","desc",sql,LineNums,columnNames,columnWidths);
        break;
    case 3://日数据
        startTime=QString("%1-%2-%3 00:00")
                .arg(ui->start_year->currentText())
                .arg(ui->start_month->currentText())
                .arg(ui->start_day->currentText());
        endTime=QString("%1-%2-%3 00:00")
                .arg(ui->end_year->currentText())
                .arg(ui->end_month->currentText())
                .arg(ui->end_day->currentText());
        if (startTime>endTime) {
            myHelper::ShowMessageBoxError("开始时间不能大于结束时间!");
            return;
        }
        columnNames[0]="数据采集时间";
        columnNames[1]="最大值";
        columnNames[2]="最小值";
        columnNames[3]="平均值";
        columnNames[4]="累积值";

        columnWidths[0]=200;
        columnWidths[1]=145;
        columnWidths[2]=145;
        columnWidths[3]=145;
        columnWidths[4]=145;
        sql+=" and datetime([GetTime])>='"+startTime+"'";
        sql+=" and datetime([GetTime])<='"+endTime+"'";
        Sqlite->BindDataSelect(QString("Day_%1").arg(Code),"GetTime","desc",sql,LineNums,columnNames,columnWidths);
        break;

    }

}

void frmdata::on_btnCancel_clicked()
{
    this->close();
}

void frmdata::on_comboBoxDataType_currentIndexChanged(int index)
{
    if(index==0){//实时数据
        ui->start_minute->setEnabled(true);
        ui->start_hour->setEnabled(true);
        ui->end_minute->setEnabled(true);
        ui->end_hour->setEnabled(true);
    }
    else if(index==1){//分钟数据
        ui->start_minute->setEnabled(true);
        ui->start_hour->setEnabled(true);
        ui->end_minute->setEnabled(true);
        ui->end_hour->setEnabled(true);
    }
    else if(index==2){//小时数据
        ui->start_minute->setEnabled(false);
        ui->start_hour->setEnabled(true);
        ui->end_minute->setEnabled(false);
        ui->end_hour->setEnabled(true);
    }
    else if(index==3){//日数据
        ui->start_minute->setEnabled(false);
        ui->start_hour->setEnabled(false);
        ui->end_minute->setEnabled(false);
        ui->end_hour->setEnabled(false);
    }
}
