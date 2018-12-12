#include "frmmain.h"
#include "ui_frmmain.h"
#include <QDateTime>
#include <QTableView>
#include "api/myapi.h"
#include "api/myapp.h"
#include "api/myhelper.h"
#include "gpio.h"
#include "api/spi_drivers.h"
#include "api/valve_control.h"
#include "frmconfig.h"
#include "frmdata.h"
#include "frmlogin.h"
#include "frmevent.h"
#include "frmsampler.h"
#include "frmvalve.h"
#include "frmdiagnose.h"
#include "frmcod.h"
#include <sys/socket.h>
#include <QProcess>
#include <netinet/tcp.h>
#ifdef Q_OS_LINUX
#include <sys/reboot.h>
#endif


extern Com_para COM[6];
extern Tcp_para TCP[4];
extern QextSerialPort *myCom[6];
extern QStandardItemModel  *model_rtd;

#include<stdio.h>
QTcpSocket *tcpSocket1=NULL;
QTcpSocket *tcpSocket2=NULL;
QTcpSocket *tcpSocket3=NULL;
QTcpSocket *tcpSocket4=NULL;
QTimer *tcpSocketTimer1=NULL;
QTimer *tcpSocketTimer2=NULL;
QTimer *tcpSocketTimer3=NULL;
QTimer *tcpSocketTimer4=NULL;

frmMain::frmMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->InitStyle();                  //初始化窗口作用
    myHelper::FormInCenter(this,myApp::DeskWidth,myApp::DeskHeigth);//窗体居中显示
//初始化硬件接口以及界面
    InitGPIO();
    this->InitForm();
    this->InitTcpSocketClient();
    this->InitRtdTable();
    QSqlQuery query;
    query.exec("select [UseChannel] from [ParaInfo]  where [UseChannel] like 'COM%'");
    while(query.next())
    {
        switch(query.value(0).toString().right(1).toInt())
        {
            case 2:
            if(u1.isRunning()==false){
            this->InitCOM(0);
            myHelper::Sleep(1);
            u1.start();
            }
            break;
            case 3:
            if(u2.isRunning()==false){
            this->InitCOM(1);
            myHelper::Sleep(1);
            u2.start();
            }
            break;
            case 4:
            if(u3.isRunning()==false){
            this->InitCOM(2);
            myHelper::Sleep(1);
            u3.start();
            }
            break;
            case 5:
            if(u4.isRunning()==false){
            this->InitCOM(3);
            myHelper::Sleep(1);
            u4.start();
            }
            break;
            case 6:
            if(u5.isRunning()==false){
            this->InitCOM(4);
            myHelper::Sleep(1);
            u5.start();
            }
            break;
            case 7:
            if(u6.isRunning()==false){
            this->InitCOM(5);
            myHelper::Sleep(1);
            u6.start();
            }
            break;
            default:   break;
        }
    }
    if(myApp::COM3ToServerOpen)
    {
        if(u2.isRunning()==false){
        this->InitCOM(1);
        myHelper::Sleep(1);
        u2.start();
        }
    }
//TEST
    rtdProc.start();
    count.start();
    Msend.start();
    db_clear.start();
    read_ad.start();
    flow.start();
}

frmMain::~frmMain()
{
    rtdProc.quit();
    count.quit();
    Msend.quit();
    db_clear.quit();
    read_ad.quit();
//    delete myCom[6];
    delete ui;
}

void frmMain::InitStyle()
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);   //没有窗体边框
}

void frmMain::InitForm()
{
    labWelcom=new QLabel(QString("%1【%2】 ").arg(myApp::SoftTitle).arg(myApp::SoftVersion));//显示版本信息
    labWelcom->setAttribute(Qt::WA_DeleteOnClose);
    ui->statusbar->addWidget(labWelcom);

    myApp::LocalIP=QNetworkInterface().allAddresses().at(1).toString();
    labIP=new QLabel(QString("本机IP:%1 ").arg(myApp::LocalIP));//显示本机IP信息
    labIP->setAttribute(Qt::WA_DeleteOnClose);
    ui->statusbar->addWidget(labIP);


    labUser=new QLabel(QString("未登录"));
    labUser->setAttribute(Qt::WA_DeleteOnClose);
    ui->statusbar->addWidget(labUser);

    labTime=new QLabel(QDateTime::currentDateTime().toString("yyyy年MM月dd日 HH:mm:ss"));
    labTime->setAttribute(Qt::WA_DeleteOnClose);
    ui->statusbar->addWidget(labTime);

    ui->statusbar->setStyleSheet(QString("QStatusBar::item{border:0px}"));//去掉label的边框.

    timerDate=new QTimer(this);
    timerDate->setInterval(1000);
    connect(timerDate,SIGNAL(timeout()),this,SLOT(ShowDateTime())); //刷新当前时间
    timerDate->start();

    timerSample=new QTimer(this);
    timerSample->setInterval(8000);
    connect(timerSample,SIGNAL(timeout()),this,SLOT(CheckSampleCmd()));// 检测物理网络链接
    timerSample->start();

    statusTimer=new QTimer(this);
    statusTimer->setInterval(5000);
    connect(statusTimer,SIGNAL(timeout()),this,SLOT(Status())); //检测状态信号
    statusTimer->start();

//    connect(&message_api,SIGNAL(flow_trigger()),this,SLOT(get_flow_trigger()));
    connect(ui->action_Setting,SIGNAL(triggered()),this,SLOT(ShowForm()));
    connect(ui->action_HistoryData,SIGNAL(triggered()),this,SLOT(ShowForm()));
    connect(ui->action_Event,SIGNAL(triggered()),this,SLOT(ShowForm()));
    connect(ui->action_Sampler,SIGNAL(triggered()),this,SLOT(ShowForm()));
    connect(ui->action_Valve,SIGNAL(triggered()),this,SLOT(ShowForm()));
    connect(ui->action_COD,SIGNAL(triggered()),this,SLOT(ShowForm()));
    connect(ui->action_User,SIGNAL(triggered()),this,SLOT(ShowForm()));
    connect(ui->action_Reset,SIGNAL(triggered()),this,SLOT(ShowForm()));
    connect(ui->action_Import,SIGNAL(triggered()),this,SLOT(ShowForm()));
    connect(ui->action_Export,SIGNAL(triggered()),this,SLOT(ShowForm()));
    connect(ui->action_Diagnose,SIGNAL(triggered()),this,SLOT(ShowForm()));

}

//显示界面
void frmMain::ShowForm()
{
    QAction *action=(QAction *)sender();
    QString triggerName=action->objectName();

    if (triggerName=="action_Setting"){            //系统设置
        if(myApp::Login){
            frmconfig *d=new frmconfig(this);
            d->showFullScreen();
        }else {
            myHelper::ShowMessageBoxInfo("请登录!");
        }
    }

    if (triggerName=="action_HistoryData"){     //历史数据
            frmdata *d=new frmdata(this);
            d->showFullScreen();
    }

    if (triggerName=="action_Event"){              //系统日志
            frmevent *d=new frmevent(this);
            d->showFullScreen();
    }

    if (triggerName=="action_Valve"){              //阀门控制
        if(myApp::Login){
            frmValve *d=new frmValve(this);
            d->showFullScreen();
        }else {
            myHelper::ShowMessageBoxInfo("请登录!");
        }
    }



    if (triggerName=="action_Sampler"){         //采样器控制
        if(myApp::Login){
            myApp::SampleMode=1;        //手动模式
            frmSampler *d=new frmSampler(this);
            d->showFullScreen();
        }else{
            myHelper::ShowMessageBoxInfo("请登录!");
        }
    }

    if (triggerName=="action_COD"){         //COD控制
        if(myApp::Login){
            frmcod *d=new frmcod(this);
            d->showFullScreen();
        }else{
            myHelper::ShowMessageBoxInfo("请登录!");
        }
    }


    if (triggerName=="action_User"){              //用户登录
        if(!myApp::Login){//未登录
            frmlogin *d=new frmlogin(this);
            d->setGeometry(QRect(160,100,480,259));
            d->exec();
            if(myApp::Login){
                ui->action_User->setText(tr("用户退出"));
                labUser->setText(QString("当前用户:%1【%2】").arg(myApp::CurrentUserName).arg(myApp::CurrentUserType));
                api.AddEventInfoUser("用户登录");
            }
            else{
                labUser->setText(QString("未登录"));
            }
        }
        else {
            myApp::Login=false;
            ui->action_User->setText(tr("用户登录"));
            labUser->setText(QString("未登录"));
            api.AddEventInfoUser("用户退出");
        }

    }

    if (triggerName=="action_Reset"){              //系统重启
        if(myHelper::ShowMessageBoxQuesion("确认系统重启？")==false){
        #ifdef Q_OS_LINUX
        system("reboot -n");
//            reboot(RB_AUTOBOOT);
        #elif defined (Q_OS_WIN)
            qApp->quit();
            QProcess::startDetached(qApp->applicationFilePath(), QStringList());
        #endif
        }
    }

    if (triggerName=="action_Export"){              //数据导出
        if(myHelper::ShowMessageBoxQuesion("确认导出吗？")==false){

        }
    }

    if (triggerName=="action_Import"){              //数据导入
        if(myHelper::ShowMessageBoxQuesion("确认导入吗？")==false){

        }
    }

    if (triggerName=="action_Diagnose"){              //在线诊断
        frmdiagnose *d=new frmdiagnose(this);
        d->showFullScreen();

    }

}

//设置实时数据显示表格模型
void frmMain::InitRtdTable()
{
    model_rtd = new QStandardItemModel();
    model_rtd->setColumnCount(4);
//    model_rtd->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("采集时间"));
    model_rtd->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("监测因子"));
    model_rtd->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("实时值"));
    model_rtd->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("累计值"));
    model_rtd->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("实时标记"));

//    columnWidths[0]=180;
    columnWidths[0]=200;
    columnWidths[1]=200;
    columnWidths[2]=200;
    columnWidths[3]=180;
    ui->tableView->setModel(model_rtd);
    //表头信息显示居中
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);
    //设置列宽根据内容变化
    for(int i=0;i<model_rtd->columnCount();i++){
        ui->tableView->horizontalHeader()->setResizeMode(i,QHeaderView::Interactive);
        ui->tableView->setColumnWidth(i,columnWidths[i]);
    }
    //点击表头时不对表头光亮
    ui->tableView->horizontalHeader()->setHighlightSections(false);
    //选中模式为单行选中
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    //选中整行
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setStyleSheet( "QTableView::item:hover{background-color:rgb(0,200,255,255)}"
                                                             "QTableView::item:selected{background-color:#0000AA}");
    //设置非编辑状态
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    //设置表头背景色
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue}");
    ui->tableView->verticalHeader()->setStyleSheet("QHeaderView::section{background:skyblue}");

}

//显示当前运行时间
void frmMain::ShowDateTime()
{
    labTime->setText(QDateTime::currentDateTime().toString("yyyy年MM月dd日 HH:mm:ss"));
}

bool frmMain::CheckSampleCmd()
{

///////////定时检测物理网络是否断开///////////
//    FILE * fp;

//    char buffer[100]={'0'};   //加判断
//    fp = popen("ifconfig eth0 | grep RUNNING", "r");
//    fgets(buffer, sizeof(buffer), fp);
//    //qDebug()<<QString(buffer);
//    pclose(fp);

    QFile etho_status("/sys/class/net/eth0/carrier");
    if(!etho_status.open(QIODevice::ReadOnly)){
        //qDebug()<<"eth0 carrier file is lose";
        return false;
    }
    QString strList;
    QTextStream stream(&etho_status);
    strList=stream.readAll();
    //qDebug()<<strList;
    etho_status.close();


    if(strList.startsWith("0"))
    {
        if(TCP[0].isConnected==true)
        {
            TCP[0].isConnected=false;
            //qDebug()<<QString("%1:%2 disconnected.").arg(TCP[0].ServerIP).arg(TCP[0].ServerPort);
        }


        if(TCP[1].isConnected==true)
        {
            TCP[1].isConnected=false;
            //qDebug()<<QString("%1:%2 disconnected.").arg(TCP[1].ServerIP).arg(TCP[1].ServerPort);
        }
        if(TCP[2].isConnected==true)
        {
            TCP[2].isConnected=false;
            //qDebug()<<QString("%1:%2 disconnected.").arg(TCP[2].ServerIP).arg(TCP[2].ServerPort);
        }
        if(TCP[3].isConnected==true)
        {
            TCP[3].isConnected=false;
            //qDebug()<<QString("%1:%2 disconnected.").arg(TCP[3].ServerIP).arg(TCP[3].ServerPort);
        }

    }
    return true;
}



//tcpsocket初始化
void frmMain::InitTcpSocketClient()
{
    if(TCP[0].ServerOpen){

        tcpSocket1 = new QTcpSocket(this);
        connect(tcpSocket1, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
        connect(tcpSocket1, SIGNAL(connected()), this, SLOT(updateclient()));
        connect(tcpSocket1, SIGNAL(readyRead()), this, SLOT(clientReadMessage()));
//        connect(tcpSocket1,SIGNAL(disconnected()),this,SLOT(updateClientStatusDisconnect()));
        tcpSocketTimer1=new QTimer(this);
        tcpSocketTimer1->setInterval(10000);
        connect(tcpSocketTimer1,SIGNAL(timeout()),this,SLOT(ConnectServer1()));
        tcpSocketTimer1->start();
    }

    if(TCP[1].ServerOpen){

        tcpSocket2 = new QTcpSocket(this);
//        ConnectServer();
        connect(tcpSocket2, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
        connect(tcpSocket2, SIGNAL(connected()), this, SLOT(updateclient()));
        connect(tcpSocket2, SIGNAL(readyRead()), this, SLOT(clientReadMessage()));
//        connect(tcpSocket2,SIGNAL(disconnected()),this,SLOT(updateClientStatusDisconnect()));

        tcpSocketTimer2=new QTimer(this);
        tcpSocketTimer2->setInterval(10000);
        connect(tcpSocketTimer2,SIGNAL(timeout()),this,SLOT(ConnectServer2()));
        tcpSocketTimer2->start();
    }

    if(TCP[2].ServerOpen){

        tcpSocket3 = new QTcpSocket(this);
//        ConnectServer();
        connect(tcpSocket3, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
        connect(tcpSocket3, SIGNAL(connected()), this, SLOT(updateclient()));
        connect(tcpSocket3, SIGNAL(readyRead()), this, SLOT(clientReadMessage()));
//        connect(tcpSocket3,SIGNAL(disconnected()),this,SLOT(updateClientStatusDisconnect()));

        tcpSocketTimer3=new QTimer(this);
        tcpSocketTimer3->setInterval(10000);
        connect(tcpSocketTimer3,SIGNAL(timeout()),this,SLOT(ConnectServer3()));
        tcpSocketTimer3->start();
    }

    if(TCP[3].ServerOpen){

        tcpSocket4 = new QTcpSocket(this);
//        ConnectServer();
        connect(tcpSocket4, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
        connect(tcpSocket4, SIGNAL(connected()), this, SLOT(updateclient()));
        connect(tcpSocket4, SIGNAL(readyRead()), this, SLOT(clientReadMessage()));
//        connect(tcpSocket4,SIGNAL(disconnected()),this,SLOT(updateClientStatusDisconnect()));

        tcpSocketTimer4=new QTimer(this);
        tcpSocketTimer4->setInterval(10000);
        connect(tcpSocketTimer4,SIGNAL(timeout()),this,SLOT(ConnectServer4()));
        tcpSocketTimer4->start();

    }
}

//客户端无连接
void frmMain::displayError(QAbstractSocket::SocketError)
{
    if(tcpSocket1!=NULL){
        if(tcpSocket1==dynamic_cast<QTcpSocket *>(sender()))
        {
    //        tcpSocket1->close();
            TCP[0].isConnected=false;
            //qDebug()<<QString("%1:%2 disconnected.").arg(TCP[0].ServerIP).arg(TCP[0].ServerPort);
        }
    }
    if(tcpSocket2!=NULL){
        if(tcpSocket2==dynamic_cast<QTcpSocket *>(sender()))
        {
    //        tcpSocket2->close();
            TCP[1].isConnected=false;
            //qDebug()<<QString("%1:%2 disconnected.").arg(TCP[1].ServerIP).arg(TCP[1].ServerPort);

        }
    }
    if(tcpSocket3!=NULL){
        if(tcpSocket3==dynamic_cast<QTcpSocket *>(sender()))
        {
    //        tcpSocket3->close();
            TCP[2].isConnected=false;
            //qDebug()<<QString("%1:%2 disconnected.").arg(TCP[2].ServerIP).arg(TCP[2].ServerPort);

        }
    }
    if(tcpSocket4!=NULL){
        if(tcpSocket4==dynamic_cast<QTcpSocket *>(sender()))
        {
    //        tcpSocket4->close();
            TCP[3].isConnected=false;
            //qDebug()<<QString("%1:%2 disconnected.").arg(TCP[3].ServerIP).arg(TCP[3].ServerPort);
        }
    }

}

//客户端读取消息
void frmMain::clientReadMessage()
{
    QByteArray str;
    if(tcpSocket1!=NULL){
    if(tcpSocket1==dynamic_cast<QTcpSocket *>(sender()))
    {
        str=tcpSocket1->readAll();
        message_api.messageProc(str,NULL,tcpSocket1);
        //qDebug()<<QString("received from socket1:%1").arg(str.data());
    }
    }
    if(tcpSocket2!=NULL){
    if(tcpSocket2==dynamic_cast<QTcpSocket *>(sender()))
    {
        str=tcpSocket2->readAll();
        message_api.messageProc(str,NULL,tcpSocket2);
        //qDebug()<<QString("received from socket2:%1").arg(str.data());
    }
    }
    if(tcpSocket3!=NULL){
    if(tcpSocket3==dynamic_cast<QTcpSocket *>(sender()))
    {
        str=tcpSocket3->readAll();
        message_api.messageProc(str,NULL,tcpSocket3);
        //qDebug()<<QString("received from socket3:%1").arg(str.data());
    }
    }
    if(tcpSocket4!=NULL){
    if(tcpSocket4==dynamic_cast<QTcpSocket *>(sender()))
    {
        str=tcpSocket4->readAll();
        message_api.messageProc(str,NULL,tcpSocket4);
        //qDebug()<<QString("received from socket4:%1").arg(str.data());
    }
    }

}

//已连接上服务器
void frmMain::updateclient()
{
    int keepAlive = 1; // 开启keepalive属性
    int keepIdle = 30; // 如该连接在30秒内没有任何数据往来,则进行探测
    int keepInterval = 10; // 探测时发包的时间间隔为5秒
    int keepCount = 3; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.
    if(tcpSocket1!=NULL){
    if(tcpSocket1==dynamic_cast<QTcpSocket *>(sender()))
    {
        TCP[0].isConnected=true;
        //qDebug()<<QString("%1:%2 connected.").arg(TCP[0].ServerIP).arg(TCP[0].ServerPort);
        int fd_1;
        fd_1=tcpSocket1->socketDescriptor();
        if(setsockopt(fd_1, SOL_SOCKET,SO_KEEPALIVE,&keepAlive,sizeof(keepAlive))!=0)
        {
            //qDebug()<<QString("keepalive error");
        }

        if(setsockopt(fd_1, SOL_TCP , TCP_KEEPIDLE, &keepIdle, sizeof(keepIdle))!=0)
        {
            //qDebug()<<QString("TCP_KEEPIDLE error");
        }

        if(setsockopt(fd_1, SOL_TCP , TCP_KEEPINTVL, &keepInterval, sizeof(keepInterval))!=0)
        {
            //qDebug()<<QString("TCP_KEEPINTVL error");
        }

        if(setsockopt(fd_1, SOL_TCP , TCP_KEEPCNT, &keepCount, sizeof(keepCount))!=0)
        {
            //qDebug()<<QString("TCP_KEEPCNT error");
        }

    }
    }
    if(tcpSocket2!=NULL){
    if(tcpSocket2==dynamic_cast<QTcpSocket *>(sender()))
    {
        TCP[1].isConnected=true;
        //qDebug()<<QString("%1:%2 connected.").arg(TCP[1].ServerIP).arg(TCP[1].ServerPort);
        int fd_2;
        fd_2=tcpSocket2->socketDescriptor();
        if(setsockopt(fd_2, SOL_SOCKET,SO_KEEPALIVE,&keepAlive,sizeof(keepAlive))!=0)
        {
            //qDebug()<<QString("keepalive error");

        }

        if(setsockopt(fd_2, SOL_TCP , TCP_KEEPIDLE, &keepIdle, sizeof(keepIdle))!=0)
        {
            //qDebug()<<QString("TCP_KEEPIDLE error");
        }

        if(setsockopt(fd_2, SOL_TCP , TCP_KEEPINTVL, &keepInterval, sizeof(keepInterval))!=0)
        {
            //qDebug()<<QString("TCP_KEEPINTVL error");
        }

        if(setsockopt(fd_2, SOL_TCP , TCP_KEEPCNT, &keepCount, sizeof(keepCount))!=0)
        {
            //qDebug()<<QString("TCP_KEEPCNT error");
        }

    }
    }
    if(tcpSocket3!=NULL){
    if(tcpSocket3==dynamic_cast<QTcpSocket *>(sender()))
    {
        TCP[2].isConnected=true;
        //qDebug()<<QString("%1:%2 connected.").arg(TCP[2].ServerIP).arg(TCP[2].ServerPort);
        int fd_3;
        fd_3=tcpSocket3->socketDescriptor();
        if(setsockopt(fd_3, SOL_SOCKET,SO_KEEPALIVE,&keepAlive,sizeof(keepAlive))!=0)
        {
            //qDebug()<<QString("keepalive error");

        }

        if(setsockopt(fd_3, SOL_TCP , TCP_KEEPIDLE, &keepIdle, sizeof(keepIdle))!=0)
        {
            //qDebug()<<QString("TCP_KEEPIDLE error");
        }

        if(setsockopt(fd_3, SOL_TCP , TCP_KEEPINTVL, &keepInterval, sizeof(keepInterval))!=0)
        {
            //qDebug()<<QString("TCP_KEEPINTVL error");
        }

        if(setsockopt(fd_3, SOL_TCP , TCP_KEEPCNT, &keepCount, sizeof(keepCount))!=0)
        {
            //qDebug()<<QString("TCP_KEEPCNT error");
        }

    }
    }
    if(tcpSocket4!=NULL){
    if(tcpSocket4==dynamic_cast<QTcpSocket *>(sender()))
    {
        TCP[3].isConnected=true;
        //qDebug()<<QString("%1:%2 connected.").arg(TCP[3].ServerIP).arg(TCP[3].ServerPort);
        int fd_4;
        fd_4=tcpSocket4->socketDescriptor();
        if(setsockopt(fd_4, SOL_SOCKET,SO_KEEPALIVE,&keepAlive,sizeof(keepAlive))!=0)
        {
            //qDebug()<<QString("keepalive error");

        }

        if(setsockopt(fd_4, SOL_TCP , TCP_KEEPIDLE, &keepIdle, sizeof(keepIdle))!=0)
        {
            //qDebug()<<QString("TCP_KEEPIDLE error");
        }

        if(setsockopt(fd_4, SOL_TCP , TCP_KEEPINTVL, &keepInterval, sizeof(keepInterval))!=0)
        {
            //qDebug()<<QString("TCP_KEEPINTVL error");
        }

        if(setsockopt(fd_4, SOL_TCP , TCP_KEEPCNT, &keepCount, sizeof(keepCount))!=0)
        {
            //qDebug()<<QString("TCP_KEEPCNT error");
        }
    }
    }
}


void frmMain::ConnectServer1()
{
    if(tcpSocketTimer1!=NULL){
        if(tcpSocketTimer1==dynamic_cast<QTimer *>(sender()))
        {

            if(TCP[0].isConnected==false&&(tcpSocket1->state()!=tcpSocket1->ConnectingState))
    //        if(TCP[0].isConnected==false)
            {
                tcpSocket1->close();
                //qDebug()<<QString("start connect to %1:%2").arg(TCP[0].ServerIP).arg(TCP[0].ServerPort);
                tcpSocket1->connectToHost(TCP[0].ServerIP,TCP[0].ServerPort.toInt());
            }
        }
    }
}

void frmMain::ConnectServer2()
{
    if(tcpSocketTimer2!=NULL){
    if(tcpSocketTimer2==dynamic_cast<QTimer *>(sender()))
    {
        if(TCP[1].isConnected==false&&(tcpSocket2->state()!=tcpSocket2->ConnectingState))
//        if(TCP[1].isConnected==false)
        {
            tcpSocket2->close();
            //qDebug()<<QString("start connect to %1:%2").arg(TCP[1].ServerIP).arg(TCP[1].ServerPort);
            tcpSocket2->connectToHost(TCP[1].ServerIP,TCP[1].ServerPort.toInt());
        }
    }
    }

}

void frmMain::ConnectServer3()
{
    if(tcpSocketTimer3!=NULL){
    if(tcpSocketTimer3==dynamic_cast<QTimer *>(sender()))
    {
        if(TCP[2].isConnected==false&&(tcpSocket3->state()!=tcpSocket3->ConnectingState))
        {
            tcpSocket3->close();
            //qDebug()<<QString("start connect to %1:%2").arg(TCP[2].ServerIP).arg(TCP[2].ServerPort);
            tcpSocket3->connectToHost(TCP[2].ServerIP,TCP[2].ServerPort.toInt());
        }
    }
    }
}
void frmMain::ConnectServer4()
{
    if(tcpSocketTimer4!=NULL){
    if(tcpSocketTimer4==dynamic_cast<QTimer *>(sender()))
    {
        if(TCP[3].isConnected==false&&(tcpSocket4->state()!=tcpSocket4->ConnectingState))
        {
            tcpSocket4->close();
            //qDebug()<<QString("start connect to %1:%2").arg(TCP[3].ServerIP).arg(TCP[3].ServerPort);
            tcpSocket4->connectToHost(TCP[3].ServerIP,TCP[3].ServerPort.toInt());
        }
    }
    }
}


//串口初始化
void frmMain::InitCOM(int port)
{
    #ifdef Q_OS_LINUX
        myCom[port] = new QextSerialPort("/dev/" + COM[port].Name);
    #elif defined (Q_OS_WIN)
        myCom[port] = new QextSerialPort(portName);
    #endif
    //设置波特率
    myCom[port]->setBaudRate((BaudRateType)COM[port].Baudrate);

    //设置数据位
    myCom[port]->setDataBits((DataBitsType)COM[port].Databits);

    //设置校验
    switch(COM[port].Parity){
    case 0:
        myCom[port]->setParity(PAR_NONE);
        break;
    case 1:
        myCom[port]->setParity(PAR_ODD);
        break;
    case 2:
        myCom[port]->setParity(PAR_EVEN);
        break;
    default:
        myCom[port]->setParity(PAR_NONE);
        break;
    }

    //设置停止位
    switch(COM[port].Stopbits){
    case 0:
        myCom[port]->setStopBits(STOP_1);
        break;
    case 1:
     #ifdef Q_OS_WIN
        myCom[port]->setStopBits(STOP_1_5);
     #endif
        break;
    case 2:
        myCom[port]->setStopBits(STOP_2);
        break;
    default:
        myCom[port]->setStopBits(STOP_1);
        break;
    }

    //设置数据流控制
    myCom[port]->setFlowControl(FLOW_OFF);

    //设置延时
    myCom[port]->setTimeout(TIME_OUT);
    if(!myCom[port]->isOpen())//关闭状态下打开
    {
        if(myCom[port]->open(QIODevice::ReadWrite)){
            //qDebug()<<QString("%1 open success").arg(COM[port].Name);

        }else{
            //qDebug()<<QString("%1 open failed").arg(COM[port].Name);
        }

    }

}
enum PowerStatus{IsOff,IsOn}Power_CurrentStatus;

//int PowerOff_Count=0;
//int Power_Old=0;
//int Power_New=Power_Old;

//extern enum Status{IsOpen,IsClose,IsError}Valve_CurrentStatus,Valve_RightStatus;
//extern bool Set_Flag;
//extern int   Valve_Set_Count;
//状态
void frmMain::Status()
{
    //网络状态
    if(TCP[0].ServerOpen){
        if(TCP[0].isConnected){
            ui->netStatus1->setStyleSheet("image: url(:/image/led_blue.png);");
        }
        else{
            ui->netStatus1->setStyleSheet("image: url(:/image/led_red.png);");
        }
    }else{
            ui->netStatus1->setStyleSheet("image: url(:/image/led_gray.png);");
    }

    if(TCP[1].ServerOpen){
        if(TCP[1].isConnected){
            ui->netStatus2->setStyleSheet("image: url(:/image/led_blue.png);");
        }
        else{
            ui->netStatus2->setStyleSheet("image: url(:/image/led_red.png);");
        }
    }else{
            ui->netStatus2->setStyleSheet("image: url(:/image/led_gray.png);");
    }

    if(TCP[2].ServerOpen){
        if(TCP[2].isConnected){
            ui->netStatus3->setStyleSheet("image: url(:/image/led_blue.png);");
        }
        else{
            ui->netStatus3->setStyleSheet("image: url(:/image/led_red.png);");
        }
    }else{
            ui->netStatus3->setStyleSheet("image: url(:/image/led_gray.png);");
    }

    if(TCP[3].ServerOpen){
        if(TCP[3].isConnected){
            ui->netStatus4->setStyleSheet("image: url(:/image/led_blue.png);");
        }
        else{
            ui->netStatus4->setStyleSheet("image: url(:/image/led_red.png);");
        }
    }else{
            ui->netStatus4->setStyleSheet("image: url(:/image/led_gray.png);");
    }
    //市电状态
    if(GetSwitchStatus(myApp::In_power)==true){
        Power_CurrentStatus=IsOn;//开
        ui->powerStatus->setStyleSheet("image: url(:/image/led_blue.png);");
        Power_Old=Power_New;
        Power_New=1;
//        针对老UPS带开关
//        PowerOff_Count=0;
//        SwitchOut_On(8);//市电提供的情况下UPS接通
    }
    else {
        Power_CurrentStatus=IsOff;//关       
        ui->powerStatus->setStyleSheet("image: url(:/image/led_red.png);");
        Power_Old=Power_New;
        Power_New=0;

//        针对老UPS带开关
//        PowerOff_Count++;
//        if(PowerOff_Count>=600)//超过10分钟市电缺失,断开UPS供电
//        {
//            SwitchOut_Off(8);
//            PowerOff_Count=0;
//        }
    }
    if(Power_New!=Power_Old)
    {
        Power_Change=1;
    }
    if(Valve_control::Valve_CurrentStatus==true)
    {
        ui->valveStatus->setStyleSheet("image: url(:/image/led_blue.png);");
    }
    else
    {
        ui->valveStatus->setStyleSheet("image: url(:/image/led_red.png);");
    }
    //阀门状态
//    if(GetSwitchStatus(myApp::In_drain_open)==true&&GetSwitchStatus(myApp::In_drain_close)==false){
//        Valve_CurrentStatus=IsOpen;//开
//        ui->valveStatus->setStyleSheet("image: url(:/image/led_blue.png);");
//    }
//    else if(GetSwitchStatus(myApp::In_drain_open)==false&&GetSwitchStatus(myApp::In_drain_close)==true){
//        Valve_CurrentStatus=IsClose;//关
//        ui->valveStatus->setStyleSheet("image: url(:/image/led_red.png);");
//    }
//    else{
//        Valve_CurrentStatus=IsError;//状态错误
//         ui->valveStatus->setStyleSheet("image: url(:/image/led_gray.png);");
//    }
//    //阀门控制
//    if(Valve_control::Set_Flag)
//    {

//    }
//    if(Set_Flag==true)//有控制阀门操作
//    {
//           Valve_Set_Count++;
//           if(Valve_Set_Count<=Valve_TimeOut)
//           {
//               if(Valve_CurrentStatus==Valve_RightStatus){//控制成功
//                   if(Valve_RightStatus==IsOpen){
//                       v.Catchment_Valve_Open_Set();
//                   }
//                   if(Valve_RightStatus==IsClose){
//                       v.Valve_Close_Clear();
//                   }
//               }
//           }
//           else{//控制超时失败
//                  v.Valve_Open_Clear();
//                  v.Valve_Close_Clear();
//           }
//    }

}

void frmMain::on_btnClearData_clicked()
{
    QSqlQuery query;
    query.exec("delete from [CacheRtd]");
    model_rtd->removeRows(0,model_rtd->rowCount());
}


void frmMain::updateClientStatusDisconnect()
{
    if(tcpSocket1==dynamic_cast<QTcpSocket *>(sender()))
    {
//        tcpSocket1->close();
        TCP[0].isConnected=false;
    }
    if(tcpSocket2==dynamic_cast<QTcpSocket *>(sender()))
    {
//        tcpSocket2->close();
        TCP[1].isConnected=false;
    }
    if(tcpSocket3==dynamic_cast<QTcpSocket *>(sender()))
    {
//        tcpSocket3->close();
        TCP[2].isConnected=false;
    }
    if(tcpSocket4==dynamic_cast<QTcpSocket *>(sender()))
    {
//        tcpSocket4->close();
        TCP[3].isConnected=false;
    }

}

//void frmMain::get_flow_trigger()
//{
//    //qDebug()<<"get flow signal";
//    if(flow.isRunning())
//    {
//        flow.terminate();   //关闭
//        flow.wait(2000); //2000ms
//        while(!flow.isFinished());
//        flow.start();   //开启
//    }
//    else{
//        flow.start();
//    }

//}

//void frmMain::Get_COD_NH_Flg()
//{
//    myApp::COD_FLG=1;
////    myApp::NH_FLG=1;
//}

