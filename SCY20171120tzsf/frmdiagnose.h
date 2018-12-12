#ifndef FRMDIAGNOSE_H
#define FRMDIAGNOSE_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class frmdiagnose;
}

class frmdiagnose : public QWidget
{
    Q_OBJECT
    
public:
    explicit frmdiagnose(QWidget *parent = 0);
    ~frmdiagnose();


private:
    Ui::frmdiagnose *ui;

    QTimer *showTimer;
    int currentMsgCount;
    int maxMsgCount;

    void InitForm();

private slots:
    void on_btnCancel_clicked();
    void on_btnClear_clicked();
    void Append();

};

#endif // FRMDIAGNOSE_H
