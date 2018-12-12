#include "frmcod.h"
#include "ui_frmcod.h"
#include "api/myhelper.h"
#include "api/myapi.h"


frmcod::frmcod(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmcod)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    myHelper::FormInCenter(this,myApp::DeskWidth,myApp::DeskHeigth);//窗体居中显示
    this->InitStyle();
}

frmcod::~frmcod()
{
    delete ui;
}

void frmcod::InitStyle()
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
}


void frmcod::on_btn_Cancel_clicked()
{
    this->close();
}

void frmcod::on_btn_StartWork_clicked()
{
    myApp::COD_FLG=1;
    myApp::Ni_FLG=1;
    myApp::Cu_FLG=1;
    myApp::Ge_FLG=1;
}
