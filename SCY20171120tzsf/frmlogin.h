#ifndef FRMLOGIN_H
#define FRMLOGIN_H

#include <QDialog>

namespace Ui {
class frmlogin;
}

class frmlogin : public QDialog
{
    Q_OBJECT
    
public:
    explicit frmlogin(QWidget *parent = 0);
    ~frmlogin();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::frmlogin *ui;

    void InitStyle();
    void InitForm();            //初始化界面

private slots:
    void on_btnLogin_clicked(); //用户按下登录按钮
    void on_btnClose_clicked(); //关闭界面

};

#endif // FRMLOGIN_H
