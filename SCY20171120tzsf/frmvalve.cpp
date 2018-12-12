#include "frmvalve.h"
#include "ui_frmvalve.h"
#include "api/gpio.h"
#include "api/myhelper.h"
#include "api/myapi.h"
#include "api/valve_control.h"

Valve_control *Manual_Valve_Control;

frmValve::frmValve(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmValve)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    myHelper::FormInCenter(this,myApp::DeskWidth,myApp::DeskHeigth);//窗体居中显示
    this->InitStyle();
}

frmValve::~frmValve()
{
    delete ui;
}

void frmValve::InitStyle()
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
}

//手动开集水
void frmValve::on_btn_ValveOpen_clicked()
{
    if (myHelper::ShowMessageBoxQuesion("确定开启集水阀吗?")==0){
        Manual_Valve_Control->Catchment_Valve_Open_Set();
    }
}
//手动关集水
void frmValve::on_btn_ValveClose_clicked()
{
    if (myHelper::ShowMessageBoxQuesion("确定关闭集水阀吗?")==0){
        Manual_Valve_Control->Catchment_Valve_Close_Set();
    }
}



void frmValve::on_btn_Cancel_clicked()
{
    this->close();
}

void frmValve::on_btn_drainValveOpen_clicked()
{
    if (myHelper::ShowMessageBoxQuesion("确定开启排水阀吗?")==0){
        Manual_Valve_Control->drain_Valve_Open_Set();
    }

}

void frmValve::on_btn_ValveClose_2_clicked()
{
    if (myHelper::ShowMessageBoxQuesion("确定关闭排水阀吗?")==0){
    Manual_Valve_Control->drain_Valve_Close_Set();
    }

}
