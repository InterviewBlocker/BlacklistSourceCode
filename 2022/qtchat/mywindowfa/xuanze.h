#ifndef XUANZE_H
#define XUANZE_H

#include <QWidget>

namespace Ui {
class xuanze;
}

class xuanze : public QWidget
{
    Q_OBJECT

public:
    explicit xuanze(QWidget *parent = nullptr);
    ~xuanze();

private slots:
    void on_btn_tui1_clicked();

    void on_btn_dl_clicked();

    void on_btn_dr_clicked();

    void on_btn_lx_clicked();

signals:
     void show4_xuanze();
     void  show_xuanze();
     void  show1_main();
     void  show2_duolaio();
     void  show_lianxi();

    void on_btn_fan_clicked();

private:
    Ui::xuanze *ui;
    void paintEvent(QPaintEvent *e);
};

#endif // XUANZE_H
