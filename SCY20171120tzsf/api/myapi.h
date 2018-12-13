#ifndef MYAPI_H
#define MYAPI_H

#include <QObject>
#include "api/myapp.h"
#include "api/message.h"
#include <QList>

#define DOWN_RETRY_CNT 2
class myAPI : public QObject
{
    Q_OBJECT
public:
    explicit myAPI(QObject *parent = 0);

    int   CountRecordIsExist(QString tableName,QString Time);
    int   CacheRecordIsExist(QString Code);
    bool TableIsExist(QString tableName);
    void RtdTableCreate(QString tableName);
    void CountDataTableCreate(QString tableName);
    double GetCountDataFromSql(QString tableName,QString StartTime,QString EndTime,QString field,QString func);
    double GetCountDataFromSql1(QString tableName,QString StartTime,QString EndTime,QString field,QString func);
    void CacheDataProc(double rtd,double total,QString flag,int dec,QString name,QString code,QString unit);
    void CacheCard(QString cardtype,QString cardno);
    void RtdProc();
    void AddEventInfo(QString TriggerType, QString TriggerContent);
    void AddEventInfoUser(QString TriggerContent);
    void MessageFromCom(int port);
    void ShowRtd();
    double AnalogConvert(double adValue,double RangeUp,double RangeLow,QString Signal);


     void MinsDataProc_WaterFlow(QString startTime,QString endTime);
     void MinsDataProc_WaterPara(QString startTime,QString endTime);
     void HourDataProc_WaterFlow(QString startTime,QString endTime);
     void HourDataProc_WaterPara(QString startTime,QString endTime);
     void DayDataProc(QString startTime,QString endTime);

     void Insert_Message_Count(int CN,int flag,QString dt);
     void Insert_Message_Rtd(int flag,QString dt);
     void Insert_Message_SampleBottle(int flag,int b1,int b2,int b3,int b4,int b5,int b6,int b7,int b8,int b9,int b10,int b11,int b12,int b13,int b14,int b15,int b16,int b17,int b18,int b19,int b20,int b21,int b22,int b23,int b24);
     void Update_Respond(QString QN,QString From);
     void Insert_Message_Received(QString QN,int CN,QString From,QString Content);
     void Insert_Message_Card_Add(QString QN,QString CardNo,QString CardType);
     void Update_Message_Card_delete(QString CardNo,QString CardType);
     void SendData_DoorRecord(int flag);

     void SendData_Master(int CN,int flag);
     void Protocol_1();
    void InsertList(QString str);
    void SendData_Status(int flag);

private:  
    Message a;   
    void Protocol_2(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_3(int port,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_4(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_5(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_6(int port,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_7(int port,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_8(int port,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_9(int port,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_10(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_11(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_12(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_13(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_14(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_15(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_16(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_17(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_18(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_19(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_20(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_21(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_22(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_23(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_24(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_25(int port);
    void Protocol_26(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_27(int port,int Address,int Dec,QString Name,QString Code,QString Unit,double alarm_max);
    void Protocol_28(int port);
    void Protocol_29(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_30(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_31(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_32(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_33(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_34(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_35(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_36(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_37(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_38(int port,int Address,int Dec,QString Name,QString Code,QString Unit);
    void Protocol_39(int port,int Address,int Dec,QString Name,QString Code,QString Unit);

    double HexToDouble(const unsigned char* buf);
    void delaymsec(int msec);

//    unsigned int getbitu(const unsigned char *buff, int pos, int len);
//    void DoubleToHex(double m,unsigned char* buf);



//signals:
//    void Message_4012_signal();



};

#endif // MYAPI_H
