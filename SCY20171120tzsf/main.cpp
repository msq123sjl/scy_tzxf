#include "frmmain.h"
#include <QTextCodec>
#include <QTranslator>
#include <QDesktopWidget>
#include "api/myapp.h"
#include "api/myhelper.h"
#include "api/myapi.h"
#include <QFont>
#if QT_VERSION >= 0x050000
#include <QApplication>
#else
#include <QtGui/QApplication>
#endif
#include "frminput.h"
#include "frmnum.h"
#include <stdio.h>
#include <stdlib.h>
//#include <QtCore>
//#include <exception>
extern QSqlDatabase        DbConn;
void myMessageOutput(QtMsgType type, const char *msg);
int main(int argc, char *argv[])
{
    int res=0;
    QApplication a(argc, argv);
    system("echo start[`date`] >> /mnt/nandflash/start.log");
    QFont font("SIMSUN",9,QFont::Normal);           //设置字体
    a.setFont(font);
    qInstallMsgHandler(myMessageOutput);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");      //中文字体编码方式
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);

    //赋值当前应用程序路径和桌面宽度高度
    myApp::AppPath=QApplication::applicationDirPath()+"/";
    myApp::DeskWidth=qApp->desktop()->availableGeometry().width();
    myApp::DeskHeigth=qApp->desktop()->availableGeometry().height();

    //判断当前数据库文件是否存在(如果数据库打开失败则终止应用程序)
       if (!myHelper::FileIsExist("/mnt/sdcard/sf_scy.db")){

        myHelper::ShowMessageBoxError("数据库文件不存在,程序将自动关闭！");
        return 1;
    }
    QSqlDatabase DbConn;                //
    DbConn=QSqlDatabase::addDatabase("QSQLITE");
    DbConn.setDatabaseName("/mnt/sdcard/sf_scy.db");
    //创建数据库连接并打开(如果数据库打开失败则终止应用程序)
    if (!DbConn.open()){
        myHelper::ShowMessageBoxError("打开数据库失败,程序将自动关闭！");
        return 1;
    }
    
    if (myHelper::FileIsExist("/mnt/sdcard/sf_scy.db-journal")){
        system("echo journal[`date`] >> /mnt/nandflash/start.log");
    }
    
    QSqlQuery query;
    QString sql;
    sql="select count(*) from sqlite_master where type='table' ";
    query.exec(sql);
    if(query.next() && query.value(0).toInt() > 0){
        qDebug()<<QString("tab count[%1]").arg(query.value(0).toInt());
    }else{
        qDebug()<<QString("tab count err, quit!!!");
        system("rm -rf /mnt/sdcard/sf_scy.db-journal");
        //system("rm -rf /mnt/sdcard/sf_scy.db");
        //system("cp -f /mnt/sdcard/bak/sf_scy.db /mnt/sdcard/sf_scy.db");
        system("echo cp bak[`date`] >> /mnt/nandflash/start.log");
        system("reboot -n");
        return 1;
    }
    
    if (myHelper::FileIsExist("/mnt/sdcard/sf_scy.db-journal")){
        system("rm -rf /mnt/sdcard/sf_scy.db-journal");
        system("echo rm journal[`date`] >> /mnt/nandflash/start.log");
        system("reboot -n");
        return 1;
    }

    QSqlDatabase memorydb;
    memorydb=QSqlDatabase::addDatabase("QSQLITE","memory");

    memorydb.setDatabaseName(":memory:");
    if(!memorydb.open()){
        myHelper::ShowMessageBoxError("打开内部数据库失败，程序将自动关闭！");
    }
    else
    {
        QSqlQuery query_memory(QSqlDatabase::database("memory",true));
        query_memory.exec("create table CacheRtd(GetTime DATETIME(20),Name NVARCHAR(20),Code NVARCHAR(10),Rtd NVARCHAR(20),Total NVARCHAR(20),Flag NVARCHAR(5),ErrorNums NVARCHAR(2));");
        query_memory.clear();
    }
    //程序加载时先加载所有配置信息
//    try{
    myApp::ReadConfig();
    myApp::ReadIoConfig();


    frmMain w;
    w.showFullScreen();


    //以下方法打开中文输入法
    frmInput::Instance()->Init("control", "black", 10, 10);
        myAPI api;
        api.AddEventInfoUser("系统启动运行");
    res=a.exec();
    DbConn.close();
    QString name;
    {
        name = QSqlDatabase::database().connectionName();
    }//超出作用域，隐含对象QSqlDatabase::database()被删除。
    QSqlDatabase::removeDatabase(name);
    system("echo stop[`date`] >> /mnt/nandflash/start.log");
    return res;

}

void myMessageOutput(QtMsgType type, const char *msg)
 {
     switch (type) {
     case QtDebugMsg:
         fprintf(stderr, "Debug: %s\n", msg);
         break;
     case QtWarningMsg:
         fprintf(stderr, "Warning: %s\n", msg);
         break;
     case QtCriticalMsg:
         fprintf(stderr, "Critical: %s\n", msg);
         break;
     case QtFatalMsg:
         fprintf(stderr, "Fatal: %s\n", msg);
         abort();
     }
 }
