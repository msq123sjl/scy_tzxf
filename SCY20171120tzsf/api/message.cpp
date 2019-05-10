#include "message.h"
#include "myhelper.h"
#include "api/myapp.h"
#include "api/myapi.h"
#include "api/gpio.h"

extern QList<QString> list;

//请求应答
void Message::RequestRespond(int QnRtn,QextSerialPort *com,QTcpSocket *tcp)
{
    QString str="ST=91;CN=9011;";
    str+="PW="+myApp::PW+';';
    str+="MN="+myApp::MN+';';   
    str+="Flag=4;";
    str+="CP=&&QN="+QN+';';
    str+="QnRtn="+QString::number(QnRtn)+"&&";

    QByteArray ch;
    ch.resize(4);
    int len=str.size();
    int crc=myHelper::CRC16_GB212(str.toLatin1().data(),len);
    sprintf(ch.data(),"%04X",crc);
    str.append(ch+"\r\n");
    ch[0] = (len/1000)+'0';
    ch[1] = (len%1000/100)+'0';
    ch[2] = (len%100/10)+'0';
    ch[3] = (len%10)+'0';
    str.insert(0,"##");
    str.insert(2,ch);

    if(com!=NULL)
        com->write(str.toAscii());
    if(tcp!=NULL)
    {
        tcp->write(str.toAscii());
        tcp->flush();
    }

}

//通知应答
void Message::NoticeRespond(QextSerialPort *com,QTcpSocket *tcp)
{
    QString str="ST=91;CN=9013;";
    str+="PW="+myApp::PW+';';
    str+="MN="+myApp::MN+';';
    str+="CP=&&QN="+QN+"&&";

    QByteArray ch;
    ch.resize(4);
    int len=str.size();
    int crc=myHelper::CRC16_GB212(str.toLatin1().data(),len);
    sprintf(ch.data(),"%04X",crc);
    str.append(ch+"\r\n");
    ch[0] = (len/1000)+'0';
    ch[1] = (len%1000/100)+'0';
    ch[2] = (len%100/10)+'0';
    ch[3] = (len%10)+'0';
    str.insert(0,"##");
    str.insert(2,ch);

    if(com!=NULL)
        com->write(str.toAscii());
    if(tcp!=NULL){
        tcp->write(str.toAscii());
    tcp->flush();
    }

}

//数据应答
void Message::DataRespond(QextSerialPort *com,QTcpSocket *tcp)
{
    QString str="ST=91;CN=9014;";
    str+="CP=&&QN="+QN+";";
    str+="CN="+QString("%1").arg(CN)+"&&";

    QByteArray ch;
    ch.resize(4);
    int len=str.size();
    int crc=myHelper::CRC16_GB212(str.toLatin1().data(),len);
    sprintf(ch.data(),"%04X",crc);
    str.append(ch+"\r\n");
    ch[0] = (len/1000)+'0';
    ch[1] = (len%1000/100)+'0';
    ch[2] = (len%100/10)+'0';
    ch[3] = (len%10)+'0';
    str.insert(0,"##");
    str.insert(2,ch);

    if(com!=NULL)
        com->write(str.toAscii());
    if(tcp!=NULL){
        tcp->write(str.toAscii());
        tcp->flush();
    }

}

//操作返回操作执行结果
void Message::ExecuteRespond(int ExeRtn,QextSerialPort *com,QTcpSocket *tcp)
{
    QString str="ST=91;CN=9012;";
    str+="PW="+myApp::PW+';';
    str+="MN="+myApp::MN+';';
    str+="CP=&&QN="+QN+";";
    str+="ExeRtn="+QString("%1").arg(ExeRtn)+"&&";

    QByteArray ch;
    ch.resize(4);
    int len=str.size();
    int crc=myHelper::CRC16_GB212(str.toLatin1().data(),len);
    sprintf(ch.data(),"%04X",crc);
    str.append(ch+"\r\n");
    ch[0] = (len/1000)+'0';
    ch[1] = (len%1000/100)+'0';
    ch[2] = (len%100/10)+'0';
    ch[3] = (len%10)+'0';
    str.insert(0,"##");
    str.insert(2,ch);

    if(com!=NULL)
        com->write(str.toAscii());
    if(tcp!=NULL){
        tcp->write(str.toAscii());
        tcp->flush();
    }
}

//发送现场时间
void Message::SendCurrentTime(QextSerialPort *com,QTcpSocket *tcp)
{
    QString str="ST="+ST_to_Str[myApp::StType]+';';
    str+="CN="+QString("%1").arg(CN_SendTime)+';';
    str+="PW="+myApp::PW+';';
    str+="MN="+myApp::MN+';';
    str+="Flag=4;CP=&&";
    str+="QN="+QN+';';
    str+="SystemTime="+QDateTime::currentDateTime().toString("yyyyMMddhhmmss")+"&&";

    QByteArray ch;
    ch.resize(4);
    int len=str.size();
    int crc=myHelper::CRC16_GB212(str.toLatin1().data(),len);
    sprintf(ch.data(),"%04X",crc);
    str.append(ch+"\r\n");
    ch[0] = (len/1000)+'0';
    ch[1] = (len%1000/100)+'0';
    ch[2] = (len%100/10)+'0';
    ch[3] = (len%10)+'0';
    str.insert(0,"##");
    str.insert(2,ch);

    if(com!=NULL)
        com->write(str.toAscii());
    if(tcp!=NULL){
        tcp->write(str.toAscii());
        tcp->flush();
    }

}

//发送报警地址
void Message::SendAlarmTarget(QextSerialPort *com,QTcpSocket *tcp)
{
    QString str="ST="+ST_to_Str[myApp::StType]+';';
    str+="CN="+QString("%1").arg(CN_SendAlarmTarget)+';';
    str+="PW="+myApp::PW+';';
    str+="MN="+myApp::MN+';';
    str+="CP=&&QN="+QN+';';
    str+="AlarmTarget="+myApp::AlarmTarget+"&&";

    QByteArray ch;
    ch.resize(4);
    int len=str.size();
    int crc=myHelper::CRC16_GB212(str.toLatin1().data(),len);
    sprintf(ch.data(),"%04X",crc);
    str.append(ch+"\r\n");
    ch[0] = (len/1000)+'0';
    ch[1] = (len%1000/100)+'0';
    ch[2] = (len%100/10)+'0';
    ch[3] = (len%10)+'0';
    str.insert(0,"##");
    str.insert(2,ch);

    if(com!=NULL)
        com->write(str.toAscii());
    if(tcp!=NULL){
        tcp->write(str.toAscii());
    tcp->flush();
    }

}

//发送日数据上报时间
void Message::SendReportTime(QextSerialPort *com,QTcpSocket *tcp)
{
    QString str="ST="+ST_to_Str[myApp::StType]+';';
    str+="CN="+QString("%1").arg(CN_SendReportTime)+';';
    str+="PW="+myApp::PW+';';
    str+="MN="+myApp::MN+';';
    str+="CP=&&QN="+QN+';';
    str+="ReportTime="+QString("%1%2")
            .arg(myApp::ReportTimeHour, 2, 10, QChar('0'))
            .arg(myApp::ReportTimeMin,2,10,QChar('0'))+"&&";

    QByteArray ch;
    ch.resize(4);
    int len=str.size();
    int crc=myHelper::CRC16_GB212(str.toLatin1().data(),len);
    sprintf(ch.data(),"%04X",crc);
    str.append(ch+"\r\n");
    ch[0] = (len/1000)+'0';
    ch[1] = (len%1000/100)+'0';
    ch[2] = (len%100/10)+'0';
    ch[3] = (len%10)+'0';
    str.insert(0,"##");
    str.insert(2,ch);

    if(com!=NULL)
        com->write(str.toAscii());
    if(tcp!=NULL){
        tcp->write(str.toAscii());
        tcp->flush();
    }
}

//发送实时数据上报间隔
void Message::SendRtdInterval(QextSerialPort *com,QTcpSocket *tcp)
{
    QString str="ST="+ST_to_Str[myApp::StType]+';';
    str+="CN="+QString("%1").arg(CN_SendRtdInterval)+';';
    str+="PW="+myApp::PW+';';
    str+="MN="+myApp::MN+';';
    str+="CP=&&QN="+QN+';';
    str+="RtdInterval="+QString("%1").arg(myApp::RtdInterval)+"&&";

    QByteArray ch;
    ch.resize(4);
    int len=str.size();
    int crc=myHelper::CRC16_GB212(str.toLatin1().data(),len);
    sprintf(ch.data(),"%04X",crc);
    str.append(ch+"\r\n");
    ch[0] = (len/1000)+'0';
    ch[1] = (len%1000/100)+'0';
    ch[2] = (len%100/10)+'0';
    ch[3] = (len%10)+'0';
    str.insert(0,"##");
    str.insert(2,ch);

    if(com!=NULL)
        com->write(str.toAscii());
    if(tcp!=NULL){
        tcp->write(str.toAscii());
        tcp->flush();
    }

}

//发送设施运行状态
void Message::SendStatus(QextSerialPort *com,QTcpSocket *tcp)
{
    QString str="ST="+ST_to_Str[myApp::StType]+';';
    str+="CN="+QString("%1").arg(CN_SendStatus)+';';
    str+="PW="+myApp::PW+';';
    str+="MN="+myApp::MN+';';
    str+="CP=&&DataTime="+QDateTime::currentDateTime().toString("yyyyMMddhhmmss")+';';
    str+="SB1-RS="+QString::number(GetSwitchStatus(10))+';';
    str+="SB2-RS="+QString::number(GetSwitchStatus(11))+';';
    str+="SB3-RS="+QString::number(GetSwitchStatus(12))+';';
    str+="SB4-RS="+QString::number(GetSwitchStatus(13))+';';
    str+="SB5-RS="+QString::number(GetSwitchStatus(14))+';';
    str+="SB6-RS="+QString::number(GetSwitchStatus(15));
    str+="&&";

    QByteArray ch;
    ch.resize(4);
    int len=str.size();
    int crc=myHelper::CRC16_GB212(str.toLatin1().data(),len);
    sprintf(ch.data(),"%04X",crc);
    str.append(ch+"\r\n");
    ch[0] = (len/1000)+'0';
    ch[1] = (len%1000/100)+'0';
    ch[2] = (len%100/10)+'0';
    ch[3] = (len%10)+'0';
    str.insert(0,"##");
    str.insert(2,ch);

    if(com!=NULL)
        com->write(str.toAscii());
    if(tcp!=NULL){
        tcp->write(str.toAscii());
        tcp->flush();
    }
}

//发送实时数据
void Message::SendRtdData_Slave(QextSerialPort *com,QTcpSocket *tcp)
{
    QString str="ST="+ST_to_Str[myApp::StType]+';';
    str+="CN="+QString("%1").arg(CN_SendRtdData)+';';
    str+="PW="+myApp::PW+';';
    str+="MN="+myApp::MN+';';
    str+="CP=&&DataTime="+QDateTime::currentDateTime().toString("yyyyMMddhhmmss");   
    QSqlQuery query(QSqlDatabase::database("memory",true));
    QString sql;
    sql="select [Code],[Rtd],[Flag] from [CacheRtd]";
    query.exec(sql);
    QString Code;
    QString flag;
    QString Rtd;
    while(query.next()){
        str+=';';
        Code=query.value(0).toString();
        Rtd=query.value(1).toString();
        flag=query.value(2).toString();
        str+=Code+"-Rtd="+Rtd.split(" ")[0]+",";
        str+=Code+"-Flag="+flag;
    }
    str+="&&";

    QByteArray ch;
    ch.resize(4);
    int len=str.size();
    int crc=myHelper::CRC16_GB212(str.toLatin1().data(),len);
    sprintf(ch.data(),"%04X",crc);
    str.append(ch+"\r\n");
    ch[0] = (len/1000)+'0';
    ch[1] = (len%1000/100)+'0';
    ch[2] = (len%100/10)+'0';
    ch[3] = (len%10)+'0';
    str.insert(0,"##");
    str.insert(2,ch);

    if(com!=NULL)
        com->write(str.toAscii());
    if(tcp!=NULL){
        tcp->write(str.toAscii());
        tcp->flush();
    }
}

//发送统计数据
void Message::SendCountData_Slave(int CN_Type,QextSerialPort *com,QTcpSocket *tcp)
{
    QDateTime tempTime=BeginTime;
    QString strTime;
    QString str;
    QSqlQuery query;
    QSqlQuery query1;
    QString sql;
    QString Code;
    QByteArray ch;
    while(tempTime<=EndTime)
    {
        if(CN_Type==CN_SendHourData)
        {
            strTime=tempTime.toString("yyyy-MM-dd hh:00:00");
            tempTime=tempTime.addSecs(3600);
        }
        else if(CN_Type==CN_SendMinsData)
        {
            strTime=tempTime.toString("yyyy-MM-dd hh:mm:00");
            tempTime=tempTime.addSecs(60);
        }
        else if(CN_Type==CN_SendDayData)
        {
            strTime=tempTime.toString("yyyy-MM-dd 00:00:00");
            tempTime=tempTime.addDays(1);
        }
        else return;

        str.clear();
        str="ST="+ST_to_Str[myApp::StType]+';';
        str+="CN="+QString("%1").arg(CN_Type)+';';
        str+="PW="+myApp::PW+';';
        str+="MN="+myApp::MN+';';
        str+="CP=&&DataTime="+QString("%1%2%3%4%5%6")
                .arg(strTime.mid(0,4))
                .arg(strTime.mid(5,2))
                .arg(strTime.mid(8,2))
                .arg(strTime.mid(11,2))
                .arg(strTime.mid(14,2))
                .arg(strTime.mid(17,2));
        sql="select * from [ParaInfo]";
        query.exec(sql);
        while(query.next())
        {
            Code=query.value(1).toString();
            if(CN_Type==CN_SendHourData){
                sql="select * from Hour_"+Code+" where [GetTime]='"+strTime+"'";
            }
            else if(CN_Type==CN_SendMinsData){
                sql="select * from Mins_"+Code+" where [GetTime]='"+strTime+"'";
            }
            else if(CN_Type==CN_SendDayData){
                sql="select * from Day_"+Code+" where [GetTime]='"+strTime+"'";
            }
            query1.exec(sql);
            if(query1.next())
            {
                str+=';';

                if(query.value(11).toBool()){
                    str+=Code+"-Max="+query1.value(1).toString()+',';
                }
                if(query.value(12).toBool()){
                    str+=Code+"-Min="+query1.value(2).toString()+',';
                }
                if(query.value(13).toBool()){
                    str+=Code+"-Avg="+query1.value(3).toString()+',';
                }
                if(query.value(14).toBool()){
                    str+=Code+"-Cou="+query1.value(4).toString();
                }

            }

        }
        str+="&&";

        ch.resize(4);
        int len=str.size();
        int crc=myHelper::CRC16_GB212(str.toLatin1().data(),len);
        sprintf(ch.data(),"%04X",crc);
        str.append(ch+"\r\n");
        ch[0] = (len/1000)+'0';
        ch[1] = (len%1000/100)+'0';
        ch[2] = (len%100/10)+'0';
        ch[3] = (len%10)+'0';
        str.insert(0,"##");
        str.insert(2,ch);

        if(str.size()>84)
        {
            if(com!=NULL)
                com->write(str.toAscii());
            if(tcp!=NULL){
                tcp->write(str.toAscii());
                tcp->flush();
            }
        }
    }
}

//发送运行时间日数据
void Message::SendRunTimeData(QextSerialPort *com,QTcpSocket *tcp)
{
    QDateTime tempTime=BeginTime;
    QString str;
    QString strTime;
    while(tempTime<=EndTime)
    {
        strTime=tempTime.toString("yyyy-MM-dd 00:00:00");
        tempTime=tempTime.addDays(1);
        str.clear();
        str="ST="+ST_to_Str[myApp::StType]+';';
        str+="CN="+QString("%1").arg(CN_SendRunTimeData)+';';
        str+="PW="+myApp::PW+';';
        str+="MN="+myApp::MN+';';
        str+="CP=&&DataTime="+QString("%1%2%3%4%5%6")
                .arg(strTime.mid(0,4))
                .arg(strTime.mid(5,2))
                .arg(strTime.mid(8,2))
                .arg(strTime.mid(11,2))
                .arg(strTime.mid(14,2))
                .arg(strTime.mid(17,2));
        QSqlQuery query;
        QString sql;
        sql="select * from [RunTimeData] where [GetTime]='"+strTime+"'";
        query.exec(sql);
        while(query.next()){
            str+=';';
            str+="SB1-RT="+query.value(1).toString()+';';
            str+="SB2-RT="+query.value(2).toString()+';';
            str+="SB3-RT="+query.value(3).toString()+';';
            str+="SB4-RT="+query.value(4).toString();
        }
        str+="&&";

        QByteArray ch;
        ch.resize(4);
        int len=str.size();
        int crc=myHelper::CRC16_GB212(str.toLatin1().data(),len);
        sprintf(ch.data(),"%04X",crc);
        str.append(ch+"\r\n");
        ch[0] = (len/1000)+'0';
        ch[1] = (len%1000/100)+'0';
        ch[2] = (len%100/10)+'0';
        ch[3] = (len%10)+'0';
        str.insert(0,"##");
        str.insert(2,ch);

        if(com!=NULL)
            com->write(str.toAscii());
        if(tcp!=NULL){
            tcp->write(str.toAscii());
            tcp->flush();
        }
    }
}

//设定报警值
void Message::SetAlarmValue(QString s)
{
    QSqlQuery query; 
    QString sql;
    QRegExp Ex;
    int pos=0;
    Ex.setPattern("([a-zA-Z0-9]+)-UpValue=([0-9|.]+)");
    while((pos=Ex.indexIn(s,pos)) != -1)
    {
        sql="update [ParaInfo] set [AlarmUp]='"+Ex.cap(2)+"' where [Code]='"+Ex.cap(1)+"'";
        query.exec(sql);
        pos+=Ex.matchedLength();
    }

    pos=0;
    Ex.setPattern("([a-zA-Z0-9]+)-LowValue=([0-9|.]+)");
    while((pos=Ex.indexIn(s,pos)) != -1)
    {
        sql="update [ParaInfo] set [AlarmLow]='"+Ex.cap(2)+"' where [Code]='"+Ex.cap(1)+"'";
        query.exec(sql);
        pos+=Ex.matchedLength();
    }

}

//发送报警限值
void Message::SendAlarmValue(QString s,QextSerialPort *com,QTcpSocket *tcp)
{
    QSqlQuery query;
    QString sql;
    QString str="ST="+ST_to_Str[myApp::StType]+';';
    str+="CN="+QString("%1").arg(CN_SendAlarmData)+';';
    str+="PW="+myApp::PW+';';
    str+="MN="+myApp::MN+';';
    str+="CP=&&QN="+QN;
    QRegExp Ex;
    int pos=0;
    Ex.setPattern("PolId=([0-9A-Za-z]+)");
    while((pos=Ex.indexIn(s,pos)) != -1)
    {
        sql="select [AlarmUp],[AlarmLow] from [ParaInfo]";
        sql+=" where [Code]='"+Ex.cap(1)+"'";
        query.exec(sql);
        str+=";";
        if(query.next()){
        str+=Ex.cap(1)+"-UpValue="+query.value(0).toString()+',';
        str+=Ex.cap(1)+"-LowValue="+query.value(1).toString();
        }
        pos+=Ex.matchedLength();
    }
    str+="&&";

    QByteArray ch;
    ch.resize(4);
    int len=str.size();
    int crc=myHelper::CRC16_GB212(str.toLatin1().data(),len);
    sprintf(ch.data(),"%04X",crc);
    str.append(ch+"\r\n");
    ch[0] = (len/1000)+'0';
    ch[1] = (len%1000/100)+'0';
    ch[2] = (len%100/10)+'0';
    ch[3] = (len%10)+'0';
    str.insert(0,"##");
    str.insert(2,ch);

    if(com!=NULL)
        com->write(str.toAscii());
    if(tcp!=NULL){
        tcp->write(str.toAscii());
        tcp->flush();
    }
}


//远程设定时间
bool Message::messageSetTime(int year,int month,int day,int hour,int min,int sec)
{
    QString TempMsg;
    #ifdef Q_OS_LINUX
        QString str=QString("/bin/date -s '%1-%2-%3 %4:%5:%6'") //  生成时间设置命令字符串
            .arg(year)
            .arg(month)
            .arg(day)
            .arg(hour)
            .arg(min)
            .arg(sec);

    int res1;
    int res2;
    res1=system(str.toLatin1().data());  //设置系统时间
    res2=system("/sbin/hwclock --systohc");   //将系统时间写入到RTC硬件中，以保留设置。这一操作是为了将修改好的时间写入到RTC中保存。如果不进行这一步操作，则
                                                                      //重新上电开机以后系统从RTC中读取到的仍然是原来的时间
    if(res1==0 && res2==0){
        TempMsg="远程更改日期时间成功";
        myAPI a;
        a.AddEventInfoUser(TempMsg);
        return true;
    }
    else{
        TempMsg="远程更改日期时间失败";
        myAPI a;
        a.AddEventInfoUser(TempMsg);
        return false;
    }
    #elif defined (Q_OS_WIN)

    #endif  

}

int Message::messageThresholdValue(QString s)
{
    QSqlQuery query;
    QString sql;
    QRegExp Ex;
    int pos=0;
    QString keyword;
    QString Code,DateType,BeginTime,EndTime,SiteType,Value;

    Ex.setPattern("([a-zA-Z0-9]+)-Thd=([0-9|.]+)");
//    while((pos=Ex.indexIn(s,pos)) != -1)
    pos=Ex.indexIn(s);
    if(pos!=-1)
    {
        Code=Ex.cap(1);
        Value=Ex.cap(2);
        keyword=Code+"-DateType=([0-9]+)";
        Ex.setPattern(keyword);
        if(Ex.indexIn(s) != -1){
            DateType=Ex.cap(1);
        }
        keyword=Code+"-BeginTime=([0-9]+)";
        Ex.setPattern(keyword);
        if(Ex.indexIn(s) != -1){
            BeginTime=Ex.cap(1);
        }
        keyword=Code+"-EndTime=([0-9]+)";
        Ex.setPattern(keyword);
        if(Ex.indexIn(s) != -1){
            EndTime=Ex.cap(1);
        }
        keyword=Code+"-SiteType=([0-9]+)";
        Ex.setPattern(keyword);
        if(Ex.indexIn(s) != -1){
            SiteType=Ex.cap(1);
        }

        sql="select count(*) from [Threshold] where [Code]='"+Code+"' and [DateType]='"+DateType+"'";
        query.exec(sql);
        int rec=-1;
        if(query.next())rec=query.value(0).toInt();
        if(rec==0){
            sql=QString("insert into [Threshold]([Code],[Value],[DateType],[BeginTime],[EndTime],[SiteType])values('%1','%2','%3','%4','%5','%6')" )
                    .arg(Code)
                    .arg(Value)
                    .arg(DateType)
                    .arg(BeginTime)
                    .arg(EndTime)
                    .arg(SiteType);
        }else if(rec>0){
            sql="update [Threshold] set";
            sql+=" [Value]='"+Value+"',";
            sql+="[DateType]='"+DateType+"',";
            sql+="[BeginTime]='"+BeginTime+"',";
            sql+="[EndTime]='"+EndTime+"',";
            sql+="[SiteType]='"+SiteType+"',";
            sql+=" where [Code]='"+Code+"'";
            sql+=" and [DateType]='"+DateType+"'";
        }
        query.exec(sql);
//        pos+=Ex.matchedLength();
    }
    return pos;

}
//收到平台信息并处理
int Message::messageProc(QString str,QextSerialPort *com,QTcpSocket *tcp)
{
    QString str_MN="MN=([0-9A-Za-z]+)";
    QString str_CN="CN=([0-9]+)";
    QString str_Flag="Flag=([0-9])";
    QString str_QN="QN=([0-9A-Za-z]+)";
    QString str_CardNo="CardNo=([0-9A-Za-z]+)";
    QString str_CardType="CardType=([0-9]+)";
    QString str_WarnTime="WarnTime=([0-9]+)";
    QString str_PW="&&PW=([0-9A-Za-z]+)";
    QString str_SystemTime="SystemTime=([0-9]+)";
    QString str_ReportTime="ReportTime=([0-9]+)";
    QString str_AlarmTarget="AlarmTarget=([0-9]+)";
    QString str_RtdInterval="RtdInterval=([0-9]+)";
    QString str_BeginTime="BeginTime=([0-9]+)";
    QString str_EndTime="EndTime=([0-9]+)";
    QString str_OverTime="OverTime=([0-9]+)";
    QString str_ReCount="ReCount=([0-9]+)";
    QString str_SampleInterval="SampleInterval=([0-9]+)";
//    QString str_UpLimit="-UpLimit=(\\d+\\.?\\d*)";
//    QString str_DownLimit="-DownLimit=(\\d+\\.?\\d*)";
    QRegExp Ex;
    QString data;
    QByteArray ch;
//    QSqlQuery query;
//    QSqlQuery query1;

//    QString sql;
//    QString Code;
    ch.resize(4);

    int len=str.length();
    if(len<=12)return -1;

    Ex.setPattern(str_QN);
    if(Ex.indexIn(str) != -1){
        QN=Ex.cap(1);
    }

    Ex.setPattern(str_CN);
    if(Ex.indexIn(str) != -1){
        CN=Ex.cap(1).toInt();
    }

    myAPI api;
    QString From;
    if(com==NULL){
        From=QString("%1:%2").arg(tcp->peerAddress().toString()).arg(tcp->peerPort());       
    }else{
        From="COM3";
    }
    data=QString("Rx[%1]:%2").arg(From).arg(str);
    api.InsertList(data);
    api.Insert_Message_Received(QN,CN,From,str);
    api.Update_Respond(QN,From);   //更新处理标志

    int crc=myHelper::CRC16_GB212(&str.toLatin1().data()[6],len-12);
    sprintf(ch.data(),"%04X",crc);
    if(str.right(6).left(4)!=QString(ch))return -1;

    Ex.setPattern(str_MN);
    if(Ex.indexIn(str) != -1){
        if(Ex.cap(1)!=myApp::MN)return -1;
    }
    else{
        return -1;
    }

    switch (CN)
    {

    case CN_SetAlarmTime:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }

        Ex.setPattern(str_WarnTime);
        if(Ex.indexIn(str) != -1){
            myApp::AlarmTime=Ex.cap(1).toInt();
            myApp::WriteConfig();
            ExecuteRespond(RESULT_SUCCESS,com,tcp);
        }
        else{
            ExecuteRespond(RESULT_NODATA,com,tcp);
        }
        break;

    case CN_Set_OverTime_ReCount:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }

        Ex.setPattern(str_OverTime);
        if(Ex.indexIn(str) != -1){
            myApp::OverTime=Ex.cap(1).toInt();
        }
        else {
            ExecuteRespond(RESULT_NODATA,com,tcp);
            return -1;
        }
        Ex.setPattern(str_ReCount);
        if(Ex.indexIn(str) != -1){
            myApp::ReCount=Ex.cap(1).toInt();
        }
        else {
            ExecuteRespond(RESULT_NODATA,com,tcp);
            return -1;
        }
        myApp::WriteConfig();
        ExecuteRespond(RESULT_SUCCESS,com,tcp);
        break;

    case CN_SetPW:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }

        Ex.setPattern(str_PW);
        if(Ex.indexIn(str) != -1){
            myApp::PW=Ex.cap(1);
            myApp::WriteConfig();
            ExecuteRespond(RESULT_SUCCESS,com,tcp);
        }
        else{
            ExecuteRespond(RESULT_NODATA,com,tcp);
        }
        break;

    case CN_GetTime:   

        SendCurrentTime(com,tcp);
        ExecuteRespond(RESULT_SUCCESS,com,tcp);
        break;

    case CN_SetTime: 

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }
        int year;
        int month;
        int day;
        int hour;
        int min;
        int sec;
        Ex.setPattern(str_SystemTime);
        if(Ex.indexIn(str) != -1){
            QByteArray dt=Ex.cap(1).toLatin1();
            year=(dt[0]-'0')*1000+
                 (dt[1]-'0')*100+
                 (dt[2]-'0')*10+
                 (dt[3]-'0');
            month=(dt[4]-'0')*10+
                  (dt[5]-'0');
            day=(dt[6]-'0')*10+
                (dt[7]-'0');
            hour=(dt[8]-'0')*10+
                 (dt[9]-'0');
            min=(dt[10]-'0')*10+
                (dt[11]-'0');
            sec=(dt[12]-'0')*10+
                (dt[13]-'0');
            if(messageSetTime(year,month,day,hour,min,sec)==true){
                ExecuteRespond(RESULT_SUCCESS,com,tcp);
            }
            else {
                ExecuteRespond(RESULT_FAILED,com,tcp);
            }
        }
        else{
            ExecuteRespond(RESULT_NODATA,com,tcp);
        };

        break;

    case CN_SetAlarmValue:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }
        SetAlarmValue(str);
        ExecuteRespond(RESULT_SUCCESS,com,tcp);
        break;

    case CN_GetAlarmValue:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }
        SendAlarmValue(str,com,tcp);
        ExecuteRespond(RESULT_SUCCESS,com,tcp);
        break;

    case CN_GetRtdData:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }
        SendRtdData_Slave(com,tcp);
        break;

    case CN_StopRtdData:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                NoticeRespond(com,tcp);
            }
        }
        break;

    case CN_GetStatus:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }
        SendStatus(com,tcp);
        break;

    case CN_StopStatus:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                NoticeRespond(com,tcp);
            }
        }
        break;

    case CN_SetAlarmTarget:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }

        Ex.setPattern(str_AlarmTarget);
        if(Ex.indexIn(str) != -1){
            myApp::AlarmTarget=Ex.cap(1);
            myApp::WriteConfig();
            ExecuteRespond(RESULT_SUCCESS,com,tcp);
        }
        else{
            ExecuteRespond(RESULT_NODATA,com,tcp);
        }
        break;

    case CN_GetAlarmTarget:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }
        SendAlarmTarget(com,tcp);
        ExecuteRespond(RESULT_SUCCESS,com,tcp);
        break;

    case CN_SetReportTime:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }

        Ex.setPattern(str_ReportTime);
        if(Ex.indexIn(str) != -1){
            myApp::ReportTime=QString("%1|%2")
                            .arg(Ex.cap(1).mid(0,2))
                            .arg(Ex.cap(1).mid(2,2));
            myApp::WriteConfig();
            ExecuteRespond(RESULT_SUCCESS,com,tcp);
        }
        else{
            ExecuteRespond(RESULT_NODATA,com,tcp);
        }
        break;

    case CN_GetReportTime:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }
        SendReportTime(com,tcp);
        ExecuteRespond(RESULT_SUCCESS,com,tcp);
        break;

    case CN_SetRtdInterval:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }

        Ex.setPattern(str_RtdInterval);
        if(Ex.indexIn(str) != -1){
            myApp::RtdInterval=Ex.cap(1).toInt();
            myApp::WriteConfig();
            ExecuteRespond(RESULT_SUCCESS,com,tcp);
        }
        else{
            ExecuteRespond(RESULT_NODATA,com,tcp);
        }
        break;

    case CN_GetRtdInterval:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }
        SendRtdInterval(com,tcp);
        ExecuteRespond(RESULT_SUCCESS,com,tcp);
        break;

    case CN_GetHourData:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }

        Ex.setPattern(str_BeginTime);
        if(Ex.indexIn(str) != -1){
            BeginTime=QDateTime::fromString(Ex.cap(1),"yyyyMMddhhmmss");
        }
        else {
            return -1;
        }

        Ex.setPattern(str_EndTime);
        if(Ex.indexIn(str) != -1){
            EndTime=QDateTime::fromString(Ex.cap(1),"yyyyMMddhhmmss");
        }
        else {
            return -1;
        }

        if(BeginTime>EndTime){
            ExecuteRespond(RESULT_FAILED,com,tcp);
            return -1;
        }
        Message::SendCountData_Slave(CN_SendHourData,com,tcp);
        Message::ExecuteRespond(1,com,tcp);
        break;

    case CN_GetDayData:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }

        Ex.setPattern(str_BeginTime);
        if(Ex.indexIn(str) != -1){
            BeginTime=QDateTime::fromString(Ex.cap(1),"yyyyMMddhhmmss");
        }
        else {
            return -1;
        }

        Ex.setPattern(str_EndTime);
        if(Ex.indexIn(str) != -1){
            EndTime=QDateTime::fromString(Ex.cap(1),"yyyyMMddhhmmss");
        }
        else {
            return -1;
        }

        if(BeginTime>EndTime){
            ExecuteRespond(RESULT_FAILED,com,tcp);
            return -1;
        }
        Message::SendCountData_Slave(CN_SendDayData,com,tcp);
        Message::ExecuteRespond(1,com,tcp);
        break;

    case CN_GetMinsData:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }

        Ex.setPattern(str_BeginTime);
        if(Ex.indexIn(str) != -1){
            BeginTime=QDateTime::fromString(Ex.cap(1),"yyyyMMddhhmmss");
        }
        else {
            return -1;
        }

        Ex.setPattern(str_EndTime);
        if(Ex.indexIn(str) != -1){
            EndTime=QDateTime::fromString(Ex.cap(1),"yyyyMMddhhmmss");
        }
        else {
            return -1;
        }

        if(BeginTime>EndTime){
            ExecuteRespond(RESULT_FAILED,com,tcp);
            return -1;
        }    
        Message::SendCountData_Slave(CN_SendMinsData,com,tcp);
        Message::ExecuteRespond(1,com,tcp);
        break;

    case CN_GetRunTimeData:

        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }

        Ex.setPattern(str_BeginTime);
        if(Ex.indexIn(str) != -1){
            BeginTime=QDateTime::fromString(Ex.cap(1),"yyyyMMddhhmmss");
        }
        else {
            return -1;
        }

        Ex.setPattern(str_EndTime);
        if(Ex.indexIn(str) != -1){
            EndTime=QDateTime::fromString(Ex.cap(1),"yyyyMMddhhmmss");
        }
        else {
            return -1;
        }

        if(BeginTime>EndTime){
            ExecuteRespond(RESULT_FAILED,com,tcp);
            return -1;
        }
        Message::SendRunTimeData(com,tcp);
        Message::ExecuteRespond(1,com,tcp);
        break;

    case CN_Sample_time :  //设置取样间隔
        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }
        Ex.setPattern(str_SampleInterval);
        if(Ex.indexIn(str) != -1){
            myApp::SampleIntervalTime=Ex.cap(1).toInt();
            myApp::WriteConfig();
            myApp::ReadConfig();
            ExecuteRespond(RESULT_SUCCESS,com,tcp);
        }
        break;

    case CN_Factor_control:   //远程采样
    {
        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }
        Ex.setPattern("PolID=w([0-9]+)");
        if(Ex.indexIn(str)!=1){
            switch (Ex.cap(1).toInt()) {
            case 1018:
                if(myApp::COD_Isok==false||myApp::NH3_Isok==false||myApp::Cu_Isok==false||myApp::Ni_Isok==false||myApp::Ge_Isok==false){
                    ExecuteRespond(RESULT_USING,com,tcp); //
                    return -1;
                    break;
                }
                myApp::COD_FLG=1;
                myApp::NH3_FLG=1;
                myApp::Ni_FLG=1;
                myApp::Cu_FLG=1;
                myApp::Ge_FLG=1;
                break;
            default: break;

            }
            if(myApp::COD_Isok==true)ExecuteRespond(RESULT_SUCCESS,com,tcp);

        }
        else{
            ExecuteRespond(RESULT_FAILED,com,tcp);
            return -1;
        }
        break;
    }
    case CN_Door_control:
    {
        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }
        //发送开门信号
        myApp::Door_FLG=1;
        ExecuteRespond(RESULT_SUCCESS,com,tcp);
        break;
    }
    case CN_Card_add:        //开卡
    {
        QString CardNo;
        QString CardType;
        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }
        Ex.setPattern(str_CardNo);
        if(Ex.indexIn(str) != -1){
            CardNo =Ex.cap(1);
            Ex.setPattern(str_CardType);
            if(Ex.indexIn(str) != -1){
                CardType=Ex.cap(1);
                api.Insert_Message_Card_Add(QN,CardNo,CardType);
                myApp::Addcard_FLG=1;
                ExecuteRespond(RESULT_SUCCESS,com,tcp);
            }
        }
        else{
            ExecuteRespond(RESULT_FAILED,com,tcp);
        }
        break;
    }
    case CN_Card_delete:        //销卡
    {
        QString CardNo;
        QString CardType;
        Ex.setPattern(str_Flag);
        if(Ex.indexIn(str) != -1){
            if(Ex.cap(1).toInt()&0x01){//Flag=1
                RequestRespond(REQUEST_READY,com,tcp);
            }
        }
        Ex.setPattern(str_CardNo);
        if(Ex.indexIn(str) != -1){
            CardNo =Ex.cap(1);
            Ex.setPattern(str_CardType);
            if(Ex.indexIn(str) != -1){
                CardType=Ex.cap(1);
                api.Update_Message_Card_delete(CardNo,CardType);
                myApp::Deletecard_FLG=1;
                sleep(10);

                ExecuteRespond(RESULT_SUCCESS,com,tcp);
            }
        }
        else{
            ExecuteRespond(RESULT_FAILED,com,tcp);
        }
        break;
    }


    default:
        break;
    }

    return true;

}





