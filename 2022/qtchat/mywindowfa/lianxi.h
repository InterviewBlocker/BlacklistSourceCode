#ifndef LIANXI_H
#define LIANXI_H

#include <QWidget>
#include<QPushButton>
#include <QToolButton>
#include <QIcon>
namespace Ui {
class lianxi;
}

class lianxi : public QWidget
{
    Q_OBJECT

public:
    explicit lianxi(QWidget *parent = nullptr);
    ~lianxi();
    QString user3[50]={0};
    QPushButton * btn[20];

    int flag=0;
    int user_num=0;

signals:
    void show_lianxi();
    void show4_xuanze();
    void show1_main();

private slots:
    void on_btn_tui2_clicked();

    void on_pushButton_clicked();

private:
    Ui::lianxi *ui;
     QVector<bool>Isshow;
     void paintEvent(QPaintEvent *e);
};

#endif // LIANXI_H
