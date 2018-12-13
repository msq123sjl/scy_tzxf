#include "myapi.h"
#include "api/myhelper.h"
#include "myapp.h"
#include <unistd.h>
#include <QStandardItemModel>
#include "api/valve_control.h"
#include "qdebug.h"
#include "frmmain.h"
#include <QSqlRecord>
extern QextSerialPort *myCom[6];
extern Com_para COM[6];
extern Tcp_para TCP[4];
QList<QString> list;

myAPI::myAPI(QObject *parent) :
    QObject(parent)
{
}


//查找统计数据表内某时间段内某记录是否存在
int myAPI::CountRecordIsExist(QString tableName,QString Time)
{
    QSqlQuery query;
    QString sql;
    int res=0;
    sql="select count(*) from "+tableName+" where [GetTime]='"+Time+"'";
    query.exec(sql);
    if(query.next())res=query.value(0).toInt();
    return res;
}

//查找缓存数据表内某污染物记录是否存在
int myAPI::CacheRecordIsExist(QString Code)
{
    QSqlQuery query(QSqlDatabase::database("memory",true));
    QString sql;
    int res=0;
    sql="select count(*) from [CacheRtd]";
    sql+=" where [Code]='"+Code+"'";
    query.exec(sql);
    if(query.next()){
        res=query.value(0).toInt();
    }
    return res;
}

//查找某数据表是否存在
bool myAPI::TableIsExist(QString tableName)
{
    QSqlQuery query;
    QString sql;
    bool res=0;
    sql="select count(*) from sqlite_master where type='table' and name='"+tableName+"'";
    query.exec(sql);
    if(query.next()){
        res=query.value(0).toBool();
    }
    return res;
}

//创建实时数据表
void myAPI::RtdTableCreate(QString tableName)
{
    QSqlQuery query;
    QString sql;
    sql="create table "+tableName+"(GetTime DATETIME(20),";
    sql+="Rtd NVARCHAR(20),Flag NVARCHAR(5),Total NVARCHAR(20))";
    query.exec(sql);

}
//创建统计数据表
void myAPI::CountDataTableCreate(QString tableName)
{
    QSqlQuery query;
    QString sql;
    sql="create table "+tableName+"(GetTime DATETIME(20),";
    sql+="Max NVARCHAR(20),Min NVARCHAR(20),Avg NVARCHAR(20),Cou NVARCHAR(20))";
    query.exec(sql);
}

//统计数据
double myAPI::GetCountDataFromSql(QString tableName,QString StartTime,QString EndTime,QString field,QString func)
{
    QSqlQuery query;
    QString sql;
    QByteArray str;
    double d_value=0;
    sql="select "+func+"(cast("+field+ " as double)) from "+'['+tableName+']';
    sql+=" where [GetTime]>='"+StartTime+"'"+" and [GetTime]<='"+EndTime+"'";
    sql+=" and [Flag]='N'";

    query.exec(sql);
    if(query.next()){
        str=query.value(0).toByteArray();
       d_value =myHelper::Str_To_Double(str.data());
    }
    return d_value;
}

//统计数据
double myAPI::GetCountDataFromSql1(QString tableName,QString StartTime,QString EndTime,QString field,QString func)
{
    QSqlQuery query;
    QString sql;
    double d_value=-1;
    sql="select "+func+"(cast("+field+ " as double)) from "+'['+tableName+']';
    sql+=" where [GetTime]>='"+StartTime+"'"+" and [GetTime]<='"+EndTime+"'";
    query.exec(sql);
   if(query.next()){
    QByteArray str=query.value(0).toByteArray();
    d_value=myHelper::Str_To_Double(str.data());
   }
    return d_value;
}

void myAPI::CacheDataProc(double rtd,double total,QString flag,int dec,QString name,QString code,QString unit)
{
    QSqlQuery query(QSqlDatabase::database("memory",true));
    QString sql;
    char str[20];
    int errorNums=0;
    QString format=QString("%.%1f").arg(dec);
    QDateTime dt=QDateTime::currentDateTime();

    if(flag=="D"){
        query.exec(QString("select [ErrorNums] from [CacheRtd] where [Code]='%1'").arg(code));
        if(query.next()){
            errorNums=query.value(0).toInt();
            if(errorNums<5){
                errorNums++;
                sql = "update [CacheRtd] set [ErrorNums]='"+QString::number(errorNums)+"' where [Code]='"+code+"'";
                query.exec(sql);//更新显示数据
                return;
            }else{
                errorNums=5;
            }
        }
    }else{
        errorNums=0;
    }

    if(CacheRecordIsExist(code)==0){

            sql = "insert into [CacheRtd]";
            sql+="([GetTime],[Name],[Code],[Rtd],[Total],[Flag],[ErrorNums])values('";
            sql+= QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+ "','";
            sql+= name + "','";
            sql+= code + "','";
            sprintf(str,format.toLatin1().data(),rtd);
            sql+= QString(str) +" "+unit+ "','";
            if(total==0){
                sql+= "--','";
            }else{
                sprintf(str,format.toLatin1().data(),total);
                sql+= QString(str) + "','";
            }
            sql+= flag + "','";
            sql+= QString::number(errorNums) + "')";
            query.exec(sql);//插入显示数据

        }
    else{
            sql = "update [CacheRtd] set";
            sql+=" [GetTime]='"+dt.toString("yyyy-MM-dd hh:mm:ss")+"',";
            sql+= "[Name]='"+name + "',";
            sprintf(str,format.toLatin1().data(),rtd);
            sql+= "[Rtd]='"+QString(str) +" "+unit+ "',";
            if(total==0){
                sql+= "[Total]='--',";
            }else
            {
                sprintf(str,format.toLatin1().data(),total);
                sql+= "[Total]='"+QString(str) + "',";
            }
            sql+= "[Flag]='"+flag + "',";
            sql+= "[ErrorNums]='"+QString::number(errorNums)+"'";
            sql+= " where [Code]='"+code+"'";
            query.exec(sql);//更新显示数据

    }
}

void myAPI::CacheCard(QString cardtype,QString cardno)
{
    QSqlQuery query;
    QString sql;
    QString currenttime=QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    sql=QString("insert into [DoorRecord] ([GetTime],[CardType],[CardNo],[T1SendTime],[T2SendTime],[T3SendTime],[T4SendTime],[T5SendTime]) values('%1','%2','%3','0','0','0','0','0')")
            .arg(currenttime,cardtype,cardno);
    query.exec(sql);
//    query.exec(QString("select [ErrorNums] from [CacheRtd] where [Code]='%1'").arg(code));
}

void myAPI::RtdProc()
{
    QSqlQuery query(QSqlDatabase::database("memory",true));
    QSqlQuery query1;
    QString sql;
    QString temp,code,rtd,total,dt;

//    sql="select * from [CacheRtd] where [flag]='N'";
    sql="select * from [CacheRtd]";
    query.exec(sql);
    while(query.next())
    {
        dt=query.value(0).toString();
        code=query.value(2).toString();
        rtd=query.value(3).toString().split(" ")[0];
        total=query.value(4).toString();

        temp=QString("Rtd_%1_%2%3%4")
                .arg(code)
                .arg(dt.left(4))
                .arg(dt.mid(5,2))
                .arg(dt.mid(8,2));
        if(!TableIsExist(temp)){
            RtdTableCreate(temp);
        }
        sql = "insert into "+temp+"([GetTime],[Rtd],[Flag],[Total])values('";
        sql+= dt+ "','";
        sql+= rtd + "','";
        sql+= query.value(5).toString() + "','";
        sql+= total + "')";
        query1.exec(sql);//插入实时数据表
    }

}

//水流量数据统计--分钟数据
void myAPI::MinsDataProc_WaterFlow(QString startTime,QString endTime)
{
    bool max_flag,min_flag,avg_flag,cou_flag;
    QString code,format;
    double max_value=0,min_value=0,avg_value=0,cou_value=0;
    char str[20];
    QSqlQuery query,query1;
    QString sql;
    QString temp;
    double total1=0,total2=0;
//    if(!TableIsExist("Mins_w00000"))return;
    query.exec("select * from ParaInfo where [Code]='w00000'");
    while(query.next())
    {
        code=query.value(1).toString();
        max_flag=query.value(11).toBool();
        min_flag=query.value(12).toBool();
        avg_flag=query.value(13).toBool();
        cou_flag=query.value(14).toBool();
        format=QString("%.%1f").arg(query.value(15).toInt());
        if(!TableIsExist(QString("Mins_%1").arg(code))){
            CountDataTableCreate(QString("Mins_%1").arg(code));
            continue;
        }

        if(CountRecordIsExist(QString("Mins_%1").arg(code),startTime)==true)continue;
        temp=QString("Rtd_%1_%2%3%4")
                .arg(code)
                .arg(startTime.left(4))
                .arg(startTime.mid(5,2))
                .arg(startTime.mid(8,2));
        if(!TableIsExist(temp))continue;

        max_value=GetCountDataFromSql(temp,startTime,endTime,"Rtd","MAX");
        min_value=GetCountDataFromSql(temp,startTime,endTime,"Rtd","MIN");
        avg_value=GetCountDataFromSql(temp,startTime,endTime,"Rtd","AVG");

        sql="select [Total] from "+temp+" where [Flag]='N' and [GetTime]>='"+startTime+"' and [GetTime]<='"+endTime+"'  order by [GetTime] asc";
        query1.exec(sql);
        if(query1.first())total1=myHelper::Str_To_Double(query1.value(0).toByteArray().data());
        if(query1.last())total2=myHelper::Str_To_Double(query1.value(0).toByteArray().data());
        cou_value=total2-total1;

        sql = "insert into Mins_"+code;
        sql+= "([GetTime],[Max],[Min],[Avg],[Cou])values('";
        sql+= startTime+ "','";
        if(max_flag){
            sprintf(str,format.toLatin1().data(),max_value);
            sql+= QString(str) + "','";
        }else{
            sql+= "--','";
        }
        if(min_flag){
            sprintf(str,format.toLatin1().data(),min_value);
            sql+= QString(str) + "','";
        }else{
            sql+="--','";
        }
        if(avg_flag){
            sprintf(str,format.toLatin1().data(),avg_value);
            sql+= QString(str) + "','";
        }else{
            sql+="--','";
        }
        if(cou_flag){
            sprintf(str,format.toLatin1().data(),cou_value);
            sql+= QString(str) + "')";
        }else{
            sql+="--')";
        }

        query1.exec(sql);//插入分钟数据表

    }
}

//水污染因子数据统计--分钟数据
void myAPI::MinsDataProc_WaterPara(QString startTime,QString endTime)
{
    bool max_flag,min_flag,avg_flag,cou_flag;
    QString code,format;
    double max_value=0,min_value=0,avg_value=0,cou_value=0;
    char str[20];
    QSqlQuery query,query1;
    QString sql;
    QString temp;
    double total=0;

    query.exec("select * from ParaInfo where [Code]<>'w00000'");
//    query.exec("select * from ParaInfo where [Code]='w00000'");

    while(query.next())
    {
        code=query.value(1).toString();
        if(!TableIsExist(QString("Mins_%1").arg(code))){
            CountDataTableCreate(QString("Mins_%1").arg(code));

        }
        if(CountRecordIsExist(QString("Mins_%1").arg(code),startTime)==true)continue;
        max_flag=query.value(11).toBool();
        min_flag=query.value(12).toBool();
        avg_flag=query.value(13).toBool();
        cou_flag=query.value(14).toBool();
        format=QString("%.%1f").arg(query.value(15).toInt());
        temp=QString("Rtd_%1_%2%3%4")
                .arg(code)
                .arg(startTime.left(4))
                .arg(startTime.mid(5,2))
                .arg(startTime.mid(8,2));
        if(!TableIsExist(temp))continue;
        max_value=GetCountDataFromSql(temp,startTime,endTime,"Rtd","MAX");
        min_value=GetCountDataFromSql(temp,startTime,endTime,"Rtd","MIN");
        avg_value=GetCountDataFromSql(temp,startTime,endTime,"Rtd","AVG");
        if(cou_flag){
            if(TableIsExist("Mins_w00000")){
                sql="select [Cou] from [Mins_w00000] where [GetTime]='"+startTime+"'";
                query1.exec(sql);
                if(query1.next()){
                    total=myHelper::Str_To_Double(query1.value(0).toByteArray().data());
                    cou_value=total*avg_value*0.001;
                    qDebug()<<QString("Mins_%1_total=%2").arg(code).arg(total);
                 }
            }
        }
        sql = "insert into Mins_"+code;
        sql+= "([GetTime],[Max],[Min],[Avg],[Cou])values('";
        sql+= startTime+ "','";
        if(max_flag){
            sprintf(str,format.toLatin1().data(),max_value);
            sql+= QString(str) + "','";
        }else{
            sql+= "--','";
        }
        if(min_flag){
            sprintf(str,format.toLatin1().data(),min_value);
            sql+= QString(str) + "','";
        }else{
            sql+="--','";
        }
        if(avg_flag){
            sprintf(str,format.toLatin1().data(),avg_value);
            sql+= QString(str) + "','";
        }else{
            sql+="--','";
        }
        if(cou_flag){
            sprintf(str,format.toLatin1().data(),cou_value);
            sql+= QString(str) + "')";
        }else{
            sql+="--')";
        }

        query1.exec(sql);//插入分钟数据表

    }
}

//水流量数据统计--小时数据
void myAPI::HourDataProc_WaterFlow(QString startTime,QString endTime)
{
    bool max_flag,min_flag,avg_flag,cou_flag;
    QString code,format;
    double max_value=0,min_value=0,avg_value=0,cou_value=0;
    char str[20];
    QSqlQuery query,query1;
    QString sql;
    QString temp;
    double total1=0,total2=0;

    query.exec("select * from ParaInfo where [Code]='w00000'");
    if(query.first())
    {
        code=query.value(1).toString();
        if(!TableIsExist(QString("Hour_%1").arg(code))){
            CountDataTableCreate(QString("Hour_%1").arg(code));
        }
        if(CountRecordIsExist(QString("Hour_%1").arg(code),startTime)==true)return;
        max_flag=query.value(11).toBool();
        min_flag=query.value(12).toBool();
        avg_flag=query.value(13).toBool();
        cou_flag=query.value(14).toBool();
        format=QString("%.%1f").arg(query.value(15).toInt());
        temp=QString("Rtd_%1_%2%3%4")
                .arg(code)
                .arg(startTime.left(4))
                .arg(startTime.mid(5,2))
                .arg(startTime.mid(8,2));
        if(!TableIsExist(temp))return;
        max_value=GetCountDataFromSql(temp,startTime,endTime,"Rtd","MAX");
        min_value=GetCountDataFromSql(temp,startTime,endTime,"Rtd","MIN");
        avg_value=GetCountDataFromSql(temp,startTime,endTime,"Rtd","AVG");
        sql="select [Total] from "+temp+" where [Flag]='N' and [GetTime]>='"+startTime+"' and [GetTime]<='"+endTime+"'  order by [GetTime] asc";

        query1.exec(sql);
        if(query1.first())total1=myHelper::Str_To_Double(query1.value(0).toByteArray().data());
        if(query1.last())total2=myHelper::Str_To_Double(query1.value(0).toByteArray().data());
        cou_value=total2-total1;

        sql = "insert into Hour_"+code;
        sql+= "([GetTime],[Max],[Min],[Avg],[Cou])values('";
        sql+= startTime+ "','";
        if(max_flag){
            sprintf(str,format.toLatin1().data(),max_value);
            sql+= QString(str) + "','";
        }else{
            sql+= "--','";
        }
        if(min_flag){
            sprintf(str,format.toLatin1().data(),min_value);
            sql+= QString(str) + "','";
        }else{
            sql+="--','";
        }
        if(avg_flag){
            sprintf(str,format.toLatin1().data(),avg_value);
            sql+= QString(str) + "','";
        }else{
            sql+="--','";
        }
        if(cou_flag){
            sprintf(str,format.toLatin1().data(),cou_value);
            sql+= QString(str) + "')";
        }else{
            sql+="--')";
        }
        query1.exec(sql);//插入小时数据表
    }
}

//水污染因子数据统计--小时数据
void myAPI::HourDataProc_WaterPara(QString startTime,QString endTime)
{
    bool max_flag,min_flag,avg_flag,cou_flag;
    QString code,format;
    double max_value=0,min_value=0,avg_value=0,cou_value=0;
    char str[20];
    QSqlQuery query,query1;
    QString sql;
    QString temp;
    double total=0;

    query.exec("select * from ParaInfo where [Code]<>'w00000'");
    while(query.next())
    {
        code=query.value(1).toString();
        if(!TableIsExist(QString("Hour_%1").arg(code))){
            CountDataTableCreate(QString("Hour_%1").arg(code));
        }
        if(CountRecordIsExist(QString("Hour_%1").arg(code),startTime)==true)continue;
        max_flag=query.value(11).toBool();
        min_flag=query.value(12).toBool();
        avg_flag=query.value(13).toBool();
        cou_flag=query.value(14).toBool();
        format=QString("%.%1f").arg(query.value(15).toInt());
        temp=QString("Rtd_%1_%2%3%4")
                .arg(code)
                .arg(startTime.left(4))
                .arg(startTime.mid(5,2))
                .arg(startTime.mid(8,2));
        if(!TableIsExist(temp))continue;
        max_value=GetCountDataFromSql(temp,startTime,endTime,"Rtd","MAX");
        min_value=GetCountDataFromSql(temp,startTime,endTime,"Rtd","MIN");
        avg_value=GetCountDataFromSql(temp,startTime,endTime,"Rtd","AVG");
        if(TableIsExist("Hour_w00000")){
            sql="select [Cou] from [Hour_w00000] where [GetTime]='"+startTime+"'";
            qDebug()<<QString(sql);
            query1.exec(sql);
            if(query1.next()){
            total=myHelper::Str_To_Double(query1.value(0).toByteArray().data());
            cou_value=total*avg_value*0.001;
            qDebug()<<QString("%1_Hour_total=%2").arg(code).arg(total);
            }
        }
        sql = "insert into Hour_"+code;
        sql+= "([GetTime],[Max],[Min],[Avg],[Cou])values('";
        sql+= startTime+ "','";
        if(max_flag){
            sprintf(str,format.toLatin1().data(),max_value);
            sql+= QString(str) + "','";
        }else{
            sql+= "--','";
        }
        if(min_flag){
            sprintf(str,format.toLatin1().data(),min_value);
            sql+= QString(str) + "','";
        }else{
            sql+="--','";
        }
        if(avg_flag){
            sprintf(str,format.toLatin1().data(),avg_value);
            sql+= QString(str) + "','";
        }else{
            sql+="--','";
        }
        if(cou_flag){
            sprintf(str,format.toLatin1().data(),cou_value);
            sql+= QString(str) + "')";
        }else{
            sql+="--')";
        }
        query1.exec(sql);//插入小时数据表
    }
}

void myAPI::DayDataProc(QString startTime,QString endTime)
{
    bool max_flag,min_flag,avg_flag,cou_flag;
    QString code,format;
    double max_value=0,min_value=0,avg_value=0,cou_value;
    char str[20];
    QSqlQuery query,query1;
    QString sql;
    QString temp;

    query.exec("select * from ParaInfo");
    while(query.next())
    {
        code=query.value(1).toString();
        if(!TableIsExist(QString("Day_%1").arg(code))){
            CountDataTableCreate(QString("Day_%1").arg(code));
        }
        if(CountRecordIsExist(QString("Day_%1").arg(code),startTime)==true)continue;
        max_flag=query.value(11).toBool();
        min_flag=query.value(12).toBool();
        avg_flag=query.value(13).toBool();
        cou_flag=query.value(14).toBool();
        format=QString("%.%1f").arg(query.value(15).toInt());
        temp=QString("Hour_%1").arg(code);
        if(!TableIsExist(temp))continue;
        max_value=GetCountDataFromSql1(temp,startTime,endTime,"Max","MAX");
        min_value=GetCountDataFromSql1(temp,startTime,endTime,"Min","MIN");
        avg_value=GetCountDataFromSql1(temp,startTime,endTime,"Avg","AVG");
        cou_value=GetCountDataFromSql1(temp,startTime,endTime,"Cou","SUM");  //注意单位

        sql = "insert into Day_"+code;
        sql+= "([GetTime],[Max],[Min],[Avg],[Cou])values('";
        sql+= startTime+ "','";
        if(max_flag){
            sprintf(str,format.toLatin1().data(),max_value);
            sql+= QString(str) + "','";
        }else{
            sql+= "--','";
        }
        if(min_flag){
            sprintf(str,format.toLatin1().data(),min_value);
            sql+= QString(str) + "','";
        }else{
            sql+="--','";
        }
        if(avg_flag){
            sprintf(str,format.toLatin1().data(),avg_value);
            sql+= QString(str) + "','";
        }else{
            sql+="--','";
        }
        if(cou_flag){
            sprintf(str,format.toLatin1().data(),cou_value);
            sql+= QString(str) + "')";
        }else{
            sql+="--')";
        }
        query1.exec(sql);//插入日数据表
    }
}

//添加事件记录
void myAPI::AddEventInfo(QString TriggerType, QString TriggerContent)
{
    QString sql = "insert into [EventInfo]([TriggerTime],[TriggerType],";
    sql+="[TriggerContent],[TriggerUser])values('";
    sql += QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "','";
    sql += TriggerType + "','";
    sql += TriggerContent + "','";
    sql += myApp::CurrentUserName + "')";
    QSqlQuery query;
    query.exec(sql);
}

void myAPI::AddEventInfoUser(QString TriggerContent)
{
  this->AddEventInfo("用户操作", TriggerContent);
}


//读取监测污染物信息
QStandardItemModel  *model_rtd;
void myAPI::ShowRtd()
{
    QSqlQuery query(QSqlDatabase::database("memory",true));
    query.exec("select [Name],[Rtd],[Total],[Flag] from [CacheRtd]");
    int rows=0;
    while(query.next())
    {
        for(int i=0;i<model_rtd->columnCount();i++){
            model_rtd->setItem(rows,i,new QStandardItem(query.value(i).toString()));
//            model_rtd->item(rows,i)->setForeground(QBrush(QColor(0, 0, 0)));    //设置字符颜色
            model_rtd->item(rows,i)->setTextAlignment(Qt::AlignCenter);           //设置字符位置
            if(rows%2==1){
                if(query.value(i).toString()=="T"){
                    model_rtd->item(rows,i)->setBackground(QBrush(QColor(150,0,0,80)));
                }else{
                    model_rtd->item(rows,i)->setBackground(QBrush(QColor(0,255,0,80)));
                }
            }
        }
        rows++;
    }

}

void myAPI::InsertList(QString str)
{
    if(list.size()>=20){
        list.clear();
    }
    list.append(str);

}

void myAPI::Insert_Message_Count(int CN,int flag,QString dt)
{
    QString content;
    QSqlQuery query,query1;
    bool max_flag,min_flag,avg_flag,cou_flag;
    QByteArray ch;
    QString code;
    QString sql;
    QString QN=QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    content="QN="+QN+";ST="+ST_to_Str[myApp::StType];
    content+=";CN="+QString::number(CN)+";PW=123456;MN="+myApp::MN;
    content+=";Flag="+QString::number(flag)+";CP=&&DataTime="+QString("%1%2%3%4%5%6")
            .arg(dt.mid(0,4))
            .arg(dt.mid(5,2))
            .arg(dt.mid(8,2))
            .arg(dt.mid(11,2))
            .arg(dt.mid(14,2))
            .arg(dt.mid(17,2));

    if(CN==3081||CN==3082){
        content+="&&";
        ch.resize(4);
        int len=content.size();
        int crc=myHelper::CRC16_GB212(content.toLatin1().data(),len);
        sprintf(ch.data(),"%04X",crc);
        content.append(ch+"\r\n");
        ch[0] = (len/1000)+'0';
        ch[1] = (len%1000/100)+'0';
        ch[2] = (len%100/10)+'0';
        ch[3] = (len%10)+'0';
        content.insert(0,"##");
        content.insert(2,ch);
        sql = "insert into MessageSend([QN],[CN],[Content],";
        sql+="[Target1_SendTimes],[Target2_SendTimes],[Target3_SendTimes],[Target4_SendTimes],[Target5_SendTimes],";
        sql+="[Target1_IsRespond],[Target2_IsRespond],[Target3_IsRespond],[Target4_IsRespond],[Target5_IsRespond])values('";
        sql+= QN+ "','"+QString::number(CN)+"','"+content+"','0','0','0','0','0','--','--','--','--','--')";\
        query.exec(sql);//插入发送数据表
        return;
    }
    if(CN==3097){
        QString cardtype;
        QString cardno;
        query.exec("select [CardType],[CardNo] from DoorRecord where [SendTime]='0'");
        while(query.next()){
            cardtype=query.value(0).toString();
            cardno=query.value(1).toString();
            if(cardtype=="1")cardtype="maintain";
            if(cardtype=="2")cardtype="admin";
            if(cardtype=="3")cardtype="guest";
            if(cardtype=="4")cardtype="others";
            content+=QString(";CardNo=%1;CardType=%2;Gate=ON").arg(cardno).arg(cardtype);
            content+="&&";
            ch.resize(4);
            int len=content.size();
            int crc=myHelper::CRC16_GB212(content.toLatin1().data(),len);
            sprintf(ch.data(),"%04X",crc);
            content.append(ch+"\r\n");
            ch[0] = (len/1000)+'0';
            ch[1] = (len%1000/100)+'0';
            ch[2] = (len%100/10)+'0';
            ch[3] = (len%10)+'0';
            content.insert(0,"##");
            content.insert(2,ch);
            sql = "insert into MessageSend([QN],[CN],[Content],";
            sql+="[Target1_SendTimes],[Target2_SendTimes],[Target3_SendTimes],[Target4_SendTimes],[Target5_SendTimes],";
            sql+="[Target1_IsRespond],[Target2_IsRespond],[Target3_IsRespond],[Target4_IsRespond],[Target5_IsRespond])values('";
            sql+= QN+ "','"+QString::number(CN)+"','"+content+"','0','0','0','0','0','--','--','--','--','--')";\
            query.exec(sql);//插入发送数据表
            return;

        }

    }
        query.exec("select * from ParaInfo");
        while(query.next())
        {
            code=query.value(1).toString();
            max_flag=query.value(11).toBool();
            min_flag=query.value(12).toBool();
            avg_flag=query.value(13).toBool();
            cou_flag=query.value(14).toBool();
            if(CN==2051){
                query1.exec(QString("select * from [Mins_%1] where [GetTime]='%2'").arg(code).arg(dt));
            }else if(CN==2061){
                query1.exec(QString("select * from [Hour_%1] where [GetTime]='%2'").arg(code).arg(dt));
            }else if(CN==2031){
                query1.exec(QString("select * from [Day_%1] where [GetTime]='%2'").arg(code).arg(dt));
            }else {
                return;
            }

            while(query1.next())
            {
                if(max_flag)
                {
                    content+=';'+code+"-Max="+query1.value(1).toString();
                }
                if(min_flag)
                {
                    content+=','+code+"-Min="+query1.value(2).toString();
                }
                if(avg_flag)
                {
                    content+=','+code+"-Avg="+query1.value(3).toString();
                }
                if(cou_flag)
                {
                    content+=','+code+"-Cou="+query1.value(4).toString();
                }

            }

        }

    content+="&&";
    if(CN==3081||CN==3082)qDebug()<<content;

    ch.resize(4);
    int len=content.size();
    int crc=myHelper::CRC16_GB212(content.toLatin1().data(),len);
    sprintf(ch.data(),"%04X",crc);
    content.append(ch+"\r\n");
    ch[0] = (len/1000)+'0';
    ch[1] = (len%1000/100)+'0';
    ch[2] = (len%100/10)+'0';
    ch[3] = (len%10)+'0';
    content.insert(0,"##");
    content.insert(2,ch);
    if(flag&0x01){
        sql = "insert into MessageSend([QN],[CN],[Content],";
        sql+="[Target1_SendTimes],[Target2_SendTimes],[Target3_SendTimes],[Target4_SendTimes],[Target5_SendTimes],";
        sql+="[Target1_IsRespond],[Target2_IsRespond],[Target3_IsRespond],[Target4_IsRespond],[Target5_IsRespond])values('";
        sql+= QN+ "','"+QString::number(CN)+"','"+content+"','0','0','0','0','0','false','false','false','false','false')";
    }
    else{
        sql = "insert into MessageSend([QN],[CN],[Content],";
        sql+="[Target1_SendTimes],[Target2_SendTimes],[Target3_SendTimes],[Target4_SendTimes],[Target5_SendTimes],";
        sql+="[Target1_IsRespond],[Target2_IsRespond],[Target3_IsRespond],[Target4_IsRespond],[Target5_IsRespond])values('";
        sql+= QN+ "','"+QString::number(CN)+"','"+content+"','0','0','0','0','0','--','--','--','--','--')";
    }
    query.exec(sql);//插入发送数据表

}
void myAPI::Insert_Message_SampleBottle(int flag,int b1,int b2,int b3,int b4,int b5,int b6,int b7,int b8,int b9,int b10,int b11,int b12,int b13,int b14,int b15,int b16,int b17,int b18,int b19,int b20,int b21,int b22,int b23,int b24)
{
    QString content;
    QSqlQuery query;
    QByteArray ch;
    QString sql;
    QString QN=QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");

    content="QN="+QN+";ST="+ST_to_Str[myApp::StType];
    content+=";CN=4012;PW=123456;MN="+myApp::MN+";Flag="+QString::number(flag)+";CP=&&DataTime="+QString("%1%2%3%4%5%6;")
            .arg(QN.mid(0,4).toInt(),4,10,QChar('0'))
            .arg(QN.mid(4,2).toInt(),2,10,QChar('0'))
            .arg(QN.mid(6,2).toInt(),2,10,QChar('0'))
            .arg(QN.mid(8,2).toInt(),2,10,QChar('0'))
            .arg(QN.mid(10,2).toInt(),2,10,QChar('0'))
            .arg(QN.mid(12,2).toInt(),2,10,QChar('0'));
    content+=QString("VaseNo1-Quantity=%1,").arg(b1);
    content+=QString("VaseNo2-Quantity=%1,").arg(b2);
    content+=QString("VaseNo3-Quantity=%1,").arg(b3);
    content+=QString("VaseNo4-Quantity=%1,").arg(b4);
    content+=QString("VaseNo5-Quantity=%1,").arg(b5);
    content+=QString("VaseNo6-Quantity=%1,").arg(b6);
    content+=QString("VaseNo7-Quantity=%1,").arg(b7);
    content+=QString("VaseNo8-Quantity=%1,").arg(b8);
    content+=QString("VaseNo9-Quantity=%1,").arg(b9);
    content+=QString("VaseNo10-Quantity=%1,").arg(b10);
    content+=QString("VaseNo11-Quantity=%1,").arg(b11);
    content+=QString("VaseNo12-Quantity=%1,").arg(b12);
    content+=QString("VaseNo13-Quantity=%1,").arg(b13);
    content+=QString("VaseNo14-Quantity=%1,").arg(b14);
    content+=QString("VaseNo15-Quantity=%1,").arg(b15);
    content+=QString("VaseNo16-Quantity=%1,").arg(b16);
    content+=QString("VaseNo17-Quantity=%1,").arg(b17);
    content+=QString("VaseNo18-Quantity=%1,").arg(b18);
    content+=QString("VaseNo19-Quantity=%1,").arg(b19);
    content+=QString("VaseNo20-Quantity=%1,").arg(b20);
    content+=QString("VaseNo21-Quantity=%1,").arg(b21);
    content+=QString("VaseNo22-Quantity=%1,").arg(b22);
    content+=QString("VaseNo23-Quantity=%1,").arg(b23);
    content+=QString("VaseNo24-Quantity=%1").arg(b24);
    content+="&&";
    ch.resize(4);
    int len=content.size();
    int crc=myHelper::CRC16_GB212(content.toLatin1().data(),len);
    sprintf(ch.data(),"%04X",crc);
    content.append(ch+"\r\n");
    ch[0] = (len/1000)+'0';
    ch[1] = (len%1000/100)+'0';
    ch[2] = (len%100/10)+'0';
    ch[3] = (len%10)+'0';
    content.insert(0,"##");
    content.insert(2,ch);

    if(flag&0x01){
        sql = "insert into MessageSend([QN],[CN],[Content],";
        sql+="[Target1_SendTimes],[Target2_SendTimes],[Target3_SendTimes],[Target4_SendTimes],[Target5_SendTimes],";
        sql+="[Target1_IsRespond],[Target2_IsRespond],[Target3_IsRespond],[Target4_IsRespond],[Target5_IsRespond])values('";
        sql+= QN+ "','4012','"+content+"','0','0','0','0','0','false','false','false','false','false')";
    }
    else{
        sql = "insert into MessageSend([QN],[CN],[Content],";
        sql+="[Target1_SendTimes],[Target2_SendTimes],[Target3_SendTimes],[Target4_SendTimes],[Target5_SendTimes],";
        sql+="[Target1_IsRespond],[Target2_IsRespond],[Target3_IsRespond],[Target4_IsRespond],[Target5_IsRespond])values('";
        sql+= QN+ "','4012','"+content+"','0','0','0','0','0','--','--','--','--','--')";
    }
    qDebug()<<sql;
    query.exec(sql);//插入发送实时数据表
}


void myAPI::Insert_Message_Rtd(int flag,QString dt)
{
    QString content;
    QSqlQuery query(QSqlDatabase::database("memory",true));
    QSqlQuery query1;
    QByteArray ch;
    QString sql;
    QString QN=QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");

    content="QN="+QN+";ST="+ST_to_Str[myApp::StType];
    content+=";CN=2011;PW=123456;MN="+myApp::MN+";Flag="+QString::number(flag)+";CP=&&DataTime="+QString("%1%2%3%4%5%6")
            .arg(dt.mid(0,4))
            .arg(dt.mid(5,2))
            .arg(dt.mid(8,2))
            .arg(dt.mid(11,2))
            .arg(dt.mid(14,2))
            .arg(dt.mid(17,2));
    query.exec("select * from [CacheRtd]");
    while(query.next())
    {
        content+=";"+query.value(2).toString()+"-Rtd="+query.value(3).toString().split(" ")[0];
        content+=","+query.value(2).toString()+"-Flag="+query.value(5).toString();
    }
    content+="&&";

    ch.resize(4);
    int len=content.size();
    int crc=myHelper::CRC16_GB212(content.toLatin1().data(),len);
    sprintf(ch.data(),"%04X",crc);
    content.append(ch+"\r\n");
    ch[0] = (len/1000)+'0';
    ch[1] = (len%1000/100)+'0';
    ch[2] = (len%100/10)+'0';
    ch[3] = (len%10)+'0';
    content.insert(0,"##");
    content.insert(2,ch);

    if(flag&0x01){
        sql = "insert into MessageSend([QN],[CN],[Content],";
        sql+="[Target1_SendTimes],[Target2_SendTimes],[Target3_SendTimes],[Target4_SendTimes],[Target5_SendTimes],";
        sql+="[Target1_IsRespond],[Target2_IsRespond],[Target3_IsRespond],[Target4_IsRespond],[Target5_IsRespond])values('";
        sql+= QN+ "','2011','"+content+"','0','0','0','0','0','false','false','false','false','false')";
    }
    else{
        sql = "insert into MessageSend([QN],[CN],[Content],";
        sql+="[Target1_SendTimes],[Target2_SendTimes],[Target3_SendTimes],[Target4_SendTimes],[Target5_SendTimes],";
        sql+="[Target1_IsRespond],[Target2_IsRespond],[Target3_IsRespond],[Target4_IsRespond],[Target5_IsRespond])values('";
        sql+= QN+ "','2011','"+content+"','0','0','0','0','0','--','--','--','--','--')";
    }
    query1.exec(sql);//插入发送实时数据表

}

void myAPI::Update_Respond(QString QN,QString From)
{
    QSqlQuery query;
    QString sql;

    if(From==QString("%1:%2").arg(TCP[0].ServerIP).arg(TCP[0].ServerPort)){
        sql = "update [MessageSend] set [Target1_IsRespond]='true' where [QN]='"+QN+"'";
        query.exec(sql);//更新接收标记
    }else if(From==QString("%1:%2").arg(TCP[1].ServerIP).arg(TCP[1].ServerPort)){
        sql = "update [MessageSend] set [Target2_IsRespond]='true' where [QN]='"+QN+"'";
        query.exec(sql);//更新接收标记
    }else if(From==QString("%1:%2").arg(TCP[2].ServerIP).arg(TCP[2].ServerPort)){
        sql = "update [MessageSend] set [Target3_IsRespond]='true' where [QN]='"+QN+"'";
        query.exec(sql);//更新接收标记
    }else if(From==QString("%1:%2").arg(TCP[3].ServerIP).arg(TCP[3].ServerPort)){
        sql = "update [MessageSend] set [Target4_IsRespond]='true' where [QN]='"+QN+"'";
        query.exec(sql);//更新接收标记
    }else if(From=="COM3"){
        sql = "update [MessageSend] set [Target5_IsRespond]='true' where [QN]='"+QN+"'";
        query.exec(sql);//更新接收标记
    }

    sql = "update [MessageReceived] set [IsProcessed]='true' where [QN]='"+QN+"'";
    query.exec(sql);//更新处理标记

}

void myAPI::Insert_Message_Received(QString QN,int CN,QString From,QString Content)
{
    QString sql;
    QSqlQuery query;

    sql = QString("insert into MessageReceived([ReceivedTime],[QN],[CN],[From],[IsProcessed],[Content])values('%1','%2','%3','%4','false','%5')")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(QN)
            .arg(CN)
            .arg(From)
            .arg(Content);

    query.exec(sql);//插入接收数据表
}

void myAPI::Insert_Message_Card_Add(QString QN,QString CardNo,QString CardType)
{
    QString sql;
    QSqlQuery query;

    sql = QString("insert into CardNumber([ReceivedTime],[QN],[CardNo],[CardType],[SendCount],[Attribute])values('%1','%2','%3','%4','0','add')")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(QN)
            .arg(CardNo)
            .arg(CardType);

    query.exec(sql);//插入接收数据表
}

void myAPI::Update_Message_Card_delete(QString CardNo,QString CardType)
{
    QString sql;
    QSqlQuery query,query1;
    sql=QString("select [CardNo], [CardType] from CardNumber where [CardNo]='%1' and [CardType]='%2'")
            .arg(CardNo).arg(CardType);
    query.exec(sql);
    if(query.first())
    {
        sql = QString("update CardNumber set [Attribute]='delete'where [CardNo]='%1'").arg(CardNo);
        query1.exec(sql);
    }
    query1.clear();
    query.clear();
}

extern QTcpSocket *tcpSocket1;
extern QTcpSocket *tcpSocket2;
extern QTcpSocket *tcpSocket3;
extern QTcpSocket *tcpSocket4;
extern Tcp_para     TCP[4];
extern QextSerialPort *myCom[6];

void myAPI::SendData_Status(int flag)
{
    QString content;
    QByteArray ch;
    QString dt;
    QString QN=QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    dt=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    content="QN="+QN+";ST="+ST_to_Str[myApp::StType];
    content+=";CN=3071;PW=123456;MN="+myApp::MN+";Flag="+QString::number(flag)+";CP=&&DataTime="+QString("%1%2%3%4%5%6")
            .arg(dt.mid(0,4))
            .arg(dt.mid(5,2))
            .arg(dt.mid(8,2))
            .arg(dt.mid(11,2))
            .arg(dt.mid(14,2))
            .arg(dt.mid(17,2));
    content+=";Gate="+myApp::DoorStatus;
    content+=";&&";

    ch.resize(4);
    int len=content.size();
    int crc=myHelper::CRC16_GB212(content.toLatin1().data(),len);
    sprintf(ch.data(),"%04X",crc);
    content.append(ch+"\r\n");
    ch[0] = (len/1000)+'0';
    ch[1] = (len%1000/100)+'0';
    ch[2] = (len%100/10)+'0';
    ch[3] = (len%10)+'0';
    content.insert(0,"##");
    content.insert(2,ch);

    if(TCP[0].ServerOpen==true)
    {
            if(TCP[0].isConnected){
                 tcpSocket1->write(content.toAscii());
                 tcpSocket1->flush();
            }

    }
    if(TCP[1].ServerOpen==true)
    {
            if(TCP[1].isConnected){
                 tcpSocket2->write(content.toAscii());
                 tcpSocket2->flush();
            }

    }
    if(TCP[2].ServerOpen==true)
    {
            if(TCP[2].isConnected){
                 tcpSocket3->write(content.toAscii());
                 tcpSocket3->flush();
            }

    }
    if(TCP[3].ServerOpen==true)
    {
            if(TCP[3].isConnected){
                 tcpSocket4->write(content.toAscii());
                 tcpSocket4->flush();
            }

    }


    if(myApp::COM3ToServerOpen==true)
    {
           if(myCom[1]->isOpen()){
               myCom[1]->write(content.toAscii());
               myCom[1]->flush();
           }
    }

}

void myAPI::SendData_DoorRecord(int flag)
{
    QString sql,data;
    QSqlQuery query,query1;
    QString cardno,cardtype;
    QString QN=QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    QString dt;
    QString content;
    QByteArray ch;
    if(TCP[0].ServerOpen==true)
    {
            if(TCP[0].isConnected){
                sql=QString("select [GetTime],[CardType],[CardNo] from [DoorRecord]  where [T1SendTime] ='0'");
                query.exec(sql);
                while(query.next()){
                    dt=query.value(0).toString();
                    switch (query.value(1).toInt()) {
                    case 1:
                        cardtype="maintain";
                        break;
                    case 2:
                        cardtype="admin";
                        break;
                    case 3:
                        cardtype="enterprise";
                        break;
                    default:
                        cardtype="others";
                        break;
                    }
                    cardno=query.value(2).toString();
                    content="QN="+QN+";ST="+ST_to_Str[myApp::StType];
                    content+=";CN=3097;PW=123456;MN="+myApp::MN+";Flag="+QString::number(flag)+";CP=&&DataTime="+QString("%1").arg(dt);
                    content+=QString(";CardNo=%1;CardType=%2;Gate=ON").arg(cardno).arg(cardtype);
                    content+="&&";
                    ch.resize(4);
                    int len=content.size();
                    int crc=myHelper::CRC16_GB212(content.toLatin1().data(),len);
                    sprintf(ch.data(),"%04X",crc);
                    content.append(ch+"\r\n");
                    ch[0] = (len/1000)+'0';
                    ch[1] = (len%1000/100)+'0';
                    ch[2] = (len%100/10)+'0';
                    ch[3] = (len%10)+'0';
                    content.insert(0,"##");
                    content.insert(2,ch);
                 tcpSocket1->write(content.toAscii());
                 tcpSocket1->flush();
                 sql = QString("update [DoorRecord] set [T1SendTime]='1' where [GetTime]='%1'").arg(dt);
                 query1.exec(sql);//更新发送次数
            }

        }
    }

    if(TCP[1].ServerOpen==true)
    {
            if(TCP[1].isConnected){
                sql=QString("select [GetTime],[CardType],[CardNo] from [DoorRecord]  where [T2SendTime] ='0'");
                query.exec(sql);
                while(query.next()){
                    dt=query.value(0).toString();
                    switch (query.value(1).toInt()) {
                    case 1:
                        cardtype="maintain";
                        break;
                    case 2:
                        cardtype="admin";
                        break;
                    case 3:
                        cardtype="enterprise";
                        break;
                    default:
                        cardtype="others";
                        break;
                    }
                    cardno=query.value(2).toString();
                    content="QN="+QN+";ST="+ST_to_Str[myApp::StType];
                    content+=";CN=3097;PW=123456;MN="+myApp::MN+";Flag="+QString::number(flag)+";CP=&&DataTime="+QString("%1").arg(dt);
                    content+=QString(";CardNo=%1;CardType=%2;Gate=ON").arg(cardno).arg(cardtype);
                    content+="&&";
                    ch.resize(4);
                    int len=content.size();
                    int crc=myHelper::CRC16_GB212(content.toLatin1().data(),len);
                    sprintf(ch.data(),"%04X",crc);
                    content.append(ch+"\r\n");
                    ch[0] = (len/1000)+'0';
                    ch[1] = (len%1000/100)+'0';
                    ch[2] = (len%100/10)+'0';
                    ch[3] = (len%10)+'0';
                    content.insert(0,"##");
                    content.insert(2,ch);
                 tcpSocket2->write(content.toAscii());
                 tcpSocket2->flush();
                 sql = QString("update [DoorRecord] set [T2SendTime]='1' where [GetTime]='%1'").arg(dt);
                 query1.exec(sql);//更新发送次数
            }

        }
    }
    if(TCP[2].ServerOpen==true)
    {
            if(TCP[2].isConnected){
                sql=QString("select [GetTime],[CardType],[CardNo] from [DoorRecord]  where [T3SendTime] ='0'");
                query.exec(sql);
                while(query.next()){
                    dt=query.value(0).toString();
                    switch (query.value(1).toInt()) {
                    case 1:
                        cardtype="maintain";
                        break;
                    case 2:
                        cardtype="admin";
                        break;
                    case 3:
                        cardtype="enterprise";
                        break;
                    default:
                        cardtype="others";
                        break;
                    }
                    cardno=query.value(2).toString();
                    content="QN="+QN+";ST="+ST_to_Str[myApp::StType];
                    content+=";CN=3097;PW=123456;MN="+myApp::MN+";Flag="+QString::number(flag)+";CP=&&DataTime="+QString("%1").arg(dt);
                    content+=QString(";CardNo=%1;CardType=%2;Gate=ON").arg(cardno).arg(cardtype);
                    content+="&&";
                    ch.resize(4);
                    int len=content.size();
                    int crc=myHelper::CRC16_GB212(content.toLatin1().data(),len);
                    sprintf(ch.data(),"%04X",crc);
                    content.append(ch+"\r\n");
                    ch[0] = (len/1000)+'0';
                    ch[1] = (len%1000/100)+'0';
                    ch[2] = (len%100/10)+'0';
                    ch[3] = (len%10)+'0';
                    content.insert(0,"##");
                    content.insert(2,ch);
                 tcpSocket3->write(content.toAscii());
                 tcpSocket3->flush();
                 sql = QString("update [DoorRecord] set [T3SendTime]='1' where [GetTime]='%1'").arg(dt);
                 query1.exec(sql);//更新发送次数
            }

        }
    }
    if(TCP[3].ServerOpen==true)
    {
            if(TCP[3].isConnected){
                sql=QString("select [GetTime],[CardType],[CardNo] from [DoorRecord]  where [T4SendTime] ='0'");
                query.exec(sql);
                while(query.next()){
                    dt=query.value(0).toString();
                    switch (query.value(1).toInt()) {
                    case 1:
                        cardtype="maintain";
                        break;
                    case 2:
                        cardtype="admin";
                        break;
                    case 3:
                        cardtype="enterprise";
                        break;
                    default:
                        cardtype="others";
                        break;
                    }
                    cardno=query.value(2).toString();
                    content="QN="+QN+";ST="+ST_to_Str[myApp::StType];
                    content+=";CN=3097;PW=123456;MN="+myApp::MN+";Flag="+QString::number(flag)+";CP=&&DataTime="+QString("%1").arg(dt);
                    content+=QString(";CardNo=%1;CardType=%2;Gate=ON").arg(cardno).arg(cardtype);
                    content+="&&";
                    ch.resize(4);
                    int len=content.size();
                    int crc=myHelper::CRC16_GB212(content.toLatin1().data(),len);
                    sprintf(ch.data(),"%04X",crc);
                    content.append(ch+"\r\n");
                    ch[0] = (len/1000)+'0';
                    ch[1] = (len%1000/100)+'0';
                    ch[2] = (len%100/10)+'0';
                    ch[3] = (len%10)+'0';
                    content.insert(0,"##");
                    content.insert(2,ch);
                 tcpSocket4->write(content.toAscii());
                 tcpSocket4->flush();
                 sql = QString("update [DoorRecord] set [T4SendTime]='1' where [GetTime]='%1'").arg(dt);
                 query1.exec(sql);//更新发送次数
            }

        }
    }


        if(myApp::COM3ToServerOpen==true)
        {


               if(myCom[1]->isOpen()){
                   sql=QString("select [GetTime],[CardType],[CardNo] from [DoorRecord]  where [T5SendTime] ='0'");
                   query.exec(sql);
                   while(query.next()){
                       dt=query.value(0).toString();
                       switch (query.value(1).toInt()) {
                       case 1:
                           cardtype="maintain";
                           break;
                       case 2:
                           cardtype="admin";
                           break;
                       case 3:
                           cardtype="enterprise";
                           break;
                       default:
                           cardtype="others";
                           break;
                       }
                       cardno=query.value(2).toString();
                       content="QN="+QN+";ST="+ST_to_Str[myApp::StType];
                       content+=";CN=3097;PW=123456;MN="+myApp::MN+";Flag="+QString::number(flag)+";CP=&&DataTime="+QString("%1").arg(dt);
                       content+=QString(";CardNo=%1;CardType=%2;Gate=ON").arg(cardno).arg(cardtype);
                       content+="&&";
                       ch.resize(4);
                       int len=content.size();
                       int crc=myHelper::CRC16_GB212(content.toLatin1().data(),len);
                       sprintf(ch.data(),"%04X",crc);
                       content.append(ch+"\r\n");
                       ch[0] = (len/1000)+'0';
                       ch[1] = (len%1000/100)+'0';
                       ch[2] = (len%100/10)+'0';
                       ch[3] = (len%10)+'0';
                       content.insert(0,"##");
                       content.insert(2,ch);
                       myCom[1]->write(content.toAscii());
                       myCom[1]->flush();
                       sql = QString("update [DoorRecord] set [T5SendTime]='1' where [GetTime]='%1'").arg(dt);
                       query1.exec(sql);//更新发送次数
               }


        }

    }




}

void myAPI::SendData_Master(int CN,int flag)
{
    QString sql,data;
    QSqlQuery query,query1;

    if(TCP[0].ServerOpen==true)
    {
        if(flag&0x01)
        {
            sql=QString("select [QN],[Content],[Target1_SendTimes] from [MessageSend] where [CN]='%1' and [Target1_SendTimes]<'%2' and [Target1_IsRespond]='false'")
                    .arg(CN).arg(myApp::ReCount);
        }else{
            sql=QString("select [QN],[Content], [Target1_SendTimes] from [MessageSend] where [CN]='%1' and [Target1_SendTimes]<'1'")
                    .arg(CN);
        }
        query.exec(sql);
        if(query.first())
        {
            if(TCP[0].isConnected){
                 tcpSocket1->write(query.value(1).toByteArray());
                 tcpSocket1->flush();
                 sql = QString("update [MessageSend] set [Target1_SendTimes]='%1' where [QN]='%2'")
                         .arg(query.value(2).toInt()+1).arg(query.value(0).toString());
                 query1.exec(sql);//更新发送次数
                 data=QString("Tx[%1:%2]:%3").arg(TCP[0].ServerIP).arg(TCP[0].ServerPort).arg(query.value(1).toString());
                 InsertList(data);

            }else{
                if(CN==2011||CN==2051){//实时数据和分钟数据不自动补发
                    sql = QString("update [MessageSend] set [Target1_SendTimes]='%1' where [QN]='%2'")
                            .arg(query.value(2).toInt()+1).arg(query.value(0).toString());
                    query1.exec(sql);//更新发送次数
                }
            }

        }
    }

    if(TCP[1].ServerOpen==true)
    {
        if(flag&0x01)
        {
            sql=QString("select [QN],[Content],[Target2_SendTimes] from [MessageSend] where [CN]='%1' and [Target2_SendTimes]<'%2' and [Target2_IsRespond]='false'")
                    .arg(CN).arg(myApp::ReCount);
        }else{
            sql=QString("select [QN],[Content],[Target2_SendTimes] from [MessageSend] where [CN]='%1' and [Target2_SendTimes]<'1'")
                    .arg(CN);
        }
        query.exec(sql);
        if(query.first())
        {
/////////////////针对服务端防火墙长时间空闲自动断开专用///////////////////////////
//            int result_va=0;
//            result_va= tcpSocket2->write("##12342345465765784\r\n");//加一个心跳包

//            qDebug()<<QString("result_out=%1").arg(result_va);
//            sleep(25);

//            if(TCP[1].isConnected==false)sleep(10);

            if(TCP[1].isConnected){

                 tcpSocket2->write(query.value(1).toByteArray());
                 tcpSocket2->flush();
                 sql = QString("update [MessageSend] set [Target2_SendTimes]='%1' where [QN]='%2'")
                         .arg(query.value(2).toInt()+1).arg(query.value(0).toString());
                 query1.exec(sql);//更新发送次数
                 data=QString("Tx[%1:%2]:%3").arg(TCP[1].ServerIP).arg(TCP[1].ServerPort).arg(query.value(1).toString());
                 InsertList(data);

            }else{
                if(CN==2011||CN==2021||CN==2051){
                    sql = QString("update [MessageSend] set [Target2_SendTimes]='%1' where [QN]='%2'")
                            .arg(query.value(2).toInt()+1).arg(query.value(0).toString());
                    query1.exec(sql);//更新发送次数
                }
            }
        }
    }

    if(TCP[2].ServerOpen==true)
    {
        if(flag&0x01)
        {
            sql=QString("select [QN],[Content],[Target3_SendTimes] from [MessageSend] where [CN]='%1' and [Target3_SendTimes]<'%2' and [Target3_IsRespond]='false'")
                    .arg(CN).arg(myApp::ReCount);
        }else{
            sql=QString("select [QN],[Content],[Target3_SendTimes] from [MessageSend] where [CN]='%1' and [Target3_SendTimes]<'1'")
                    .arg(CN);
        }
        query.exec(sql);
        if(query.first())
        {
            if(TCP[2].isConnected){
                 tcpSocket3->write(query.value(1).toByteArray());
                 tcpSocket3->flush();
                 sql = QString("update [MessageSend] set [Target3_SendTimes]='%1' where [QN]='%2'")
                         .arg(query.value(2).toInt()+1).arg(query.value(0).toString());
                 query1.exec(sql);//更新发送次数
                 data=QString("Tx[%1:%2]:%3").arg(TCP[2].ServerIP).arg(TCP[2].ServerPort).arg(query.value(1).toString());
                 InsertList(data);

            }else{
                if(CN==2011||CN==2021||CN==2051){
                    sql = QString("update [MessageSend] set [Target3_SendTimes]='%1' where [QN]='%2'")
                            .arg(query.value(2).toInt()+1).arg(query.value(0).toString());
                    query1.exec(sql);//更新发送次数
                }
            }
        }
    }

    if(TCP[3].ServerOpen==true)
    {
        if(flag&0x01)
        {
            sql=QString("select [QN],[Content],[Target4_SendTimes] from [MessageSend] where [CN]='%1' and [Target4_SendTimes]<'%2' and [Target4_IsRespond]='false'")
                    .arg(CN).arg(myApp::ReCount);
        }else{
            sql=QString("select [QN],[Content],[Target4_SendTimes] from [MessageSend] where [CN]='%1' and [Target4_SendTimes]<'1'")
                    .arg(CN);
        }
        query.exec(sql);
        if(query.first())
        {
            if(TCP[3].isConnected){
                 tcpSocket4->write(query.value(1).toByteArray());
                 tcpSocket4->flush();
                 sql = QString("update [MessageSend] set [Target4_SendTimes]='%1' where [QN]='%2'")
                         .arg(query.value(2).toInt()+1).arg(query.value(0).toString());
                 query1.exec(sql);//更新发送次数
                 data=QString("Tx[%1:%2]:%3").arg(TCP[3].ServerIP).arg(TCP[3].ServerPort).arg(query.value(1).toString());
                 InsertList(data);

            }else{
                if(CN==2011||CN==2021||CN==2051){
                    sql = QString("update [MessageSend] set [Target4_SendTimes]='%1' where [QN]='%2'")
                            .arg(query.value(2).toInt()+1).arg(query.value(0).toString());
                    query1.exec(sql);//更新发送次数
                }
            }
        }
    }

    if(myApp::COM3ToServerOpen==true)
    {
        if(flag&0x01)
        {
            sql=QString("select [QN],[Content],[Target5_SendTimes] from [MessageSend] where [CN]='%1' and [Target5_SendTimes]<'%2' and [Target5_IsRespond]='false'")
                    .arg(CN).arg(myApp::ReCount);
        }else{
            sql=QString("select [QN],[Content],[Target5_SendTimes] from [MessageSend] where [CN]='%1' and [Target5_SendTimes]<'1'")
                    .arg(CN);
        }
        query.exec(sql);
        if(query.first())
        {
           if(myCom[1]->isOpen()){
               myCom[1]->write(query.value(1).toByteArray());
               myCom[1]->flush();
               sql = QString("update [MessageSend] set [Target5_SendTimes]='%1' where [QN]='%2'")
                       .arg(query.value(2).toInt()+1).arg(query.value(0).toString());
               query1.exec(sql);//更新发送次数
               data=QString("Tx[COM3]:%1").arg(query.value(1).toString());
               InsertList(data);

           }else{
                if(CN==2011||CN==2021||CN==2051){
                    sql = QString("update [MessageSend] set [Target5_SendTimes]='%1' where [QN]='%2'")
                            .arg(query.value(2).toInt()+1).arg(query.value(0).toString());
                    query1.exec(sql);//更新发送次数
                }
           }
        }
    }

}
//**************************************************************************************
//模拟采集电压转成实际值
double myAPI::AnalogConvert(double adValue,double RangeUp,double RangeLow,QString Signal)
{
    double realValue=0;
    //采样电阻200欧姆（认证使用）
//    if(Signal=="4-20mA")
//    {
//        realValue=(RangeUp-RangeLow)*(adValue*10-8)/32+RangeLow;
//    }
//    else if(Signal=="0-20mA"){
//        realValue=(RangeUp-RangeLow)*(adValue*10-0)/40+RangeLow;
//    }

//采样电阻100欧姆
    if(Signal=="4-20mA")
    {
        realValue=(RangeUp-RangeLow)*(adValue*10-4)/16+RangeLow;
    }
    else if(Signal=="0-20mA"){
        realValue=(RangeUp-RangeLow)*(adValue*10-0)/20+RangeLow;
    }
    else if(Signal=="1-5V"){
        realValue=(RangeUp-RangeLow)*(adValue-1)/4+RangeLow;
    }
    else if(Signal=="0-5V"){
        realValue=(RangeUp-RangeLow)*(adValue-0)/5+RangeLow;
    }


    if(realValue<RangeLow){
        realValue=RangeLow;
    }
    if(realValue>RangeUp){
        realValue=RangeUp;
    }

    return realValue;
}

//**************************************************************************************
//GB212协议
void myAPI::Protocol_1()
{
    int  tt=0;
    QByteArray temp;
    QByteArray readbuf;
    do{
        temp=myCom[1]->readAll();
        readbuf+=temp;
        if(temp.size()==0)tt++;
        else tt=0;
        if(tt>COM[1].Timeout/20)break;
        usleep(20000);
    }while(!readbuf.endsWith("\r\n"));

    if(tt<=COM[1].Timeout/20&&readbuf.size()>0){
        qDebug()<<QString("COM3 received:%1").arg(readbuf.data());
        a.messageProc(readbuf,myCom[1],NULL);
    }
}


//天泽VOCs
void myAPI::Protocol_2(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    uchar s[4];
    float f1,f2;
    QByteArray readbuf;
    QByteArray sendbuf;

    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x10;
    int check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    myCom[port]->write(sendbuf);
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    if(readbuf.length()>=37){
        qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
        s[0]=readbuf[6];
        s[1]=readbuf[5];
        s[2]=readbuf[4];
        s[3]=readbuf[3];
        f1=*(float *)s;//采集浓度,低字节前

        s[0]=readbuf[34];
        s[1]=readbuf[33];
        s[2]=readbuf[32];
        s[3]=readbuf[31];
        f2=*(float *)s;//气体系数
        flag='N';
        rtd=f1*f2;
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}

//声级计
void myAPI::Protocol_3(int port,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf="AWA0";

    myCom[port]->write(sendbuf);
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf;
    QString str="([0-9|.]+)dBA,";
    QRegExp Ex;
    Ex.setPattern(str);
    if(Ex.indexIn(readbuf) != -1){
        rtd=Ex.cap(1).toFloat();
        flag='N';
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}

//风速
void myAPI::Protocol_4(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;

    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x02;
    int check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    myCom[port]->write(sendbuf);
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
    if(readbuf.length()>=9){
        if(myHelper::CRC16_Modbus(readbuf.data(),7)==(readbuf[7]+readbuf[8]*256))
        {
            rtd=(readbuf[3]*256+readbuf[4])*0.1;
            flag='N';
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}

//风向
void myAPI::Protocol_5(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;

    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x02;
    int check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    myCom[port]->write(sendbuf);
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
    if(readbuf.length()>=9){
        if(myHelper::CRC16_Modbus(readbuf.data(),7)==(readbuf[7]+readbuf[8]*256))
        {
            rtd=readbuf[5]*256+readbuf[6];
            flag='N';
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}

//ES-642浓度
void myAPI::Protocol_6(int port,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    int  tt=0;
    QByteArray temp;
    QByteArray readbuf;
    do{
        temp=myCom[port]->readAll();
        readbuf+=temp;
        if(temp.size()==0)tt++;
        else tt=0;
        if(tt>COM[port].Timeout/20)break;
        usleep(20000);
    }while(!readbuf.endsWith("\r\n"));

    if(tt<=COM[port].Timeout/20&&readbuf.size()>0){
        qDebug()<<QString("COM%1 received:%2").arg(port+2).arg(readbuf.data());
        int sum_r=readbuf.right(7).left(5).toInt();
        int sum_c=myHelper::SUM(readbuf.data(),readbuf.length()-8);
        if(sum_r==sum_c){
            rtd=readbuf.split(',')[0].toDouble();
            flag='N';
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}
//ES-642温度
void myAPI::Protocol_7(int port,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    int  tt=0;
    QByteArray temp;
    QByteArray readbuf;

    do{
        temp=myCom[port]->readAll();
        readbuf+=temp;
        if(temp.size()==0)tt++;
        else tt=0;
        if(tt>COM[port].Timeout/20)break;
        usleep(20000);
    }while(!readbuf.endsWith("\r\n"));

    if(tt<=COM[port].Timeout/20&&readbuf.size()>0){
        qDebug()<<QString("COM%1 received:%2").arg(port+2).arg(readbuf.data());
        int sum_r=readbuf.right(7).left(5).toInt();
        int sum_c=myHelper::SUM(readbuf.data(),readbuf.length()-8);
        if(sum_r==sum_c){
            rtd=readbuf.split(',')[2].toDouble();
            flag='N';
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}

//ES-642湿度
void myAPI::Protocol_8(int port,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    int tt=0;
    QByteArray temp;
    QByteArray readbuf;
    do{
        temp=myCom[port]->readAll();
        readbuf+=temp;
        if(temp.size()==0)tt++;
        else tt=0;
        if(tt>COM[port].Timeout/20)break;
        usleep(20000);
    }while(!readbuf.endsWith("\r\n"));
//需要检测下readbuf的长度
    if(tt<=COM[port].Timeout/20&&readbuf.size()>0){
        qDebug()<<QString("COM%1 received:%2").arg(port+2).arg(readbuf.data());
        int sum_r=readbuf.right(7).left(5).toInt();
        int sum_c=myHelper::SUM(readbuf.data(),readbuf.length()-8);
        if(sum_r==sum_c){
            rtd=readbuf.split(',')[3].toDouble();
            flag='N';
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);

}

//ES-642气压
void myAPI::Protocol_9(int port,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    int  tt=0;
    QByteArray temp;
    QByteArray readbuf;
    do{
        temp=myCom[port]->readAll();
        readbuf+=temp;
        if(temp.size()==0)tt++;
        else tt=0;
        if(tt>COM[port].Timeout/20)break;
        usleep(20000);
    }while(!readbuf.endsWith("\r\n"));
//需要检测readbuf长度
    if(tt<=COM[port].Timeout/20&&readbuf.size()>0){
        qDebug()<<QString("COM%1 received:%2").arg(port+2).arg(readbuf.data());
        int sum_r=readbuf.right(7).left(5).toInt();
        int sum_c=myHelper::SUM(readbuf.data(),readbuf.length()-8);
        if(sum_r==sum_c){
            rtd=readbuf.split(',')[4].toDouble();
            flag='N';
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}

//经度
void myAPI::Protocol_10(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf=QString("##Add=%1;FC=1;CRC=").arg(Address).toAscii();
    QByteArray ch;
    ch.resize(4);

    int len=sendbuf.size();
    int crc=myHelper::CRC16_GB212(&sendbuf.data()[2],len-2);
    sprintf(ch.data(),"%04X",crc);
    sendbuf.append(ch+"\r\n");
    myCom[port]->write(sendbuf);
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf;
    if(readbuf.endsWith("\r\n")){
        QRegExp Ex;
        Ex.setPattern("Lot=([0-9|.]+)");
        if(Ex.indexIn(readbuf) != -1){
            double f1=myHelper::Str_To_Double(Ex.cap(1).toLatin1().data());
            int f2=(int)(f1/100);
            rtd=f2+(f1-f2*100)/60;
            flag='N';
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}

//纬度
void myAPI::Protocol_11(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf=QString("##Add=%1;FC=1;CRC=").arg(Address).toAscii();
    QByteArray ch;
    ch.resize(4);

    int len=sendbuf.size();
    int crc=myHelper::CRC16_GB212(&sendbuf.data()[2],len-2);
    sprintf(ch.data(),"%04X",crc);
    sendbuf.append(ch+"\r\n");
    myCom[port]->write(sendbuf);
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf;
    if(readbuf.endsWith("\r\n")){
        QRegExp Ex;
        Ex.setPattern("Lat=([0-9|.]+)");
        if(Ex.indexIn(readbuf) != -1){
            double f1=myHelper::Str_To_Double(Ex.cap(1).toLatin1().data());
            int f2=(int)(f1/100);
            rtd=f2+(f1-f2*100)/60;
            flag='N';
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}

//瑾熙流量计
void myAPI::Protocol_12(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    double total=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check;
    int len;
    QByteArray ch;
    ch.resize(4);
    char s[4];

    sendbuf.resize(8);

    sendbuf[0]=Address;//读取瞬时流量
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x02;
    sendbuf[6]=0xC4;
    sendbuf[7]=0x0B;
    myCom[port]->write(sendbuf);
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex();
    if(readbuf.length()>=9){
        len=readbuf.length();
        check = myHelper::CRC16_Modbus(&readbuf.data()[len-9],7);
        qDebug()<<QString::number(check,16).toUpper();
        if((char)(check)==readbuf[len-2] && (char)(check>>8)==readbuf[len-1])
        {
            s[0]=readbuf[len-3];
            s[1]=readbuf[len-4];
            s[2]=readbuf[len-5];
            s[3]=readbuf[len-6];
            rtd=*(float *)s;
            flag='N';
        }
    }

    sendbuf[0]=Address;//读取正向总量
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x02;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    myCom[port]->write(sendbuf);
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex();
    if(readbuf.length()>=9){
        len=readbuf.length();
        check = myHelper::CRC16_Modbus(&readbuf.data()[len-9],7);
        qDebug()<<QString::number(check,16).toUpper();
        if((char)(check)==readbuf[len-2] && (char)(check>>8)==readbuf[len-1])
        {
            total=readbuf[len-6]*16777216+readbuf[len-5]*65536+readbuf[len-4]*256+readbuf[len-3];
        }
    }
//    qDebug()<<QString("Rtd:%1 Total:%2").arg(rtd).arg(total);
    CacheDataProc(rtd,total,flag,Dec,Name,Code,Unit);
}

//光华流量计
void myAPI::Protocol_13(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    double total=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
//    QByteArray ch;
//    ch.resize(4);
    int d0,d1,d2,d3,d4;
    usleep(1000000);
    sendbuf.resize(4);
    sendbuf[0]=0x2A;//读取正向总量
    sendbuf[1]=Address;
    sendbuf[2]=0x04;
    sendbuf[3]=0x2E;
    myCom[port]->readAll();
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex();
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex();
    if(readbuf.length()>=10){
        if(0xAA==readbuf[9]){
        d0 = myHelper::to_natural_binary(readbuf[2]);
        d1 = myHelper::to_natural_binary(readbuf[3]);
        d2 = myHelper::to_natural_binary(readbuf[4]);
        d3 = myHelper::to_natural_binary(readbuf[5]);
        d4 = myHelper::to_natural_binary(readbuf[6]);
        total=100000000*d4+1000000*d3+10000*d2+100*d1+d0;
        flag='N';
    }
    }
    usleep(1000000);
    sendbuf[0]=0x2A;//读取瞬时流量
    sendbuf[1]=Address;
    sendbuf[2]=0x00;
    sendbuf[3]=0x2E;
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex();
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex();
    if(readbuf.length()>=10){
        if(0xAA==readbuf[9]){
        d0 = myHelper::to_natural_binary(readbuf[2]);
        d1 = myHelper::to_natural_binary(readbuf[3]);
        d2 = myHelper::to_natural_binary(readbuf[4]);
        rtd = 10000*d2+100*d1+d0;
        rtd = rtd*pow(10,readbuf[5]-5);//以10为底的temp次幂
        if(flag=="N")CacheDataProc(rtd,total,flag,Dec,Name,Code,Unit);
        }
    }
    usleep(1000000);

//    qDebug()<<QString("Rtd:%1 Total:%2").arg(rtd).arg(total);
//    double total=0;
//    QString flag="D";
//    QByteArray readbuf;
//    QByteArray sendbuf;
//    QByteArray ch;
//    ch.resize(4);
//    int d0,d1,d2,d3,d4;

//    sendbuf.resize(4);
//    usleep(1000000);
//    sendbuf[0]=0x2A;//读取正向总量
//    sendbuf[1]=Address;
//    sendbuf[2]=0x04;
//    sendbuf[3]=0x2E;
//    myCom[port]->write(sendbuf);
//    usleep(COM[port].Timeout*1000);
//    readbuf=myCom[port]->readAll();
//    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex();
//    if(readbuf.length()>=10&&0xAA==readbuf[9]){
//        d0 = myHelper::to_natural_binary(readbuf[2]);
//        d1 = myHelper::to_natural_binary(readbuf[3]);
//        d2 = myHelper::to_natural_binary(readbuf[4]);
//        d3 = myHelper::to_natural_binary(readbuf[5]);
//        d4 = myHelper::to_natural_binary(readbuf[6]);
//        total=100000000*d4+1000000*d3+10000*d2+100*d1+d0;
//    }

//    usleep(1000000);
//    sendbuf[0]=0x2A;//读取瞬时流量
//    sendbuf[1]=Address;
//    sendbuf[2]=0x00;
//    sendbuf[3]=0x2E;
//    myCom[port]->write(sendbuf);
//    usleep(COM[port].Timeout*1000);
//    readbuf=myCom[port]->readAll();
//    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex();
//    if(readbuf.length()>=10&&0xAA==readbuf[9]){
//        d0 = myHelper::to_natural_binary(readbuf[2]);
//        d1 = myHelper::to_natural_binary(readbuf[3]);
//        d2 = myHelper::to_natural_binary(readbuf[4]);
//        rtd = 10000*d2+100*d1+d0;
//        rtd = rtd*pow(10,readbuf[5]-5);//以10为底的temp次幂
//        flag="N";
//    }
//   qDebug()<<QString("Rtd:%1 Total:%2").arg(rtd).arg(total);
//    CacheDataProc(rtd,total,flag,Dec,Name,Code,Unit);
//    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);


//    usleep(1000000);
}

//TOC-4100CJ
void myAPI::Protocol_14(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QString rtd_tmp;
    QByteArray readbuf;
    QByteArray sendbuf=QString("##000R%1***").arg(Address).toAscii();

    myCom[port]->write(sendbuf);
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf;
    //需要检测readbuf长度

    if(readbuf.startsWith("&&&")&&readbuf.endsWith("***")&&0x30==readbuf[8]){
        rtd_tmp=readbuf.mid(22,6);
        rtd=rtd_tmp.toDouble();
        flag='N';
        CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
    }
}

//承德流量计
void myAPI::Protocol_15(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
	static double total_prev = -255;
	
	int 	retry_cnt = 0;
	uchar 	warn_flag = 0;
    double rtd=0;
    double total=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    uchar check=0;
    char s[4];
    sendbuf.resize(14);
    sendbuf[0]=0x68;
    sendbuf[1]=(char)(Address);
    sendbuf[2]=(char)(Address>>8);
    sendbuf[3]=(char)(Address>>16);
    sendbuf[4]=(char)(Address>>24);
    sendbuf[5]=0x00;
    sendbuf[6]=0x00;
    sendbuf[7]=0x68;
    sendbuf[8]=0x01;
    sendbuf[9]=0x02;
    sendbuf[10]=0x18;
    sendbuf[11]=0xC0;
    check=0;
    for(int j=0;j<12;j++)
    {
        check+=sendbuf.data()[j];
    }
    sendbuf[12]=(uchar)check;
    sendbuf[13]=0x16;

	for(retry_cnt = 0; retry_cnt < DOWN_RETRY_CNT; retry_cnt++){
	    myCom[port]->write(sendbuf);
	    myCom[port]->flush();
	    qDebug()<<QString("COM%1 send_rtd:").arg(port+2)<<sendbuf.toHex();
	    sleep(4);
	    readbuf=myCom[port]->readAll();
	    qDebug()<<QString("COM%1 received_rtd:").arg(port+2)<<readbuf.toHex();
	    if(readbuf.length()==18){
	        if(readbuf.endsWith(0x16)&&readbuf.startsWith(0x68)){
	            check=0;
	            for(int j=0;j<16;j++)
	            {
	                check+=readbuf.data()[j];
	            }
	            check=(uchar)check;
	            if(check==readbuf[16]){
	                s[0]=readbuf[12];
	                s[1]=readbuf[13];
	                s[2]=readbuf[14];
	                s[3]=readbuf[15];
	                rtd=*(float *)s;
					if(rtd < 0 && 0 == warn_flag){ //若第一次出现负数 重新发报文获取数据
						warn_flag = 1;
						retry_cnt = 0;
						continue;
					}
					if(rtd < 0 && rtd > -3.0){ //屏蔽仪表没有校准好 出现的小负数
						rtd = 0;
					}
	                flag="N";
	                if(rtd>3) myApp::FLOW_FLG=1;
	                else {
	                    myApp::FLOW_FLG=0;
	                }
					break;
	            }

	        }
	    }
	}
	
    sendbuf[0]=0x68;
    sendbuf[1]=(char)(Address);
    sendbuf[2]=(char)(Address>>8);
    sendbuf[3]=(char)(Address>>16);
    sendbuf[4]=(char)(Address>>24);
    sendbuf[5]=0x00;
    sendbuf[6]=0x00;
    sendbuf[7]=0x68;
    sendbuf[8]=0x01;
    sendbuf[9]=0x02;
    sendbuf[10]=0x03;
    sendbuf[11]=0xC0;
    check=0;
    for(int j=0;j<12;j++)
    {
        check+=sendbuf.data()[j];
    }
    sendbuf[12]=(uchar)check;
    sendbuf[13]=0x16;
	
	warn_flag = 0;
	for(retry_cnt = 0; retry_cnt < DOWN_RETRY_CNT; retry_cnt++){
	    myCom[port]->write(sendbuf);//读取累积流量
	    myCom[port]->flush();
	    qDebug()<<QString("COM%1 send_total:").arg(port+2)<<sendbuf.toHex();
	    sleep(4);
	    readbuf = myCom[port]->readAll();
	    qDebug()<<QString("COM%1 received_tl:").arg(port+2)<<readbuf.toHex();
	    if(readbuf.length()==22){
	        if(readbuf.endsWith(0x16)&&readbuf.startsWith(0x68))
	        {
	            check=0;
	            for(int j=0;j<20;j++)
	            {
	                check+=readbuf.data()[j];
	            }
	            check=(uchar)check;
	            if(check==readbuf[20]){
	                total=  myHelper::to_natural_binary(readbuf[19])*100000000000LL+
	                            myHelper::to_natural_binary(readbuf[18])*1000000000+
	                            myHelper::to_natural_binary(readbuf[17])*10000000+
	                            myHelper::to_natural_binary(readbuf[16])*100000+
	                            myHelper::to_natural_binary(readbuf[15])*1000+
	                            myHelper::to_natural_binary(readbuf[14])*10+
	                            myHelper::to_natural_binary(readbuf[13])*0.1+
	                            myHelper::to_natural_binary(readbuf[12])*0.001;
	                if(flag=="N"){
						if(total < 0 || total>50000000){  //累计值量程
							continue;
						}
						if(-255 != total_prev){
							if(total < total_prev){ //比上一次小 直接过滤掉
								continue;
							}
							if(0 == warn_flag && total - total_prev > 1000){ 
								//第一次出现增量数据超过一定范围，重新获取数据
								warn_flag = 1;
								retry_cnt = 0;
								continue;
							}
						}						
						total_prev = total;
	                    CacheDataProc(rtd,total,flag,Dec,Name,Code,Unit);
	                }
					break;
	            }
	        }
	    }
	}
}


//SJFC-200苏净粉尘仪
void myAPI::Protocol_16(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    int32_t rtd_tmp=0;
    QString flag="D";

    QByteArray readbuf;
    QByteArray sendbuf;

    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x9C;
    sendbuf[3]=0x46;
    sendbuf[4]=0x00;
    sendbuf[5]=0x02;
    int check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    myCom[port]->write(sendbuf);
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    if(readbuf.length()>=9){
        qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),7);
                if((readbuf[7]==(char)(check&0xff))&&(readbuf[8]==(char)(check>>8)))
                {
                    rtd_tmp=readbuf[3];
                    rtd_tmp=rtd_tmp<<8;
                    rtd_tmp+=readbuf[4];
                    rtd_tmp=rtd_tmp<<8;
                    rtd_tmp+=readbuf[5];
                    rtd_tmp=rtd_tmp<<8;
                    rtd_tmp+=readbuf[6];
                    rtd=(float)rtd_tmp/10;                     //单位：ug/m3
                    flag='N';
                }
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}

//天泽温度
void myAPI::Protocol_17(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf=QString("##Add=%1;FC=1;CRC=").arg(Address).toAscii();
    QByteArray ch;
    ch.resize(4);

    int len=sendbuf.size();
    int crc=myHelper::CRC16_GB212(&sendbuf.data()[2],len-2);
    sprintf(ch.data(),"%04X",crc);
    sendbuf.append(ch+"\r\n");
    myCom[port]->write(sendbuf);
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf;
    if(readbuf.endsWith("\r\n")){
        QRegExp Ex;                             //
        Ex.setPattern("Temp=([0-9]+)");
        if(Ex.indexIn(readbuf) != -1){
            double f1=myHelper::Str_To_Double(Ex.cap(1).toLatin1().data());
            rtd=f1/100;
            flag='N';
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}
//天泽湿度
void myAPI::Protocol_18(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf=QString("##Add=%1;FC=1;CRC=").arg(Address).toAscii();
    QByteArray ch;
    ch.resize(4);

    int len=sendbuf.size();
    int crc=myHelper::CRC16_GB212(&sendbuf.data()[2],len-2);
    sprintf(ch.data(),"%04X",crc);
    sendbuf.append(ch+"\r\n");
    myCom[port]->write(sendbuf);
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf;
    if(readbuf.endsWith("\r\n")){
        QRegExp Ex;                             //
        Ex.setPattern("Hum=([0-9]+)");
        if(Ex.indexIn(readbuf) != -1){
            double f1=myHelper::Str_To_Double(Ex.cap(1).toLatin1().data());
            rtd=f1/100;
            flag='N';
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}
//天泽气压
void myAPI::Protocol_19(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf=QString("##Add=%1;FC=1;CRC=").arg(Address).toAscii();
    QByteArray ch;
    ch.resize(4);

    int len=sendbuf.size();
    int crc=myHelper::CRC16_GB212(&sendbuf.data()[2],len-2);
    sprintf(ch.data(),"%04X",crc);
    sendbuf.append(ch+"\r\n");
    myCom[port]->write(sendbuf);
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf;
    if(readbuf.endsWith("\r\n")){
        QRegExp Ex;                             //
        Ex.setPattern("Prs=([0-9]+)");
        if(Ex.indexIn(readbuf) != -1){
            double f1=myHelper::Str_To_Double(Ex.cap(1).toLatin1().data());
            rtd=f1/100;
            flag='N';
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}

//光华流量计-总量
void myAPI::Protocol_20(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    QByteArray ch;
    ch.resize(4);
    int d0,d1,d2,d3,d4;

    sendbuf.resize(4);

//    sendbuf[0]=0x2A;//读取瞬时流量
//    sendbuf[1]=Address;
//    sendbuf[2]=0x00;
//    sendbuf[3]=0x2E;
//    myCom[port]->write(sendbuf);
//    usleep(COM[port].Timeout*1000);
//    readbuf=myCom[port]->readAll();
//    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex();
//    if(readbuf.length()>=10&&0xAA==readbuf[9]){
//        d0 = myHelper::to_natural_binary(readbuf[2]);
//        d1 = myHelper::to_natural_binary(readbuf[3]);
//        d2 = myHelper::to_natural_binary(readbuf[4]);
//        rtd = 10000*d2+100*d1+d0;
//        rtd = rtd*pow(10,readbuf[5]-5);//以10为底的temp次幂
//        flag='N';
//    }
    usleep(1000000);
    sendbuf[0]=0x2A;//读取正向总量
    sendbuf[1]=Address;
    sendbuf[2]=0x04;
    sendbuf[3]=0x2E;
    myCom[port]->readAll();
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex();
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex();
    if(readbuf.length()>=10&&Address==readbuf[0]){
        d0 = myHelper::to_natural_binary(readbuf[2]);
        d1 = myHelper::to_natural_binary(readbuf[3]);
        d2 = myHelper::to_natural_binary(readbuf[4]);
        d3 = myHelper::to_natural_binary(readbuf[5]);
        d4 = myHelper::to_natural_binary(readbuf[6]);
        rtd=100000000*d4+1000000*d3+10000*d2+100*d1+d0;
        flag='N';
        CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
    }
    usleep(1000000);
//    qDebug()<<QString("Rtd:%1 Total:%2").arg(rtd).arg(total);
//    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}
//PH-P206
void myAPI::Protocol_21(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
   volatile char s[4];

    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x04;
    sendbuf[2]=0x00;
    sendbuf[3]=0x02;
    sendbuf[4]=0x00;
    sendbuf[5]=0x02;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    myCom[port]->write(sendbuf);
    sleep(1);
//    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
    if(readbuf.length()>=9){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),7);
                if((readbuf[7]==(char)(check&0xff))&&(readbuf[8]==(char)(check>>8)))
                {
                    s[0]=readbuf[4];
                    s[1]=readbuf[3];
                    s[2]=readbuf[6];
                    s[3]=readbuf[5];
                    rtd=*(float *)s;        //瞬时PH
                    flag='N';
                }
        }
    }

    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}

//明渠流量计
void myAPI::Protocol_22(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    double total=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
   volatile char s[4];

    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0x08;
    sendbuf[4]=0x00;
    sendbuf[5]=0x04;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    myCom[port]->write(sendbuf);
    sleep(1);
//    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
    if(readbuf.length()>=13){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),11);
                if((readbuf[11]==(char)(check&0xff))&&(readbuf[12]==(char)(check>>8)))
                {
                    s[0]=readbuf[6];
                    s[1]=readbuf[5];
                    s[2]=readbuf[4];
                    s[3]=readbuf[3];
                    rtd=*(float *)s;        //瞬时流量单位M3/H
                    s[0]=readbuf[10];
                    s[1]=readbuf[9];
                    s[2]=readbuf[8];
                    s[3]=readbuf[7];
                    total=*(float *)s;        //累计流量单位M3
                    flag='N';
                }
        }
    }

    CacheDataProc(rtd,total,flag,Dec,Name,Code,Unit);

}
//C10电导率
void myAPI::Protocol_23(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    char check=0;
    char ch_temp;
    volatile char s[4];
    sendbuf.resize(14);
    sendbuf[0]=0x40;
    sendbuf[1]=((Address >> 4) & 0x0f)+0x30;
    sendbuf[2]=(Address&0x0f)+0x30;
    sendbuf[3]='C';
    sendbuf[4]='8';
    sendbuf[5]='0';
    sendbuf[6]='0';
    sendbuf[7]='0';
    sendbuf[8]='0';
    sendbuf[9]='0';
    sendbuf[10]='8';
    check=myHelper::XORValid(&sendbuf.data()[1],10);
    ch_temp = (check  >> 4) & 0x0F;  //取高位数；
    if (ch_temp < 10)                       //低于10的数
        ch_temp = ch_temp  +  '0';
    else
        ch_temp = (ch_temp - 10 ) +  'A';   //不低于10的16进制数，如：A、B、C、D、E、F
    sendbuf[11]=ch_temp;
    ch_temp = check & 0x0F;  //取低位数；
    if (ch_temp < 10) ch_temp = ch_temp  +  '0';
    else ch_temp = (ch_temp - 10 )+  'A';
    sendbuf[12]=ch_temp;
    sendbuf[13]=0x0D;
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf;
    myCom[port]->write(sendbuf);
    sleep(1);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf;
    if(readbuf.length()>=24&&0x40==readbuf[0]){
        if(Address==(readbuf[1]-0x30)*16+readbuf[2]-0x30)
        {
            check = myHelper::XORValid(&readbuf.data()[1],readbuf.length()-4);
            ch_temp = (check  >> 4) & 0x0F;  //取高位数；
            if (ch_temp < 10)                       //低于10的数
                ch_temp = ch_temp  +  '0';
            else
                ch_temp = (ch_temp - 10 ) +  'A';   //不低于10的16进制数，如：A、B、C、D、E、F
            if(readbuf[readbuf.length()-3]!=ch_temp) return;
            ch_temp = check & 0x0F;  //取低位数；
            if (ch_temp < 10) ch_temp = ch_temp  +  '0';
            else ch_temp = (ch_temp - 10 )+  'A';
            if(readbuf[readbuf.length()-2]!=ch_temp) return;
//            qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf;
            s[0]=myHelper::HexStrValue(readbuf[11],readbuf[12]);
            s[1]=myHelper::HexStrValue(readbuf[9],readbuf[10]);
            s[2]=myHelper::HexStrValue(readbuf[7],readbuf[8]);
            s[3]=myHelper::HexStrValue(readbuf[5],readbuf[6]);
            rtd=*(float *)s;        //瞬时电导率
//            qDebug()<<QString("rtd=%1").arg(rtd);

            flag='N';
        }
    }

    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);


}

//锐泉COD
void myAPI::Protocol_24(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
   volatile char s[4];

    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x02;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
//    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    myCom[port]->write(sendbuf);
    sleep(1);
    readbuf=myCom[port]->readAll();
//    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
    if(readbuf.length()>=9){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),7);
                if((readbuf[7]==(char)(check&0xff))&&(readbuf[8]==(char)(check>>8)))
                {
                    s[0]=readbuf[4];
                    s[1]=readbuf[3];
                    s[2]=readbuf[6];
                    s[3]=readbuf[5];
                    rtd=*(float *)s;        //瞬时COD
                    flag='N';
                }
        }
    }

    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
    //添加COD状态检测 空闲状态         myApp::COD_Isok=true;

        if(myApp::COD_FLG&&myApp::COD_Isok==true){
            myApp::COD_FLG=0;
            myApp::COD_Isok=false;
            //添加COD取样指令
            sendbuf.resize(11);
            sendbuf[0]=Address;
            sendbuf[1]=0x10;
            sendbuf[2]=0x00;
            sendbuf[3]=0x33;
            sendbuf[4]=0x00;
            sendbuf[5]=0x01;
            sendbuf[6]=0x02;
            sendbuf[7]=0x00;
            sendbuf[8]=0x01;
            check = myHelper::CRC16_Modbus(sendbuf.data(),9);
            sendbuf[9]=(char)(check);
            sendbuf[10]=(char)(check>>8);
            myCom[port]->readAll();
            qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
            myCom[port]->write(sendbuf);
            myCom[port]->flush();
            sleep(1);
            myCom[port]->readAll();
        }

}

//门禁刷卡设备
void myAPI::Protocol_25(int port)
{
    QString sql;
    QSqlQuery query_memory(QSqlDatabase::database("memory",true));
    QSqlQuery query_memory1(QSqlDatabase::database("memory",true));
    QByteArray sendbuf;
    QByteArray readbuf;
    QByteArray ch;
    ch.resize(4);
    int len=0;
    int crc=0;
    readbuf=myCom[port]->readAll();
    sendbuf="##FC=5;CRC=7340\r\n";
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(2);
    readbuf=myCom[port]->readAll();
    if(readbuf.data()!=NULL){
        QRegExp Ex;
        QString cardtype;
        QString cardno;
        qDebug()<<readbuf;

        Ex.setPattern("CardType=([0-9]+)");
        if(Ex.indexIn(readbuf) != -1){
            cardtype=Ex.cap(1);
            Ex.setPattern("CardNO=([0-9A-Za-z]+)");
            if(Ex.indexIn(readbuf) != -1){
                cardno=Ex.cap(1);
                qDebug()<<QString("type=%1,cardno=%2").arg(cardtype,cardno);

                CacheCard(cardtype,cardno);
                myApp::Cardrecord_FLG=1;
            }
        }
    }
    QString temp;
    int  paranumber=0;
    sql="select count(*) from [CacheRtd] where [Flag]='N'";
    query_memory.exec(sql);
    if(query_memory.first())paranumber=query_memory.value(0).toInt();
    if(paranumber!=0){
        sql="select [Code],[Rtd],[Total] from [CacheRtd] where [Flag]='N'";
        query_memory1.exec(sql);
        temp=QString("FC=3;ParaNum=%1;").arg(paranumber);
        while(query_memory1.next()){
            QString code,rtd_value,total_value;
            QRegExp Ex1;
            Ex1.setPattern("(\\d+\\.?\\d*)");
            if(Ex1.indexIn(query_memory1.value(1).toString()) != -1){
                rtd_value=Ex1.cap(1);
                code=query_memory1.value(0).toString();
                if(code=="w00000"){
                    total_value=query_memory1.value(2).toString();
                    temp+=QString("%1-Rtd=%2;%3-TF=%4;").arg(code).arg(rtd_value).arg(code).arg(total_value);
                }
                else{
                    temp+=QString("%1-Rtd=%2;").arg(code).arg(rtd_value);
                }
            }
        }
        temp+="CRC=";
        QByteArray ch;
        ch.resize(4);
        int len=temp.size();
        int crc=myHelper::CRC16_GB212(temp.toLatin1().data(),len);
        sprintf(ch.data(),"%04X",crc);
        temp.append(ch+"\r\n");
        temp.insert(0,"##");
        qDebug()<<temp;
        myCom[port]->write(temp.toLatin1());
        myCom[port]->flush();
        sleep(2);
        myCom[port]->readAll();

    }

//门禁状态
    sendbuf="##FC=4;CRC=7640\r\n";
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(2);
    readbuf=myCom[port]->readAll();
    if(readbuf.data()!=NULL){
        QRegExp Ex;
        Ex.setPattern("Gate=([A-Z]+)");
        if(Ex.indexIn(readbuf) != -1){
            myApp::DoorStatus=Ex.cap(1);
        }
    }
    if(myApp::Door_FLG){
        //发送开门协议
        readbuf=myCom[port]->readAll();
        sendbuf="##FC=6;Gate=ON;CRC=8581\r\n";
        myCom[port]->write(sendbuf);
        myCom[port]->flush();
        sleep(2);
        readbuf=myCom[port]->readAll();
        if(readbuf.contains("FC=6;ExeRtn=1;")){
            //执行成功；
            myApp::Door_FLG=0;
        }
        else{
            //执行失败
            myApp::Door_FLG=1;
        }
    }

    //开卡
    if(myApp::Addcard_FLG){
        QString sendms;
        QString CardNo;
        QString CardType;
        QSqlQuery query,query1;

        sql="select [CardNo], [CardType],[SendCount] from CardNumber where [SendCount]='0'";
        query.exec(sql);

        while(query.next()){
         CardNo=query.value(0).toString();
         CardType=query.value(1).toString();
        myCom[port]->readAll();
        sendms=QString("##FC=1;CardType=%1;CardNO=%2;CRC=").arg(CardType).arg(CardNo);
        len=sendms.size();
        crc=myHelper::CRC16_GB212(&sendms.toLatin1().data()[2],len-2);
        sprintf(ch.data(),"%04X",crc);
        sendms.append(ch+"\r\n");
        myCom[port]->write(sendms.toLatin1());
        myCom[port]->flush();
        sleep(2);
        readbuf=myCom[port]->readAll();
        if(readbuf.contains("FC=1;ExeRtn=1;")){
//        if(readbuf=="##FC=1;ExeRtn=1;CRC=1100\r\n"){
            //执行成功；
            myApp::Addcard_FLG=0;
            sql = QString("update CardNumber set [SendCount]='1'where [CardNo]='%1'").arg(CardNo);
            query1.exec(sql);//更新处理标记
        }
        else{
            //执行失败
            myApp::Addcard_FLG=1;
        }
        }
    }
    //销卡
    if(myApp::Deletecard_FLG){
        QString sql2;
        QSqlQuery query2,query3;
        QString sendms2;
        QString CardNo2;
        QString CardType2;
        sql2=QString("select [CardNo], [CardType],[SendCount] from CardNumber where [Attribute]='delete'");
        query2.exec(sql2);
        while(query2.next()){
            CardNo2=query2.value(0).toString();
            CardType2=query2.value(1).toString();
            myCom[port]->readAll();
            sendms2=QString("##FC=2;CardType=%1;CardNO=%2;CRC=").arg(CardType2).arg(CardNo2);
            len=sendms2.size();
            crc=myHelper::CRC16_GB212(&sendms2.toLatin1().data()[2],len-2);
            sprintf(ch.data(),"%04X",crc);
            sendms2.append(ch+"\r\n");
            myCom[port]->write(sendms2.toLatin1());
            myCom[port]->flush();
            sleep(2);
            readbuf=myCom[port]->readAll();
            if(readbuf.contains("FC=2;ExeRtn=1;")){
                //执行成功；
                sql2=QString("delete from [CardNumber] where [Attribute]='delete' and [CardNo]='%1'").arg(CardNo2);
                qDebug()<<sql2;
                query3.exec(sql2);
                myApp::Deletecard_FLG=0;
            }
            else{
                //执行失败
                myApp::Deletecard_FLG=1;

            }
        }
    }





}

//哈希COD
void myAPI::Protocol_26(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    float rtd=0;
    double total=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
    char s[4];

    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x06;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(2);
//    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
    if(readbuf.length()>=17){
        if(Address==readbuf[0])
        {
            check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
            if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
            {
                s[0]=readbuf[4];
                s[1]=readbuf[3];
                s[2]=readbuf[6];
                s[3]=readbuf[5];
                rtd=*(float *)s;        //COD单位
                flag='N';
                CacheDataProc(rtd,total,flag,Dec,Name,Code,Unit);
            }
        }
    }
}

//哈希氨氮
void myAPI::Protocol_27(int port,int Address,int Dec,QString Name,QString Code,QString Unit,double alarm_max)
{
    float rtd=0;
    double total=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
    char s[4];

    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x02;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    myCom[port]->write(sendbuf);
    sleep(2);
//    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();

    if(readbuf.length()>=9){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
                if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
                {
                    s[0]=readbuf[4];
                    s[1]=readbuf[3];
                    s[2]=readbuf[6];
                    s[3]=readbuf[5];
                    rtd=*(float *)s;        //NH3-N单位
                    flag='N';
                    CacheDataProc(rtd,total,flag,Dec,Name,Code,Unit);
                    if(rtd>alarm_max)myApp::NH_Isok=false;
                    else myApp::NH_Isok=true;
                }
        }
    }
}

//KS-3200采样仪
void myAPI::Protocol_28(int port)
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
    int i=0;
   QString sendbuf;
   QByteArray readbuf;
   int current_bottle=0;
   int sample_capacity=400;
   myCom[port]->readAll();
   myCom[port]->write("DR@81HB");
   myCom[port]->flush();
   sleep(2);
   readbuf=myCom[port]->readAll();
//   qDebug()<<QString("COM%1 Re:").arg(port+2)<<readbuf;
   //采集到的数据存放到数据库待用
//   readbuf="DR@81030003000300030003000300030003000300030003000300030003000300030003000300030003000300030003000300HB";
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
       for(i=5;i<98;i=i+4){
            if(readbuf.mid(i,4).toInt()==0){
                current_bottle=(i-1)/4;
                break;
            }
            else{
                current_bottle=0;
            }

        }
       if(current_bottle==0&&myApp::SampleSyn==true)
       {
           myApp::SampleSyn=false;
           Insert_Message_SampleBottle(4,bottle_1,bottle_2,bottle_3,bottle_4,bottle_5,bottle_6,bottle_7,bottle_8,bottle_9,bottle_10,bottle_11,bottle_12,bottle_13,bottle_14,bottle_15,bottle_16,bottle_17,bottle_18,bottle_19,bottle_20,bottle_21,bottle_22,bottle_23,bottle_24);
       }

  }
       if(myApp::SampleSyn==true){   //采样同步
           myApp::SampleSyn=false;
               if(current_bottle!=0){
               sendbuf=QString("DR@35001040200550600000000000000000HB");
               myCom[port]->write(sendbuf.toAscii());
               myCom[port]->flush();
               delaymsec(10000);
               qDebug()<<myCom[port]->readAll();
                sendbuf=QString( "DR@1506012401%1%200000000000000000000002000000060HB").arg(sample_capacity,3,10,QChar('0')).arg(current_bottle,2,10,QChar('0'));

               myCom[port]->write(sendbuf.toAscii());
               qDebug()<<sendbuf;
                myCom[port]->flush();
                delaymsec(10000);
               qDebug()<<myCom[port]->readAll();
               myCom[port]->write("DR@8001HB");
               myCom[port]->flush();
               delaymsec(2000);
               qDebug()<<myCom[port]->readAll();
           }
       }

       if(myApp::SampleCmd==true){ //超标留样
           myApp::SampleCmd=false;
           if(current_bottle!=0){
           myCom[port]->write("DR@8002HB");
           myCom[port]->flush();
           sleep(1);
          qDebug()<<myCom[port]->readAll();
          delaymsec(300000);

           myCom[port]->readAll();
           myCom[port]->write("DR@81HB");
           myCom[port]->flush();
           delaymsec(2000);
           readbuf=myCom[port]->readAll();
        //   readbuf="DR@81030003000300030003000300030003000300030003000300030003000300030003000300030003000300030003000300HB";
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
                   Insert_Message_SampleBottle(4,bottle_1,bottle_2,bottle_3,bottle_4,bottle_5,bottle_6,bottle_7,bottle_8,bottle_9,bottle_10,bottle_11,bottle_12,bottle_13,bottle_14,bottle_15,bottle_16,bottle_17,bottle_18,bottle_19,bottle_20,bottle_21,bottle_22,bottle_23,bottle_24);
              }
           }
       }
       sleep(2);
}

//ABB流量计
void myAPI::Protocol_29(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    double total=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
    volatile char s[4];
    unsigned char t[8];
    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x04;
    sendbuf[2]=0x0b;
    sendbuf[3]=0xf0;
    sendbuf[4]=0x00;
    sendbuf[5]=0x02;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    readbuf=myCom[port]->readAll();
    readbuf="";
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(2);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
    if(readbuf.length()>=9){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),7);
                if((readbuf[7]==(char)(check&0xff))&&(readbuf[8]==(char)(check>>8)))
                {
                    s[0]=readbuf[6];
                    s[1]=readbuf[5];
                    s[2]=readbuf[4];
                    s[3]=readbuf[3];
                    rtd=*(float *)s;        //瞬时流量
                    flag="N";
                }
        }
    }
    sleep(2);
    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x04;
    sendbuf[2]=0x0f;
    sendbuf[3]=0xa0;
    sendbuf[4]=0x00;
    sendbuf[5]=0x08;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    readbuf=myCom[port]->readAll();
    readbuf="";
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(2);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
    if(readbuf.length()>=21){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
                if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
                {
                    t[0]=readbuf[3];
                    t[1]=readbuf[4];
                    t[2]=readbuf[5];
                    t[3]=readbuf[6];
                    t[4]=readbuf[7];
                    t[5]=readbuf[8];
                    t[6]=readbuf[9];
                    t[7]=readbuf[10];
                    total=myAPI::HexToDouble(t); //累计流量
                    if(flag!="N") flag='D';
                    if(flag=="N")
                    {
                        CacheDataProc(rtd,total,flag,Dec,Name,Code,Unit);
                    }
                }
        }
    }
}

//ABB流量计-表显累计值
void myAPI::Protocol_30(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
    unsigned char t[8];
    sleep(2);
    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x04;
    sendbuf[2]=0x0f;
    sendbuf[3]=0xa0;
    sendbuf[4]=0x00;
    sendbuf[5]=0x08;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    readbuf=myCom[port]->readAll();
    readbuf.clear();
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    delaymsec(2000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();

    if(readbuf.length()>=21){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
                if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
                {
                    t[0]=readbuf[3];
                    t[1]=readbuf[4];
                    t[2]=readbuf[5];
                    t[3]=readbuf[6];
                    t[4]=readbuf[7];
                    t[5]=readbuf[8];
                    t[6]=readbuf[9];
                    t[7]=readbuf[10];
                    rtd=myAPI::HexToDouble(t); //累计流量
                    flag='N';
                    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
                }
        }
    }
}

//肯特流量计
void myAPI::Protocol_31(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    double total=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
   volatile char s[4];

    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x07;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    myCom[port]->readAll();
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(1);
//    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
    if(readbuf.length()>=19){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
                if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
                {
                    s[0]=readbuf[6];
                    s[1]=readbuf[5];
                    s[2]=readbuf[4];
                    s[3]=readbuf[3];
                    rtd=*(float *)s;        //瞬时流量单位
                    total= (readbuf[9]<<24)+(readbuf[10]<<16)+(readbuf[11]<<8)+readbuf[12];
                    s[0]=readbuf[16];
                    s[1]=readbuf[15];
                    s[2]=readbuf[14];
                    s[3]=readbuf[13];
                    total+=*(float *)s;//累计流量单位M3
                    flag='N';
                    if(rtd>3) myApp::FLOW_FLG=1;
                    else {
                        myApp::FLOW_FLG=0;
                    }
                }
          }
    }

    CacheDataProc(rtd,total,flag,Dec,Name,Code,Unit);

}
//天泽COD
void myAPI::Protocol_32(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
   volatile char s[4];
//状态读取
    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x03;
    sendbuf[3]=0xBC;
    sendbuf[4]=0x00;
    sendbuf[5]=0x01;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    myCom[port]->readAll();
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(2);
//    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
    if(readbuf.length()>=7){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
                if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
                {
                    if(0==readbuf[4]&&0==readbuf[3]) myApp::COD_Isok=true;
                }
        }
    }
//瞬时数据读取
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0xEE;
    sendbuf[4]=0x00;
    sendbuf[5]=0x02;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    myCom[port]->readAll();
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(1);
//    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
    if(readbuf.length()>=9){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
                if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
                {
                    s[0]=readbuf[6];
                    s[1]=readbuf[5];
                    s[2]=readbuf[4];
                    s[3]=readbuf[3];
                    rtd=*(float *)s;
                    flag='N';
                }
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
//添加COD状态检测 空闲状态         myApp::COD_Isok=true;

    if(myApp::COD_FLG&&myApp::COD_Isok==true){
        myApp::COD_FLG=0;
        myApp::COD_Isok=false;
        //添加COD取样指令
        sendbuf.resize(11);
        sendbuf[0]=Address;
        sendbuf[1]=0x10;
        sendbuf[2]=0x06;
        sendbuf[3]=0x72;
        sendbuf[4]=0x00;
        sendbuf[5]=0x01;
        sendbuf[6]=0x02;
        sendbuf[7]=0x00;
        sendbuf[8]=0x01;
        check = myHelper::CRC16_Modbus(sendbuf.data(),9);
        sendbuf[9]=(char)(check);
        sendbuf[10]=(char)(check>>8);
        myCom[port]->readAll();
        qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
        myCom[port]->write(sendbuf);
        myCom[port]->flush();
    }

}

//莫迪康流量计
void myAPI::Protocol_33(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    qint64 total=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
    volatile char s[4];
    volatile qint64 t[8];

    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x02;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    myCom[port]->readAll();
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(1);
//    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
    if(readbuf.length()>=9){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
                if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
                {
                    s[0]=readbuf[3];
                    s[1]=readbuf[4];
                    s[2]=readbuf[5];
                    s[3]=readbuf[6];
                    rtd=s[0]*256+s[1]/qPow(10,s[3]);        //瞬时流量单位M3/H
                    flag='N';
                    if(rtd>3) myApp::FLOW_FLG=1;
                    else {
                        myApp::FLOW_FLG=0;
                    }
                }
          }
    }

    sleep(1);
    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0x10;
    sendbuf[4]=0x00;
    sendbuf[5]=0x02;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    readbuf=myCom[port]->readAll();
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(2);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
    if(readbuf.length()>=9){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
                if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
                {
                    t[0]=readbuf[3];
                    t[1]=readbuf[4];
                    t[2]=readbuf[5];
                    t[3]=readbuf[6];
                    if(flag!="N") flag='D';
                    sleep(1);
                    sendbuf.resize(8);
                    sendbuf[0]=Address;
                    sendbuf[1]=0x03;
                    sendbuf[2]=0x00;
                    sendbuf[3]=0x14;
                    sendbuf[4]=0x00;
                    sendbuf[5]=0x02;
                    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
                    sendbuf[6]=(char)(check);
                    sendbuf[7]=(char)(check>>8);
                    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
                    readbuf=myCom[port]->readAll();
                    myCom[port]->write(sendbuf);
                    myCom[port]->flush();
                    sleep(1);
                    readbuf=myCom[port]->readAll();
                    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
                    if(readbuf.length()>=9){
                        if(Address==readbuf[0])
                        {
                                check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
                                if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
                                {
                                    t[4]=readbuf[3];
                                    t[5]=readbuf[4];
                                    t[6]=readbuf[5];
                                    t[7]=readbuf[6];
                                    total =( t[0] << 40 | t[1]  << 32 | t[2]  << 24 | t[3] <<16|t[4] <<8|t[5] )/qPow(10,t[7])  ;
                                    if(flag!="N") flag='D';
                //                    if(flag=="N")
                //                    {
                //                        CacheDataProc(rtd,total,flag,Dec,Name,Code,Unit);
                //                    }
                                }
                        }
                    }

                }
        }
    }

    CacheDataProc(rtd,total,flag,Dec,Name,Code,Unit);

}

//聚阳-COD
void myAPI::Protocol_34(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
    volatile char s[4];
//状态读取
   sendbuf.resize(8);
   sendbuf[0]=Address;
   sendbuf[1]=0x04;
   sendbuf[2]=0x00;
   sendbuf[3]=0x13;
   sendbuf[4]=0x00;
   sendbuf[5]=0x01;
   check = myHelper::CRC16_Modbus(sendbuf.data(),6);
   sendbuf[6]=(char)(check);
   sendbuf[7]=(char)(check>>8);
   myCom[port]->readAll();
   qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
   myCom[port]->write(sendbuf);
   myCom[port]->flush();
   sleep(2);
//    usleep(COM[port].Timeout*1000);
   readbuf=myCom[port]->readAll();
   qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
   if(readbuf.length()>=7){
       if(Address==readbuf[0])
       {
               check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
               if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
               {
                   if(1==readbuf[4]&&0==readbuf[3]) myApp::COD_Isok=true;
               }
       }
   }

//测量值读取
    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x04;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x03;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    readbuf=myCom[port]->readAll();
    readbuf="";
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(2);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();

    if(readbuf.length()>=11){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
                if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
                {
                    if(1==readbuf[4])
                    {
                        s[0]=readbuf[6];
                        s[1]=readbuf[5];
                        s[2]=readbuf[8];
                        s[3]=readbuf[7];
                        rtd=*(float *)s;
                        flag='N';
                    }
                }
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
//添加COD状态检测 空闲状态         myApp::COD_Isok=true;

    if(myApp::COD_FLG&&myApp::COD_Isok==true){
        myApp::COD_FLG=0;
        myApp::COD_Isok=false;
        //添加COD取样指令
        sendbuf.resize(11);
        sendbuf[0]=Address;
        sendbuf[1]=0x10;
        sendbuf[2]=0x00;
        sendbuf[3]=0x00;
        sendbuf[4]=0x00;
        sendbuf[5]=0x01;
        sendbuf[6]=0x02;
        sendbuf[7]=0x00;
        sendbuf[8]=0x01;
        check = myHelper::CRC16_Modbus(sendbuf.data(),9);
        sendbuf[9]=(char)(check);
        sendbuf[10]=(char)(check>>8);
        myCom[port]->readAll();
        qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
        myCom[port]->write(sendbuf);
        myCom[port]->flush();
        sleep(1);
        myCom[port]->readAll();
    }

}

//聚阳-Ni
void myAPI::Protocol_35(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
    volatile char s[4];
//状态读取
   sendbuf.resize(8);
   sendbuf[0]=Address;
   sendbuf[1]=0x04;
   sendbuf[2]=0x00;
   sendbuf[3]=0x13;
   sendbuf[4]=0x00;
   sendbuf[5]=0x01;
   check = myHelper::CRC16_Modbus(sendbuf.data(),6);
   sendbuf[6]=(char)(check);
   sendbuf[7]=(char)(check>>8);
   myCom[port]->readAll();
   qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
   myCom[port]->write(sendbuf);
   myCom[port]->flush();
   sleep(2);
//    usleep(COM[port].Timeout*1000);
   readbuf=myCom[port]->readAll();
   qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
   if(readbuf.length()>=7){
       if(Address==readbuf[0])
       {
               check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
               if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
               {
                   if(1==readbuf[4]&&0==readbuf[3]) myApp::Ni_Isok=true;
               }
       }
   }

//测量值读取
    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x04;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x03;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    readbuf=myCom[port]->readAll();
    readbuf="";
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(2);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();

    if(readbuf.length()>=11){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
                if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
                {
                    if(1==readbuf[4])
                    {
                        s[0]=readbuf[6];
                        s[1]=readbuf[5];
                        s[2]=readbuf[8];
                        s[3]=readbuf[7];
                        rtd=*(float *)s;
                        flag='N';
                    }
                }
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
//添加COD状态检测 空闲状态         myApp::COD_Isok=true;

    if(myApp::Ni_FLG&&myApp::Ni_Isok==true){
        myApp::Ni_FLG=0;
        myApp::Ni_Isok=false;
        //添加COD取样指令
        sendbuf.resize(11);
        sendbuf[0]=Address;
        sendbuf[1]=0x10;
        sendbuf[2]=0x00;
        sendbuf[3]=0x00;
        sendbuf[4]=0x00;
        sendbuf[5]=0x01;
        sendbuf[6]=0x02;
        sendbuf[7]=0x00;
        sendbuf[8]=0x01;
        check = myHelper::CRC16_Modbus(sendbuf.data(),9);
        sendbuf[9]=(char)(check);
        sendbuf[10]=(char)(check>>8);
        myCom[port]->readAll();
        qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
        myCom[port]->write(sendbuf);
        myCom[port]->flush();
        sleep(1);
        myCom[port]->readAll();
    }

}

//聚阳-Cu
void myAPI::Protocol_36(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
    volatile char s[4];
//状态读取
   sendbuf.resize(8);
   sendbuf[0]=Address;
   sendbuf[1]=0x04;
   sendbuf[2]=0x00;
   sendbuf[3]=0x13;
   sendbuf[4]=0x00;
   sendbuf[5]=0x01;
   check = myHelper::CRC16_Modbus(sendbuf.data(),6);
   sendbuf[6]=(char)(check);
   sendbuf[7]=(char)(check>>8);
   myCom[port]->readAll();
   qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
   myCom[port]->write(sendbuf);
   myCom[port]->flush();
   sleep(2);
//    usleep(COM[port].Timeout*1000);
   readbuf=myCom[port]->readAll();
   qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
   if(readbuf.length()>=7){
       if(Address==readbuf[0])
       {
               check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
               if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
               {
                   if(1==readbuf[4]&&0==readbuf[3]) myApp::Cu_Isok=true;
               }
       }
   }

//测量值读取
    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x04;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x03;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    readbuf=myCom[port]->readAll();
    readbuf="";
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(2);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();

    if(readbuf.length()>=11){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
                if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
                {
                    if(1==readbuf[4])
                    {
                        s[0]=readbuf[6];
                        s[1]=readbuf[5];
                        s[2]=readbuf[8];
                        s[3]=readbuf[7];
                        rtd=*(float *)s;
                        flag='N';
                    }
                }
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
//添加COD状态检测 空闲状态         myApp::COD_Isok=true;

    if(myApp::Cu_FLG&&myApp::Cu_Isok==true){
        myApp::Cu_FLG=0;
        myApp::Cu_Isok=false;
        //添加COD取样指令
        sendbuf.resize(11);
        sendbuf[0]=Address;
        sendbuf[1]=0x10;
        sendbuf[2]=0x00;
        sendbuf[3]=0x00;
        sendbuf[4]=0x00;
        sendbuf[5]=0x01;
        sendbuf[6]=0x02;
        sendbuf[7]=0x00;
        sendbuf[8]=0x01;
        check = myHelper::CRC16_Modbus(sendbuf.data(),9);
        sendbuf[9]=(char)(check);
        sendbuf[10]=(char)(check>>8);
        myCom[port]->readAll();
        qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
        myCom[port]->write(sendbuf);
        myCom[port]->flush();
        sleep(1);
        myCom[port]->readAll();
    }

}

////028-总镍
//void myAPI::Protocol_34(int port,int Dec,QString Name,QString Code,QString Unit)
//{
//    double rtd=0;
//    QString flag="D";
//    int CN=0;
//    QString str_Flag="028-Flag=([A-Z])";
//    QString str_code="028-Rtd=([[0-9|.]+)";
//    QString str_CN="CN=([0-9]+)";
//    QByteArray readbuf;
//    do{
//        readbuf=myCom[port]->readAll();
//        qDebug()<<QString("COM%1 received:%2").arg(port+2).arg(readbuf.toHex().data());
//        sleep(2);
//    }while(!readbuf.endsWith("\r\n"));
////需要检测下readbuf的长度
//    if(readbuf.startsWith("##")&&readbuf.endsWith("\r\n")){
//        qDebug()<<QString("COM%1 received:%2").arg(port+2).arg(readbuf.data());
//        QRegExp Ex;
//        Ex.setPattern(str_CN);
//        if(Ex.indexIn(readbuf) != -1){
//            CN=Ex.cap(1).toInt();
//        }
//        if(CN==2011){
//            Ex.setPattern(str_code);
//            if(Ex.indexIn(readbuf) != -1){
//                rtd=Ex.cap(1).toDouble();
//            }

//            Ex.setPattern(str_Flag);
//            if(Ex.indexIn(readbuf) != -1){
//                flag=Ex.cap(1);
//            }
//            if(flag=="N")CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
//        }

//    }
//}

////029-总铜
//void myAPI::Protocol_35(int port,int Dec,QString Name,QString Code,QString Unit)
//{
//    double rtd=0;
//    QString flag="D";
//    int CN=0;
//    QString str_Flag="029-Flag=([A-Z])";
//    QString str_code="029-Rtd=([[0-9|.]+)";
//    QString str_CN="CN=([0-9]+)";
//    QByteArray readbuf;
//    do{
//        readbuf=myCom[port]->readAll();
//        qDebug()<<QString("COM%1 received:%2").arg(port+2).arg(readbuf.toHex().data());
//        sleep(2);
//    }while(!readbuf.endsWith("\n\r"));
////需要检测下readbuf的长度
//    if(readbuf.endsWith("\n\r")&&readbuf.startsWith("##")){
//        qDebug()<<QString("COM%1 received2:%2").arg(port+2).arg(readbuf.data());
//        QRegExp Ex;
//        Ex.setPattern(str_CN);
//        if(Ex.indexIn(readbuf) != -1){
//            CN=Ex.cap(1).toInt();
//        }
//        if(CN==2011){
//            Ex.setPattern(str_code);
//            if(Ex.indexIn(readbuf) != -1){
//                rtd=Ex.cap(1).toDouble();
//            }
//            Ex.setPattern(str_Flag);
//            if(Ex.indexIn(readbuf) != -1){
//                flag=Ex.cap(1);
//            }
//            CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
//        }

//    }
//    sleep(5);
//}

////011-COD
//void myAPI::Protocol_36(int port,int Dec,QString Name,QString Code,QString Unit)
//{
//    double rtd=0;
//    QString flag="D";
//    int CN=0;
//    QString str_Flag="011-Flag=([A-Z])";
//    QString str_code="011-Rtd=([[0-9|.]+)";
//    QString str_CN="CN=([0-9]+)";
//    QByteArray readbuf;
//    do{
//        readbuf=myCom[port]->readAll();
//        qDebug()<<QString("COM%1 received:%2").arg(port+2).arg(readbuf.toHex().data());
//        sleep(2);
//    }while(!readbuf.endsWith("\r\n"));
////需要检测下readbuf的长度
//    if(readbuf.startsWith("##")&&readbuf.endsWith("\r\n")){
//        qDebug()<<QString("COM%1 received:%2").arg(port+2).arg(readbuf.data());
//        QRegExp Ex;
//        Ex.setPattern(str_CN);
//        if(Ex.indexIn(readbuf) != -1){
//            CN=Ex.cap(1).toInt();
//        }
//        if(CN==2011){
//            Ex.setPattern(str_code);
//            if(Ex.indexIn(readbuf) != -1){
//                rtd=Ex.cap(1).toDouble();
//            }

//            Ex.setPattern(str_Flag);
//            if(Ex.indexIn(readbuf) != -1){
//                flag=Ex.cap(1);
//            }
//            if(flag=="N")CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
//        }

//    }
//}

//LMAG流量计
void myAPI::Protocol_37(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    double total=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
    volatile char s[4];
    int32_t total_tmp;
    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x04;
    sendbuf[2]=0x10;
    sendbuf[3]=0x10;
    sendbuf[4]=0x00;
    sendbuf[5]=0x02;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    readbuf=myCom[port]->readAll();
    readbuf="";
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(2);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
    if(readbuf.length()>=9){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),7);
                if((readbuf[7]==(char)(check&0xff))&&(readbuf[8]==(char)(check>>8)))
                {
                    s[0]=readbuf[6];
                    s[1]=readbuf[5];
                    s[2]=readbuf[4];
                    s[3]=readbuf[3];
                    rtd=*(float *)s;        //瞬时流量
                    flag="N";
                }
        }
    }
    sleep(2);
    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x04;
    sendbuf[2]=0x10;
    sendbuf[3]=0x18;
    sendbuf[4]=0x00;
    sendbuf[5]=0x02;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(2);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
    if(readbuf.length()>=9){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
                if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
                {
                    total_tmp=readbuf[3];
                    total_tmp=total_tmp<<8;
                    total_tmp+=readbuf[4];
                    total_tmp=total_tmp<<8;
                    total_tmp+=readbuf[5];
                    total_tmp=total_tmp<<8;
                    total_tmp+=readbuf[6];
                    total=total_tmp;
                    if(flag!="N") flag='D';
                    if(flag=="N")
                    {
                        CacheDataProc(rtd,total,flag,Dec,Name,Code,Unit);
                    }
                }
        }
    }

}


//聚阳-Ge
void myAPI::Protocol_38(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
    volatile char s[4];
//状态读取
   sendbuf.resize(8);
   sendbuf[0]=Address;
   sendbuf[1]=0x04;
   sendbuf[2]=0x00;
   sendbuf[3]=0x13;
   sendbuf[4]=0x00;
   sendbuf[5]=0x01;
   check = myHelper::CRC16_Modbus(sendbuf.data(),6);
   sendbuf[6]=(char)(check);
   sendbuf[7]=(char)(check>>8);
   myCom[port]->readAll();
   qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
   myCom[port]->write(sendbuf);
   myCom[port]->flush();
   sleep(2);
//    usleep(COM[port].Timeout*1000);
   readbuf=myCom[port]->readAll();
   qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
   if(readbuf.length()>=7){
       if(Address==readbuf[0])
       {
               check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
               if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
               {
                   if(1==readbuf[4]&&0==readbuf[3]) myApp::Ge_Isok=true;
               }
       }
   }

//测量值读取
    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x04;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x03;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    readbuf=myCom[port]->readAll();
    readbuf="";
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(2);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();

    if(readbuf.length()>=11){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
                if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
                {
                    if(1==readbuf[4])
                    {
                        s[0]=readbuf[6];
                        s[1]=readbuf[5];
                        s[2]=readbuf[8];
                        s[3]=readbuf[7];
                        rtd=*(float *)s;
                        flag='N';
                    }
                }
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);

    if(myApp::Ge_FLG&&myApp::Ge_Isok==true){
        myApp::Ge_FLG=0;
        myApp::Ge_Isok=false;
        //添加COD取样指令
        sendbuf.resize(11);
        sendbuf[0]=Address;
        sendbuf[1]=0x10;
        sendbuf[2]=0x00;
        sendbuf[3]=0x00;
        sendbuf[4]=0x00;
        sendbuf[5]=0x01;
        sendbuf[6]=0x02;
        sendbuf[7]=0x00;
        sendbuf[8]=0x01;
        check = myHelper::CRC16_Modbus(sendbuf.data(),9);
        sendbuf[9]=(char)(check);
        sendbuf[10]=(char)(check>>8);
        myCom[port]->readAll();
        qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
        myCom[port]->write(sendbuf);
        myCom[port]->flush();
        sleep(1);
        myCom[port]->readAll();
    }

}


//太仓-NI-GE
void myAPI::Protocol_39(int port,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    double rtd=0;
    QString flag="D";
    QByteArray readbuf;
    QByteArray sendbuf;
    int check=0;
    volatile char s[4];
//测量值读取
    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x02;
    check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    qDebug()<<QString("COM%1 send:").arg(port+2)<<sendbuf.toHex().toUpper();
    readbuf=myCom[port]->readAll();
    readbuf="";
    myCom[port]->write(sendbuf);
    myCom[port]->flush();
    sleep(2);
    readbuf=myCom[port]->readAll();
    qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();

    if(readbuf.length()>=9){
        if(Address==readbuf[0])
        {
                check = myHelper::CRC16_Modbus(readbuf.data(),readbuf.length()-2);
                if((readbuf[readbuf.length()-2]==(char)(check&0xff))&&(readbuf[readbuf.length()-1]==(char)(check>>8)))
                {
                    s[0]=readbuf[6];
                    s[1]=readbuf[5];
                    s[2]=readbuf[4];
                    s[3]=readbuf[3];
                    rtd=*(float *)s;
                    flag='N';
                }
        }
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}



double myAPI::HexToDouble(const unsigned char* bytes)
    {
        quint64   data1 = bytes[0];
        quint64   data2 = bytes[1];
        quint64   data3 = bytes[2];
        quint64   data4 = bytes[3];
        quint64   data5 = bytes[4];
        quint64   data6 = bytes[5];
        quint64   data7 = bytes[6];
        quint64   data8 = bytes[7];

        quint64  data = data1 << 56 | data2 << 48 | data3 << 40 | data4<<32|data5<<24|data6<<16|data7<<8|data8;


        int nSign;
        if ((data>>63)==1)
        {
            nSign = -1;
        }
        else
        {
            nSign = 1;
        }
        quint64 nExp = data & (0x7FF0000000000000LL);
        nExp = nExp >> 52;

        double nMantissa = data & (0xFFFFFFFFFFFFFLL);

        if (nMantissa != 0)
            nMantissa = 1 + nMantissa / 4503599627370496LL;

        double value = nSign * nMantissa * (2 << (nExp - 2048));
        return value;
    }




//与设备进行通讯
void myAPI::MessageFromCom(int port)
{
    QString Code;
    QString Name;
    QString  Unit;
    int Address;
    int Decimals;
    double alarm_max;
    double alarm_min;

    QSqlQuery query;
    QString sql=QString("select * from [ParaInfo] where [UseChannel]='COM%1'").arg(port+2);
    query.exec(sql);
    while(query.next())
    {
        Name=query.value(0).toString();
        Code=query.value(1).toString();
        Unit=query.value(2).toString();
        Address=query.value(4).toInt();
        Decimals=query.value(15).toInt();
        alarm_max=query.value(9).toDouble();
        alarm_min=query.value(10).toDouble();
        switch (query.value(5).toInt())//通讯协议
        {
        case 1://天泽VOCs
                Protocol_2(port,Address,Decimals,Name,Code,Unit);
                break;
        case 2://声级计
                Protocol_3(port,Decimals,Name,Code,Unit);
                break;
        case 3://风速
                 Protocol_4(port,Address,Decimals,Name,Code,Unit);
                break;
        case 4://风向
                Protocol_5(port,Address,Decimals,Name,Code,Unit);
                break;
        case 5://ES-642浓度
                Protocol_6(port,Decimals,Name,Code,Unit);
                break;
        case 6://ES-642温度
                Protocol_7(port,Decimals,Name,Code,Unit);
                break;
        case 7://ES-642湿度
                Protocol_8(port,Decimals,Name,Code,Unit);
                break;
        case 8://气压
                Protocol_9(port,Decimals,Name,Code,Unit);
                break;
        case 9://经度
                Protocol_10(port,Address,Decimals,Name,Code,Unit);
                break;
        case 10://纬度
                Protocol_11(port,Address,Decimals,Name,Code,Unit);
                break;
        case 11://瑾熙流量计
                Protocol_12(port,Address,Decimals,Name,Code,Unit);
                break;
        case 12://光华流量计
                Protocol_13(port,Address,Decimals,Name,Code,Unit);
                break;
        case 13://TOC-4100CJ
                Protocol_14(port,Address,Decimals,Name,Code,Unit);
                break;
        case 14://承德流量计
                Protocol_15(port,Address,Decimals,Name,Code,Unit);
                break;
        case 15://SJFC-200
                Protocol_16(port,Address,Decimals,Name,Code,Unit);
                break;
        case 16://天泽温度
                Protocol_17(port,Address,Decimals,Name,Code,Unit);
                break;
        case 17://天泽湿度
                Protocol_18(port,Address,Decimals,Name,Code,Unit);
                break;
        case 18://天泽气压
                Protocol_19(port,Address,Decimals,Name,Code,Unit);
                break;
        case 19://光华流量计-总量
                Protocol_20(port,Address,Decimals,Name,Code,Unit);
            break;
        case 20://PH-P206
            Protocol_21(port,Address,Decimals,Name,Code,Unit);
        break;
        case 21://明渠流量计
            Protocol_22(port,Address,Decimals,Name,Code,Unit);
        break;
        case 22://电导率
            Protocol_23(port,Address,Decimals,Name,Code,Unit);
        break;
        case 23://锐泉COD
            Protocol_24(port,Address,Decimals,Name,Code,Unit);
        break;
        case 24://刷卡设备
            Protocol_25(port);
        break;
        case 25://哈希COD
            Protocol_26(port,Address,Decimals,Name,Code,Unit);
        break;
        case 26://哈希氨氮
            Protocol_27(port,Address,Decimals,Name,Code,Unit,alarm_max);
        break;
        case 27://KS-3200采样仪
            if(myApp::SampleMode==0){   //自动模式
                Protocol_28(port);
            }
            break;
        case 28://ABB流量计
            Protocol_29(port,Address,Decimals,Name,Code,Unit);
            break;

        case 29://ABB流量计-表显累计值
            Protocol_30(port,Address,Decimals,Name,Code,Unit);
            break;
        case 30://肯特流量计
            Protocol_31(port,Address,Decimals,Name,Code,Unit);
            break;
        case 31://天泽COD
            Protocol_32(port,Address,Decimals,Name,Code,Unit);
            break;
        case 32://莫迪康流量计
            Protocol_33(port,Address,Decimals,Name,Code,Unit);
            break;
        case 33://聚阳-COD
            Protocol_34(port,Address,Decimals,Name,Code,Unit);
            break;
        case 34://聚阳-Ni
            Protocol_35(port,Address,Decimals,Name,Code,Unit);
            break;
        case 35://聚阳-Cu
            Protocol_36(port,Address,Decimals,Name,Code,Unit);
            break;
        case 36://LMAG流量计
            Protocol_37(port,Address,Decimals,Name,Code,Unit);
            break;
        case 37://聚阳-Ge
            Protocol_38(port,Address,Decimals,Name,Code,Unit);
            break;
        case 38://太仓-NI-GE
            Protocol_39(port,Address,Decimals,Name,Code,Unit);
            break;
        default: break;

        }
    }
    usleep(COM[port].Interval*1000);
}

//    非阻塞延时：

    void myAPI::delaymsec(int msec)
    {
        QTime dieTime = QTime::currentTime().addMSecs(msec);

        while( QTime::currentTime() < dieTime )

        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }


