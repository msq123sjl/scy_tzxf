#include "frmconfig.h"
#include "ui_frmconfig.h"
#include "api/myapp.h"
#include "api/myhelper.h"
#include "frmmain.h"
#include "api/myapi.h"
#include "frmpwd.h"
#include <QSqlQueryModel>

extern Com_para COM[6];
extern Tcp_para TCP[4];

frmconfig::frmconfig(QWidget *parent) : QWidget(parent),
    ui(new Ui::frmconfig)
{
    ui->setupUi(this);  
    this->setAttribute(Qt::WA_DeleteOnClose);
    myHelper::FormInCenter(this,myApp::DeskWidth,myApp::DeskHeigth);//窗体居中显示
    this->InitStyle();                          //这个功能隐藏界面的？
    this->InitForm();                         //
    this->ReadUserInfo();
    this->InitParaInfoModel();
    this->ReadParaInfo();

}

frmconfig::~frmconfig()
{
    delete ui;
}

void frmconfig::InitStyle()
{    
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);//
}

void frmconfig::InitForm()
{
    ui->stackedWidget_config->setCurrentIndex(0);
    //加载配置文件对应界面展示信息

    //基本设置
    ui->txtMN->setText(myApp::MN);                                                                  //MN号
    ui->txtRtdInterval->setText(QString::number(myApp::RtdInterval));         //实时数据间隔
//    ui->txtAlarmTarget->setText(myApp::AlarmTarget);                                    //上位机地址
    ui->txtSampleIntervalTime->setText(QString::number(myApp::SampleIntervalTime));
//    ui->txtCatchmentTime->setText(QString::number(myApp::CatchmentTime)); //集水时间
//    ui->txtDrainTime->setText(QString::number(myApp::DrainTime));               //排水时间
    ui->txtMinInterval->setText(QString::number(myApp::MinInterval));       //分钟数据间隔
//    ui->txtPW->setText(myApp::PW);                                                                  //访问密码
    ui->comboBoxStType->setCurrentIndex(myApp::StType);                         //污染源类型
    ui->txtOverTime->setText(QString::number(myApp::OverTime));             //上传周期
    ui->txtReCount->setText(QString::number(myApp::ReCount));                 //超时重发次数
    ui->txtAlarmTime->setText(QString::number(myApp::AlarmTime));         //超时报警时间
    ui->btn_RespondOpen->SetCheck(myApp::RespondOpen);                      //上位机应答

    //因子设置
    QSqlQuery query;
    query.exec("select * from [Protocol]");
    while(query.next()){
        ui->comboBox_protocol->addItem(query.value(0).toString());
    }
    query.exec("select * from [Unit]");
    while(query.next()){
        ui->comboBox_unit->addItem(query.value(0).toString());
    }

    //串口设置
    on_comboBox_serial_currentIndexChanged(0);

    //站点设置
    ui->btn_server1Open->SetCheck(TCP[0].ServerOpen);
    ui->txtIP1->setText(TCP[0].ServerIP);
    ui->txtPort1->setText(TCP[0].ServerPort);

    ui->btn_server2Open->SetCheck(TCP[1].ServerOpen);
    ui->txtIP2->setText(TCP[1].ServerIP);
    ui->txtPort2->setText(TCP[1].ServerPort);

    ui->btn_server3Open->SetCheck(TCP[2].ServerOpen);
    ui->txtIP3->setText(TCP[2].ServerIP);
    ui->txtPort3->setText(TCP[2].ServerPort);

    ui->btn_server4Open->SetCheck(TCP[3].ServerOpen);
    ui->txtIP4->setText(TCP[3].ServerIP);
    ui->txtPort4->setText(TCP[3].ServerPort);

    ui->btn_com3ToServerOpen->SetCheck(myApp::COM3ToServerOpen);
    //用户设置

    //本机网络设置

    myApp::ReadLocalNet();
    ui->btn_Autogetip->SetCheck(myApp::DHCP=="1"?true:false);
    ui->txtLocalIP->setText(myApp::LocalIP);
    ui->txtMask->setText(myApp::Mask);
    ui->txtGateWay->setText(myApp::GateWay);
    if(myApp::DHCP=="1")
    {
        ui->txtLocalIP->setEnabled(false);
        ui->txtMask->setEnabled(false);
        ui->txtGateWay->setEnabled(false);
    }
    else {
        ui->txtLocalIP->setEnabled(true);
        ui->txtMask->setEnabled(true);
        ui->txtGateWay->setEnabled(true);
    }

    //日期时间
    //加载年月日时分秒下拉框
    for(int i=2010;i<=2030;i++){
        ui->comboBox_year->addItem(QString::number(i));
    }
    for(int i=1;i<=12;i++){
        ui->comboBox_month->addItem(QString::number(i));
    }
    for(int i=1;i<=31;i++){
        ui->comboBox_day->addItem(QString::number(i));
    }
    for(int i=0;i<24;i++){
        ui->comboBox_hour->addItem(QString::number(i));
    }
    for(int i=0;i<60;i++){
        ui->comboBox_minute->addItem(QString::number(i));
        ui->comboBox_second->addItem(QString::number(i));
    }
    //加载当前时间
    QStringList Now =QDateTime::currentDateTime().toString("yyyy-M-d-h-m-s").split("-");
    ui->comboBox_year->setCurrentIndex(ui->comboBox_year->findText(Now[0]));
    ui->comboBox_month->setCurrentIndex(ui->comboBox_month->findText(Now[1]));
    ui->comboBox_day->setCurrentIndex(ui->comboBox_day->findText(Now[2]));
    ui->comboBox_hour->setCurrentIndex(ui->comboBox_hour->findText(Now[3]));
    ui->comboBox_minute->setCurrentIndex(ui->comboBox_minute->findText(Now[4]));
    ui->comboBox_second->setCurrentIndex(ui->comboBox_second->findText(Now[5]));
//加载串口配置信息
    myApp::ReadIoConfig();
    int p_index=0;
    p_index=myApp::Out_drain_open-6;
    if(p_index>3)p_index-=6;
    if(p_index>5)p_index-=6;
    ui->comboOut_drain_open->setCurrentIndex(p_index);

    p_index=myApp::Out_drain_close-6;
    if(p_index>3)p_index-=6;
    if(p_index>5)p_index-=6;
    ui->comboOut_drain_close->setCurrentIndex(p_index);

    p_index=myApp::Out_catchment_open-6;
    if(p_index>3)p_index-=6;
    if(p_index>5)p_index-=6;
    ui->comboOut_catchment_open->setCurrentIndex(p_index);


    p_index=myApp::In_power-10;
    if(p_index>5)p_index-=2;
    ui->comboIn_power->setCurrentIndex(p_index);
}

//切换到常规设置界面
void frmconfig::on_btn_general_clicked()
{
    ui->stackedWidget_config->setCurrentIndex(0);
}

//切换到通道设置界面
void frmconfig::on_btn_parameter_clicked()
{
    ui->stackedWidget_config->setCurrentIndex(1);
}
//切换到串口设置界面
void frmconfig::on_btn_serial_clicked()
{
    ui->stackedWidget_config->setCurrentIndex(2);
}

//切换到站点设置界面
void frmconfig::on_btn_site_clicked()
{
    ui->stackedWidget_config->setCurrentIndex(3);
}

//切换到用户设置界面
void frmconfig::on_btn_user_clicked()
{
    ui->stackedWidget_config->setCurrentIndex(4);
}

//切换到系统更新界面
void frmconfig::on_btn_updateSystem_clicked()
{
    ui->stackedWidget_config->setCurrentIndex(5);
}

//切换到日期时间界面
void frmconfig::on_btn_datetime_clicked()
{
    ui->stackedWidget_config->setCurrentIndex(6);
}

//切换到本机网络界面
void frmconfig::on_btn_localNet_clicked()
{
    ui->stackedWidget_config->setCurrentIndex(7);
}
//开关量配置
void frmconfig::on_btn_io_clicked()
{
        ui->stackedWidget_config->setCurrentIndex(8);
}


//*************************************串口设置*********************************************/
/*********************************************
 *串口号选择界面加载对应端口信息
 *********************************************/
void frmconfig::on_comboBox_serial_currentIndexChanged(int index)
{
    ui->comboBox_baudrate->setCurrentIndex(ui->comboBox_baudrate->findText(QString("%1").arg(COM[index].Baudrate)));
    ui->comboBox_databits->setCurrentIndex(ui->comboBox_databits->findText(QString("%1").arg(COM[index].Databits)));
    ui->comboBox_parity->setCurrentIndex(COM[index].Parity);
    ui->comboBox_stopbits->setCurrentIndex(COM[index].Stopbits);
    ui->txtuartInterval->setText(QString("%1").arg(COM[index].Interval));
    ui->txtuartTimeout->setText(QString("%1").arg(COM[index].Timeout));
}
//*************************************串口设置结束*****************************************/
//*************************************用户设置*********************************************/
/*********************************************
 *读取并加载用户信息
 *********************************************/

void frmconfig::ReadUserInfo()
{  
    ui->listUser->clear();
    TempUserName.clear();
    TempUserInfo.clear();

    QSqlQuery query;
    query.exec("select [UserName],[UserPwd],[UserType] from [UserInfo]");
    if(myApp::CurrentUserType!="管理员"){
        while(query.next()){
            if(query.value(2).toString()==myApp::CurrentUserType){
            TempUserName<<query.value(0).toString();
            TempUserInfo<<QString("%1|%2|%3").arg(query.value(0).toString()) .arg(query.value(1).toString()).arg(query.value(2).toString());
            }
        }

    }
    else {
        while(query.next()){
            TempUserName<<query.value(0).toString();
            TempUserInfo<<QString("%1|%2|%3").arg(query.value(0).toString()) .arg(query.value(1).toString()).arg(query.value(2).toString());
        }
    }

    ui->listUser->addItems(TempUserName);
    int count = ui->listUser->count();
    for(int i = 0; i < count; i++)
    {
     QListWidgetItem *item = ui->listUser->item(i);
     item->setSizeHint(QSize(item->sizeHint().width(), 35));
    }
    if (TempUserName.count()>0) {
        ui->listUser->setCurrentRow(TempUserName.count()-1);
    }

}

/*********************************************
 *用户名选中界面加载对应信息
 *********************************************/
void frmconfig::on_listUser_currentRowChanged(int currentRow)
{
    if (currentRow<0){return;}
    QStringList user=TempUserInfo[currentRow].split("|");
    ui->txtUserName->setText(user[0]);
    ui->txtUserPwd->setText(user[1]);
    ui->comboBox_UserType->setCurrentIndex(ui->comboBox_UserType->findText(user[2]));
}

/*********************************************
 *添加新用户
 *********************************************/
void frmconfig::on_btn_addUser_clicked()
{
    QString UserName=ui->txtUserName->text();
    QString UserPwd=ui->txtUserPwd->text();
    QString UserType=ui->comboBox_UserType->currentText();

    if (!this->AddUserIsLegal()){return;}

    QSqlQuery query;
    QString sql="insert into [UserInfo]([UserName],[UserPwd],[UserType])";
    sql+="values('"+UserName+"','";
    sql+=UserPwd+"','";
    sql+=UserType+"')";
    query.exec(sql);
    this->ReadUserInfo();//重新加载用户列表
}

/*********************************************
 *删除用户
 *********************************************/
void frmconfig::on_btn_deleteUser_clicked()
{

    if (ui->txtUserName->text()==""){
        myHelper::ShowMessageBoxError("请选择用户!");
        return ;
    }

    if (ui->txtUserName->text()=="admin"){
        myHelper::ShowMessageBoxError("该用户无权限删除!");
        return ;
    }

    if (myHelper::ShowMessageBoxQuesion("确定要删除该操作员吗?")==0){
        QSqlQuery query;
        QString TempUserName=ui->listUser->currentIndex().data().toString();
        QString sql="delete from [UserInfo] where [UserName]='"+TempUserName+"'";
        query.exec(sql);
        this->ReadUserInfo();//重新加载用户列表
    }
}

/*********************************************
 *修改用户
 *********************************************/
void frmconfig::on_btn_updateUser_clicked()
{

    if (ui->txtUserName->text()==""){
        myHelper::ShowMessageBoxError("请选择用户!");
        return ;
    }

    if (ui->txtUserName->text()=="admin"){
        myHelper::ShowMessageBoxError("该用户无权限删除!");
        return ;
    }
    QString UserName=ui->txtUserName->text();
    QString UserPwd=ui->txtUserPwd->text();
    QString UserType=ui->comboBox_UserType->currentText();
    QString TempUserName=ui->listUser->currentIndex().data().toString();
    QSqlQuery query;
    QString sql="update [UserInfo] set";
    sql+=" [UserName]='"+UserName;
    sql+="',[UserPwd]='"+UserPwd;
    sql+="',[UserType]='"+UserType;
    sql+="' where [UserName]='"+TempUserName+"'";
    query.exec(sql);
    this->ReadUserInfo();
}

/*********************************************
 *添加用户检查合法性
 *********************************************/
bool frmconfig::AddUserIsLegal()
{
    foreach (QString name, TempUserName){
        if (name==ui->txtUserName->text()){
            myHelper::ShowMessageBoxError("该用户已存在,请重新设置!");
            ui->txtUserName->setFocus();
            return false;
        }
    }

    if (ui->txtUserName->text()==""){
        myHelper::ShowMessageBoxError("用户姓名不能为空,请重新设置!");
        ui->txtUserName->setFocus();
        return false;
    }

    if (ui->txtUserPwd->text()==""){
        myHelper::ShowMessageBoxError("用户密码不能为空,请重新设置!");
        ui->txtUserPwd->setFocus();
        return false;
    }

    return true;
}

//*********************************************
// *修改用户密码
// *********************************************/
//void frmconfig::on_btn_updatePwd_clicked()
//{
//    frmPwd *d=new frmPwd;
//    d->exec();
//}
//*************************************用户设置结束*********************************************/

//*************************************因子设置*************************************************/
/*********************************************
 *添加因子
 *********************************************/
void frmconfig::on_btn_addPara_clicked()
{
    if (!AddParaIsLegal())return;
    QString Name=ui->txtparaName->text();
    QString UseChannel=ui->comboBox_channel->currentText();
    QString Protocol=QString::number(ui->comboBox_protocol->currentIndex());
    QString Address=ui->txtAddress->text();
    QString Signal=ui->comboBox_signal->isEnabled()?ui->comboBox_signal->currentText():"";
    QString RangeUp=ui->txtrangeup->text();
    QString RangeLow=ui->txtrangelow->text();
    QString AlarmUp=ui->txtAlarmUp->text();
    QString AlarmLow=ui->txtAlarmLow->text();
    QString MaxFlag=ui->btn_maxOpen->GetCheck()?"true":"false";
    QString MinFlag=ui->btn_minOpen->GetCheck()?"true":"false";
    QString AvgFlag=ui->btn_avgOpen->GetCheck()?"true":"false";
    QString CouFlag=ui->btn_couOpen->GetCheck()?"true":"false";
    QString Decimals=ui->comboBox_decimals->currentText();
    QString Code=ui->txtparaCode->text();
    QString Unit=ui->comboBox_unit->currentText();
    QSqlQuery query;
    QString sql;
    sql="insert into [ParaInfo]([Name],[Code],[Unit],[UseChannel],[Address],[Protocol],[Signal],[RangeUp],[RangeLow],";
    sql+="[AlarmUp],[AlarmLow],[MaxFlag],[MinFlag],[AvgFlag],[CouFlag],[Decimals])values('";
    sql+=Name+"','";
    sql+=Code+"','";
    sql+=Unit+"','";
    sql+=UseChannel+"','";
    sql+=Address+"','";
    sql+=Protocol+"','";
    sql+=Signal+"','";
    sql+=RangeUp+"','";
    sql+=RangeLow+"','";
    sql+=AlarmUp+"','";
    sql+=AlarmLow+"','";
    sql+=MaxFlag+"','";
    sql+=MinFlag+"','";
    sql+=AvgFlag+"','";
    sql+=CouFlag+"','";
    sql+=Decimals+"')";
    query.exec(sql);
    this->ReadParaInfo();//重新加载列表
}

/*********************************************
 *删除因子
 *********************************************/
void frmconfig::on_btn_deletePara_clicked()
{
    int row=ui->tableView_ParaInfo->currentIndex().row();
    QModelIndex index=ui->tableView_ParaInfo->currentIndex();
    QString TempName=index.sibling(row,0).data().toString();
    if (myHelper::ShowMessageBoxQuesion("确定删除吗?")==0){
        QSqlQuery query;
        QString sql="delete from [ParaInfo] where [Name]='"+TempName+"'";
        query.exec(sql);
        this->ReadParaInfo();//重新加载列表
    }
}

/*********************************************
 *添加因子检查合法性
 *********************************************/
bool frmconfig::AddParaIsLegal()
{
    if(ui->txtparaName->text().isEmpty()){
        myHelper::ShowMessageBoxError("因子名称不能为空,请重新设置!");
        return false;
    }

    if(ui->txtparaCode->text().isEmpty()){
        myHelper::ShowMessageBoxError("因子代码不能为空,请重新设置!");
        return false;
    }

    if(ui->txtrangeup->text().isEmpty()){
        myHelper::ShowMessageBoxError("量程上限不能为空,请重新设置!");
        return false;
    }

    if(ui->txtrangelow->text().isEmpty()){
        myHelper::ShowMessageBoxError("量程下限不能为空,请重新设置!");
        return false;
    }

    QSqlQuery query;
    query.exec("select * from [ParaInfo]");
    while(query.next()){

        if(ui->txtparaName->text()==query.value(0).toString()){
            myHelper::ShowMessageBoxError("因子名称已存在,请重新设置!");
            return false;
        }

        if(ui->txtparaCode->text()==query.value(1).toString()){
            myHelper::ShowMessageBoxError("因子代码已存在,请重新设置!");
            return false;
        }

        if(ui->comboBox_channel->currentIndex()<8){
            if(ui->comboBox_channel->currentText()==query.value(3).toString()){
                myHelper::ShowMessageBoxError("通道已被占用,请重新设置!");
                return false;
            }
        }
        if(ui->comboBox_channel->currentText()=="COM3"){
            if(myApp::COM3ToServerOpen){
                myHelper::ShowMessageBoxError("COM3已被用于站点通讯,请重新设置!");
                return false;
            }
        }
    }
    return true;
}

/*********************************************
 *设置污染物信息显示表格模型
 *********************************************/
void frmconfig::InitParaInfoModel()
{
    paraInfoModel = new QStandardItemModel();
    paraInfoModel->setColumnCount(16);
    paraInfoModel->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("监测因子"));
    paraInfoModel->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("代码"));
    paraInfoModel->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("单位"));
    paraInfoModel->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("占用通道"));   
    paraInfoModel->setHeaderData(4,Qt::Horizontal,QString::fromLocal8Bit("通讯地址"));
    paraInfoModel->setHeaderData(5,Qt::Horizontal,QString::fromLocal8Bit("通道协议"));
    paraInfoModel->setHeaderData(6,Qt::Horizontal,QString::fromLocal8Bit("信号范围"));
    paraInfoModel->setHeaderData(7,Qt::Horizontal,QString::fromLocal8Bit("量程上限"));
    paraInfoModel->setHeaderData(8,Qt::Horizontal,QString::fromLocal8Bit("量程下限"));
    paraInfoModel->setHeaderData(9,Qt::Horizontal,QString::fromLocal8Bit("报警上限"));
    paraInfoModel->setHeaderData(10,Qt::Horizontal,QString::fromLocal8Bit("报警下限"));
    paraInfoModel->setHeaderData(11,Qt::Horizontal,QString::fromLocal8Bit("最大值"));
    paraInfoModel->setHeaderData(12,Qt::Horizontal,QString::fromLocal8Bit("最小值"));
    paraInfoModel->setHeaderData(13,Qt::Horizontal,QString::fromLocal8Bit("平均值"));
    paraInfoModel->setHeaderData(14,Qt::Horizontal,QString::fromLocal8Bit("累积值"));
    paraInfoModel->setHeaderData(15,Qt::Horizontal,QString::fromLocal8Bit("小数位"));
    ui->tableView_ParaInfo->setModel(paraInfoModel);
    //表头信息显示居中
    ui->tableView_ParaInfo->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);
    //设置列宽根据内容变化
    for(int i=0;i<paraInfoModel->columnCount();i++){
        ui->tableView_ParaInfo->horizontalHeader()->setResizeMode(i,QHeaderView::Interactive);
        //ui->tableView_ParaInfo->setColumnWidth(i,100);
    }
    //点击表头时不对表头光亮
    ui->tableView_ParaInfo->horizontalHeader()->setHighlightSections(false);
    //选中模式为单行选中
    ui->tableView_ParaInfo->setSelectionMode(QAbstractItemView::SingleSelection);
    //选中整行
    ui->tableView_ParaInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_ParaInfo->setStyleSheet( "QTableView::item:hover{background-color:rgb(0,200,255,255)}"
                                                                     "QTableView::item:selected{background-color:#0000FF}");
    //设置非编辑状态
//    ui->tableView_ParaInfo->setEditTriggers(QTableView::NoEditTriggers);
    //设置表头背景色
    ui->tableView_ParaInfo->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue}");
    ui->tableView_ParaInfo->verticalHeader()->setStyleSheet("QHeaderView::section{background:skyblue}");
}

/*********************************************
 *读取监测污染物信息
 *********************************************/
void frmconfig::ReadParaInfo()
{
    paraInfoModel->removeRows(0,paraInfoModel->rowCount());
    QSqlQuery query;
    query.exec("select  *  from [ParaInfo]");
    int rows=0;
    while(query.next())
    {
        for(int i=0;i<paraInfoModel->columnCount();i++){
            paraInfoModel->setItem(rows,i,new QStandardItem(query.value(i).toString()));
//            paraInfoModel->item(rows,i)->setForeground(QBrush(QColor(0, 0, 0)));    //设置字符颜色
            paraInfoModel->item(rows,i)->setTextAlignment(Qt::AlignCenter);           //设置字符位置
            if(rows%2){
                paraInfoModel->item(rows,i)->setBackground(QBrush(QColor(0,255,0,80)));
            }
            }
        rows++;

    }

}

void frmconfig::on_comboBox_channel_currentIndexChanged(int index)
{
    if(index<=7){
        ui->comboBox_signal->setEnabled(true);
    }else{
        ui->comboBox_signal->setEnabled(false);
    }
}
//*************************************因子设置结束*********************************************/

void frmconfig::on_btn_Cancel_clicked()
{
    this->close();
}

//时间日期设置
void frmconfig::on_btnChange_clicked()
{
    if (myHelper::ShowMessageBoxQuesion("确定要更改日期时间吗?")==0){
        QString TempMsg;
        #ifdef Q_OS_LINUX
        QString str=QString("/bin/date -s '%1-%2-%3 %4:%5:%6'") //  生成时间设置命令字符串
                            .arg(ui->comboBox_year->currentText())
                            .arg(ui->comboBox_month->currentText())
                            .arg(ui->comboBox_day->currentText())
                            .arg(ui->comboBox_hour->currentText())
                            .arg(ui->comboBox_minute->currentText())
                            .arg(ui->comboBox_second->currentText());

        int res1;
        int res2;
        res1=system(str.toLatin1().data());  //设置系统时间
        res2=system("/sbin/hwclock --systohc");   //将系统时间写入到RTC硬件中，以保留设置。这一操作是为了将修改好的时间写入到RTC中保存。如果不进行这一步操作，则
                                                                          //重新上电开机以后系统从RTC中读取到的仍然是原来的时间
        if(res1==0 && res2==0){
            TempMsg="本地更改日期时间成功";
        }
        else{
            TempMsg="本地更改日期时间失败";
        }
        #elif defined (Q_OS_WIN)

        #endif
        myAPI api;
        api.AddEventInfoUser(TempMsg);
        myHelper::ShowMessageBoxInfo(QString("%1!").arg(TempMsg));

        }
}

void frmconfig::on_btn_SaveGeneral_clicked()
{
    myApp::MN=ui->txtMN->text();
    myApp::RtdInterval=ui->txtRtdInterval->text().toInt();
//    myApp::AlarmTarget=ui->txtAlarmTarget->text();
    myApp::SampleIntervalTime=ui->txtSampleIntervalTime->text().toInt();
//    myApp::CatchmentTime=ui->txtCatchmentTime->text().toInt();
//    myApp::DrainTime=ui->txtDrainTime->text().toInt();
    myApp::MinInterval=ui->txtMinInterval->text().toInt();
//    myApp::PW=ui->txtPW->text();
    myApp::OverTime=ui->txtOverTime->text().toInt();
    myApp::ReCount=ui->txtReCount->text().toInt();
    myApp::StType=ui->comboBoxStType->currentIndex();
    myApp::RespondOpen=ui->btn_RespondOpen->GetCheck();
    //调用保存配置文件函数
    myApp::WriteConfig();
}

void frmconfig::on_btn_SaveSerial_clicked()
{
    switch (ui->comboBox_serial->currentIndex())
    {
        case 0:
            myApp::ConfigUart1=QString("%1|%2|%3|%4|%5|%6")
                    .arg(ui->comboBox_baudrate->currentText())
                    .arg(ui->comboBox_databits->currentText())
                    .arg(ui->comboBox_parity->currentIndex())
                    .arg(ui->comboBox_stopbits->currentIndex())
                    .arg(ui->txtuartTimeout->text().toInt())
                    .arg(ui->txtuartInterval->text().toInt());
            break;
        case 1:
            myApp::ConfigUart2=QString("%1|%2|%3|%4|%5|%6")
                    .arg(ui->comboBox_baudrate->currentText())
                    .arg(ui->comboBox_databits->currentText())
                    .arg(ui->comboBox_parity->currentIndex())
                    .arg(ui->comboBox_stopbits->currentIndex())
                    .arg(ui->txtuartTimeout->text().toInt())
                    .arg(ui->txtuartInterval->text().toInt());
            break;
        case 2:
            myApp::ConfigUart3=QString("%1|%2|%3|%4|%5|%6")
                    .arg(ui->comboBox_baudrate->currentText())
                    .arg(ui->comboBox_databits->currentText())
                    .arg(ui->comboBox_parity->currentIndex())
                    .arg(ui->comboBox_stopbits->currentIndex())
                    .arg(ui->txtuartTimeout->text().toInt())
                    .arg(ui->txtuartInterval->text().toInt());
            break;
        case 3:
            myApp::ConfigUart4=QString("%1|%2|%3|%4|%5|%6")
                    .arg(ui->comboBox_baudrate->currentText())
                    .arg(ui->comboBox_databits->currentText())
                    .arg(ui->comboBox_parity->currentIndex())
                    .arg(ui->comboBox_stopbits->currentIndex())
                    .arg(ui->txtuartTimeout->text().toInt())
                    .arg(ui->txtuartInterval->text().toInt());
            break;
        case 4:
            myApp::ConfigUart5=QString("%1|%2|%3|%4|%5|%6")
                    .arg(ui->comboBox_baudrate->currentText())
                    .arg(ui->comboBox_databits->currentText())
                    .arg(ui->comboBox_parity->currentIndex())
                    .arg(ui->comboBox_stopbits->currentIndex())
                    .arg(ui->txtuartTimeout->text().toInt())
                    .arg(ui->txtuartInterval->text().toInt());
            break;
        case 5:
            myApp::ConfigUart6=QString("%1|%2|%3|%4|%5|%6")
                    .arg(ui->comboBox_baudrate->currentText())
                    .arg(ui->comboBox_databits->currentText())
                    .arg(ui->comboBox_parity->currentIndex())
                    .arg(ui->comboBox_stopbits->currentIndex())
                    .arg(ui->txtuartTimeout->text().toInt())
                    .arg(ui->txtuartInterval->text().toInt());
            break;

    }
    //调用保存配置文件函数
    myApp::WriteConfig();

}

void frmconfig::on_btn_SaveSite_clicked()
{
    QString IP1=ui->txtIP1->text();
    if (!myHelper::IsIP(IP1)){
        myHelper::ShowMessageBoxError("IP1地址有误,请重新输入!");
        ui->txtIP1->setFocus();
        return;
    }
    myApp::ServerAddr1=QString("%1|%2|%3")
            .arg(ui->btn_server1Open->GetCheck()==true?"true":"false")
            .arg(IP1)
            .arg(ui->txtPort1->text());

    QString IP2=ui->txtIP2->text();
    if (!myHelper::IsIP(IP2)){
        myHelper::ShowMessageBoxError("IP2地址有误,请重新输入!");
        ui->txtIP2->setFocus();
        return;
    }
    myApp::ServerAddr2=QString("%1|%2|%3")
            .arg(ui->btn_server2Open->GetCheck()==true?"true":"false")
            .arg(IP2)
            .arg(ui->txtPort2->text());

    QString IP3=ui->txtIP3->text();
    if (!myHelper::IsIP(IP3)){
        myHelper::ShowMessageBoxError("IP3地址有误,请重新输入!");
        ui->txtIP3->setFocus();
        return;
    }
    myApp::ServerAddr3=QString("%1|%2|%3")
            .arg(ui->btn_server3Open->GetCheck()==true?"true":"false")
            .arg(IP3)
            .arg(ui->txtPort3->text());

    QString IP4=ui->txtIP4->text();
    if (!myHelper::IsIP(IP4)){
        myHelper::ShowMessageBoxError("IP4地址有误,请重新输入!");
        ui->txtIP4->setFocus();
        return;
    }

    myApp::ServerAddr4=QString("%1|%2|%3")
            .arg(ui->btn_server4Open->GetCheck()==true?"true":"false")
            .arg(IP4)
            .arg(ui->txtPort4->text());
    if(ui->btn_autoUpdateOpen->GetCheck()==true){
        myApp::AutoUpdateIP=QString("%1|%2")
        .arg(ui->btn_autoUpdateOpen->GetCheck()==true?"true":"false")
        .arg(ui->txt_updateIP->text());
    }
    myApp::COM3ToServerOpen_Config=ui->btn_com3ToServerOpen->GetCheck();
    //调用保存配置文件函数
    myApp::WriteConfig();
}

//*************************************本机网络*************************************************/
void frmconfig::on_btn_SaveLocalNet_clicked()
{

    myApp::DHCP=QString("%1").arg(ui->btn_Autogetip->GetCheck()==true?"1":"0");
    myApp::LocalIP=ui->txtLocalIP->text();
    myApp::Mask=ui->txtMask->text();
    myApp::GateWay=ui->txtGateWay->text();
//    myApp::WriteConfig();
    //调用保存配置文件函数
    myApp::WriteLocalNet();
}
//*************************************本机网络结束*************************************************/

void frmconfig::on_btn_Autogetip_clicked()
{
    if(ui->btn_Autogetip->GetCheck()==true)
    {
        ui->txtLocalIP->setEnabled(false);
        ui->txtMask->setEnabled(false);
        ui->txtGateWay->setEnabled(false);
    }
    else {
        ui->txtLocalIP->setEnabled(true);
        ui->txtMask->setEnabled(true);
        ui->txtGateWay->setEnabled(true);
    }

}

//*************************************开关量配置结束*************************************************/
void frmconfig::on_btn_SaveIo_clicked()
{
    //开排水口
    switch (ui->comboOut_drain_open->currentIndex()){
    case 0: myApp::Out_drain_open=6;
    break;
    case 1: myApp::Out_drain_open=7;
    break;
    case 2: myApp::Out_drain_open=8;
    break;
    case 3: myApp::Out_drain_open=9;
    break;
    case 4: myApp::Out_drain_open=16;
    break;
    case 5: myApp::Out_drain_open=17;
    break;
    case 6: myApp::Out_drain_open=24;
    break;
    case 7: myApp::Out_drain_open=25;
    break;
    default:break;
    }
    //关排水口
    switch (ui->comboOut_drain_close->currentIndex()){
    case 0: myApp::Out_drain_close=6;
    break;
    case 1: myApp::Out_drain_close=7;
    break;
    case 2: myApp::Out_drain_close=8;
    break;
    case 3: myApp::Out_drain_close=9;
    break;
    case 4: myApp::Out_drain_close=16;
    break;
    case 5: myApp::Out_drain_close=17;
    break;
    case 6: myApp::Out_drain_close=24;
    break;
    case 7: myApp::Out_drain_close=25;
    break;
    default:break;
    }
    //开集水池
    switch (ui->comboOut_catchment_open->currentIndex()){
    case 0: myApp::Out_catchment_open=6;
    break;
    case 1: myApp::Out_catchment_open=7;
    break;
    case 2: myApp::Out_catchment_open=8;
    break;
    case 3: myApp::Out_catchment_open=9;
    break;
    case 4: myApp::Out_catchment_open=16;
    break;
    case 5: myApp::Out_catchment_open=17;
    break;
    case 6: myApp::Out_catchment_open=24;
    break;
    case 7: myApp::Out_catchment_open=25;
    break;
    default:break;
    }
    //市电检测
    switch (ui->comboIn_power->currentIndex()){
    case 0: myApp::In_power=10;
    break;
    case 1: myApp::In_power=11;
    break;
    case 2: myApp::In_power=12;
    break;
    case 3: myApp::In_power=13;
    break;
    case 4: myApp::In_power=14;
    break;
    case 5: myApp::In_power=15;
    break;
    case 6: myApp::In_power=18;
    break;
    case 7: myApp::In_power=19;
    break;
    case 8: myApp::In_power=20;
    break;
    case 9: myApp::In_power=21;
    break;
    case 10: myApp::In_power=22;
    break;
    case 11: myApp::In_power=23;
    break;
    default:break;
    }
    myApp::WriteIoConfig();


}



