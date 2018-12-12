#ifndef FRMVALVE_H
#define FRMVALVE_H

#include <QWidget>

namespace Ui {
class frmValve;
}

class frmValve : public QWidget
{
    Q_OBJECT
    
public:
    explicit frmValve(QWidget *parent = 0);
    ~frmValve();

    void Catchment_Valve_Open_Set();
    void Catchment_Valve_Close_Set();
    void drain_Valve_Open_Set();
    void drain_Valve_Close_Set();

private slots:

    void on_btn_ValveOpen_clicked();

    void on_btn_ValveClose_clicked();

    void on_btn_Cancel_clicked();

    void on_btn_drainValveOpen_clicked();

    void on_btn_ValveClose_2_clicked();

private:
    Ui::frmValve *ui;

    void InitStyle();

};

#endif // FRMVALVE_H
