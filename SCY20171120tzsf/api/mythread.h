#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include "api/myapi.h"
#include <QStandardItemModel>
#include <QDateTime>
#include "frmvalve.h"

#define	WATCHDOG_IOCTL_BASE	'W'
#define	WDIOC_KEEPALIVE		_IOR(WATCHDOG_IOCTL_BASE, 5, int)

class Uart1_Execute : public QThread
{
      Q_OBJECT
protected:
    void run();

};

class Uart2_Execute: public QThread
{
      Q_OBJECT
protected:
    void run();

};

class Uart3_Execute : public QThread
{
      Q_OBJECT
protected:
    void run();

};

class Uart4_Execute : public QThread
{
      Q_OBJECT
protected:
    void run();

};

class Uart5_Execute : public QThread
{
      Q_OBJECT
protected:
    void run();

};

class Uart6_Execute : public QThread
{
      Q_OBJECT
protected:
    void run();

};

class Count : public QThread
{
      Q_OBJECT
protected:
    void run();

private:
    QString startTime;
    QString endTime;
    QDateTime Before;
    QDateTime Now;
};

class RtdProc: public QThread
{
      Q_OBJECT
protected:
    void run();

};

class SendMessage: public QThread
{
      Q_OBJECT
protected:
    void run();
};

class SPI_Read_ad: public QThread
{
      Q_OBJECT
protected:
    void run();
};

class DB_Clear: public QThread
{
      Q_OBJECT
protected:
    void run();
};

class Control_Execute :public QThread
{
  Q_OBJECT
protected:
    void run();
private:
signals:
    void Flow_Trig();
    void Flow_Stop();
};

#endif // MYTHREAD_H
