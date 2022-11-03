#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>

namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_btn_login1_clicked();

    void on_btn_zhuce1_clicked();

    void on_btn_exit_clicked();

signals:
     void show1_main();
     void show_zhu();
     void show3_zhu();
     void MainWindow();
     void show4_xuanze();

     //void show_xuanze();

private:
    Ui::login *ui;
    void paintEvent(QPaintEvent *e);
};

#endif // LOGIN_H
