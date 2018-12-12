#ifndef FRMDATA_H
#define FRMDATA_H

#include <QWidget>
#include "api/mysqliteapi.h"

namespace Ui {
class frmdata;
}

class frmdata : public QWidget
{
    Q_OBJECT
    
public:
    explicit frmdata(QWidget *parent = 0);
    ~frmdata(); 
    
private slots:
    void on_btnOk_clicked();
    void on_btnCancel_clicked();

    void on_comboBoxDataType_currentIndexChanged(int index);

private:
    Ui::frmdata *ui;

    void InitForm();
    void InitStyle();

    mySqliteAPI *Sqlite;        //数据库操作对象   
    QString columnNames[8];     //列名数组
    int columnWidths[8];        //列宽数组
};

#endif // FRMDATA_H
