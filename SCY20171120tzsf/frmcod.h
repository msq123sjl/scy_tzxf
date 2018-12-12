#ifndef FRMCOD_H
#define FRMCOD_H

#include <QWidget>

namespace Ui {
class frmcod;
}

class frmcod : public QWidget
{
    Q_OBJECT
    
public:
    explicit frmcod(QWidget *parent = 0);
    ~frmcod();
    
private slots:
    void on_btn_Cancel_clicked();

    void on_btn_StartWork_clicked();

private:
    Ui::frmcod *ui;
    void InitStyle();

};

#endif // FRMCOD_H
