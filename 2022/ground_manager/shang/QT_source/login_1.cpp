#include "login.h"
#include "ui_login.h"
#include <QMessageBox>
#include <QDebug>

Login::Login(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Login)
{

    ui->setupUi(this);
    this->setWindowTitle("基于QT的停车收费系统");
    this->setWindowIcon(QIcon(":/pic/logo.ico"));
    query_3=new QSqlQuery;
    query_3->exec("create table account(acc vchar primary key,pw vchar)");
    query_3->exec("insert into account values('admin','admin')");
    query_3->exec("insert into account values('admin1','admin1')");
        query_3->exec("insert into account values('','')");

}

Login::~Login()
{
    delete ui;
}


void Login::on_pushButton_exit_clicked()
{
    this->close();
}

void Login::on_pushButton_register_clicked()
{
    z_register = new Register();
    z_register->show();
}

void Login::on_pushButton_login_clicked()
{

    int flag=0;
    QString inputacc=ui->lineEdit_account->text();
    QString inputpw=ui->lineEdit_pw->text();

    query_3->exec("select * from account");

    while(query_3->next()){
        if(inputacc==query_3->value(0)&&inputpw==query_3->value(1)){
            Frame = new MainFrame(this);
            Frame->show();
            this->hide();
            ui->lineEdit_account->clear();
            ui->lineEdit_pw->clear();
            flag=1;
        }
    }

    if(flag==0){
        QMessageBox::information(NULL, "Error", "用户名或密码错误，请重试");
    }

}

