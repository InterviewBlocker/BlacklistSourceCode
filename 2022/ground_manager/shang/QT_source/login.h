#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QSqlQuery>
//#include <
#include "register.h"
#include "mainframe.h"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

namespace Ui { class Login; }

class Login : public QMainWindow
{
    Q_OBJECT

public:
    Login(QWidget *parent = nullptr);
    ~Login();

    QSqlQuery* query_3;

private slots:
    void on_pushButton_exit_clicked();
    void on_pushButton_register_clicked();
    void on_pushButton_login_clicked();


private:
    Ui::Login *ui;
    Register *z_register;
    MainFrame *Frame;

    QMainWindow *mainwindow;


//    Admin admin;
};



#endif // LOGIN_H
