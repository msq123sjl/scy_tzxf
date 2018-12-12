#ifndef FRMSAMPLER_H
#define FRMSAMPLER_H

#include <QWidget>

namespace Ui {
class frmSampler;
}

class frmSampler : public QWidget
{
    Q_OBJECT
    
public:
    explicit frmSampler(QWidget *parent = 0);
    ~frmSampler();
    
private slots:

    void on_btn_GetSamplerStatus_clicked();

    void on_btn_Cancel_clicked();

    void on_btn_SyncSampling_clicked();

    void on_btn_OverSampling_clicked();

private:
    Ui::frmSampler *ui;
    bool GetSamplerStatus();
    int port;

    void InitStyle();
    void InitForm();

};

#endif // FRMSAMPLER_H
