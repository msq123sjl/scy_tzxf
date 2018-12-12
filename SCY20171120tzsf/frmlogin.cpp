#include "frmlogin.h"
#include "ui_frmlogin.h"
#include "api/myhelper.h"
#include "api/myapi.h"
#include "api/myapp.h"


frmlogin::frmlogin(QWidget *parent) :
    QDialog(parent),

    ui(new Ui::frmlogin)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    myHelper::FormInCenter(this,myApp::DeskWidth,myApp::DeskHeigth);
    this->setGeometry(QRect(160, 100, 480,259));//可设置窗口显示的方位与大小
//    myHelper::FormNotResize(this);
    this->InitStyle();
    this->InitForm();

}

frmlogin::~frmlogin()
{
    delete ui;
}

void frmlogin::InitStyle()
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);

}

bool frmlogin::eventFilter(QObject *obj, QEvent *event)
{
    //用户按下回车键,触发登录信号.
    if (event->type()==QEvent::KeyPress) {
        QKeyEvent *keyEvent=static_cast<QKeyEvent *>(event);
        if (keyEvent->key()==Qt::Key_Return || keyEvent->key()==Qt::Key_Enter) {
            this->on_btnLogin_clicked();
            return true;
        }
    }
    return QObject::eventFilter(obj,event);
}

void frmlogin::InitForm()
{

}


void frmlogin::on_btnLogin_clicked()
{
    QString UserName=ui->txtUserName->text();
    QString UserPwd=ui->txtUserPwd->text();
    if (UserName==""||UserPwd==""){
        myHelper::ShowMessageBoxError("用户名和密码不能为空,请重新输入!");
//        ui->txtUserPwd->setFocus();
        return;
    }

    QString sql;
    QSqlQuery query;
    QString rec="9999999";

    sql = "select [UserPwd],[UserType] from [UserInfo]  where [UserName]='"+UserName+"'";
    query.exec(sql);
    if(query.next()){
        rec=query.value(0).toString();
    }
    if (UserPwd.toUpper()==rec.toUpper()){
        //记录当前用户,写入到配置文件,下次启动时显示最后一次登录用户名.
        myApp::LastLoginer = ui->txtUserName->text();
        myApp::CurrentUserName = myApp::LastLoginer;
        myApp::CurrentUserPwd = UserPwd;
        myApp::CurrentUserType = query.value(1).toString();
        myApp::Login=true;
        this->close();
    }else{
        myHelper::ShowMessageBoxError("用户名或密码错误,请重新输入!");
//        ui->txtUserPwd->setFocus();
    }

}

void frmlogin::on_btnClose_clicked()
{
    this->close();
}
