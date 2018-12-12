#ifndef FRMCONFIG_H
#define FRMCONFIG_H

#include <QWidget>
#include <QStandardItemModel>

namespace Ui {
class frmconfig;
}

class frmconfig : public QWidget
{
    Q_OBJECT
    
public:
    explicit frmconfig(QWidget *parent = 0);
    ~frmconfig();

private slots:
    void on_btn_general_clicked();

    void on_btn_parameter_clicked();

    void on_btn_serial_clicked();

    void on_btn_site_clicked();

    void on_btn_user_clicked();

    void on_btn_updateSystem_clicked();

    void on_comboBox_serial_currentIndexChanged(int index);

    void on_listUser_currentRowChanged(int currentRow);

    void on_btn_addUser_clicked();

    void on_btn_deleteUser_clicked();

    void on_btn_updateUser_clicked();

//    void on_btn_updatePwd_clicked();

    void on_btn_addPara_clicked();

    void on_btn_deletePara_clicked();

    void on_btn_Cancel_clicked();

    void on_btn_datetime_clicked();

    void on_btn_localNet_clicked();

    void on_btnChange_clicked();

    void on_btn_SaveGeneral_clicked();

    void on_btn_SaveSerial_clicked();

    void on_btn_SaveSite_clicked();

    void on_comboBox_channel_currentIndexChanged(int index);

    void on_btn_SaveLocalNet_clicked();

    void on_btn_Autogetip_clicked();

    void on_btn_io_clicked();

    void on_btn_SaveIo_clicked();

private:
    Ui::frmconfig *ui;

    void InitStyle();
    void InitForm();

    QStringList TempUserName;   //用户姓名链表
    QStringList TempUserInfo;      //用户信息链表
    QStandardItemModel  *paraInfoModel;

    void ReadUserInfo();
    bool AddUserIsLegal();
    bool AddParaIsLegal();
    void InitParaInfoModel();
    void ReadParaInfo();

};

#endif // FRMCONFIG_H
