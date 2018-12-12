#ifndef FRMEVENT_H
#define FRMEVENT_H

#include <QWidget>
#include "api/mysqliteapi.h"

namespace Ui {
class frmevent;
}

class frmevent : public QWidget
{
    Q_OBJECT
    
public:
    explicit frmevent(QWidget *parent = 0);
    ~frmevent();
    
private slots:
    void on_btnOk_clicked();

    void on_btnCancel_clicked();

private:
    Ui::frmevent *ui;

    void InitStyle();
    void InitForm();
    mySqliteAPI *Sqlite;              //数据库操作对象
    QString columnNames[4];     //列名数组
    int columnWidths[4];            //列宽数组

};

#endif // FRMEVENT_H
