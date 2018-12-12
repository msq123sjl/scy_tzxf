#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QTcpSocket>
#include "api/myapp.h"
#include "api/mythread.h"
#include "api/message.h"
#include "frmmain.h"

//延时，TIME_OUT是串口读写的延时单位ms
#define TIME_OUT 50
namespace Ui {
class frmMain;
}

class frmMain : public QMainWindow
{
    Q_OBJECT
    
public:

    explicit frmMain(QWidget *parent = 0);
    ~frmMain();


private:
    Ui::frmMain *ui;
    QLabel *labTime;    //当前时间标签
    QLabel *labWelcom;    //当前时间标签
    QLabel *labUser;    //当前时间标签
    QLabel *labIP;    //当前ip
    QTimer *timerDate;  //计算在线时长和显示当前时间定时器对象
    QTimer *timerSample; //定时读取WEB对采样器的指令
    QTimer *tcpSocketTimer1;
    QTimer *tcpSocketTimer2;
    QTimer *tcpSocketTimer3;
    QTimer *tcpSocketTimer4;
    QTimer *statusTimer;

    Message message_api;
    myAPI api;
    Uart1_Execute u1;
    Uart2_Execute u2;
    Uart3_Execute u3;
    Uart4_Execute u4;
    Uart5_Execute u5;
    Uart6_Execute u6;
    Control_Execute flow;
    RtdProc          rtdProc;
    Count              count;
    SendMessage Msend;
    DB_Clear db_clear;
    SPI_Read_ad  read_ad;



    //QString columnNames[11];     //列名数组
    int  columnWidths[6];        //列宽数组

    void InitStyle();
    void InitForm();
    void InitTcpSocketClient();
    void InitCOM(int port);
    void InitRtdTable();

private slots:
    void ShowDateTime();
    void displayError(QAbstractSocket::SocketError);
    void clientReadMessage();
    void updateclient();
//    void CheckStatus();
//    void ConnectServer();
    void ConnectServer1();
    void ConnectServer2();
    void ConnectServer3();
    void ConnectServer4();
    bool CheckSampleCmd();
//    void Valve_Status_Check();
    void ShowForm();
    void Status();
    void on_btnClearData_clicked();
    void updateClientStatusDisconnect();
//    void get_flow_trigger();
//    void Get_COD_NH_Flg();

//    void lookedUp(const QHostInfo &host);
};

#endif // FRMMAIN_H
