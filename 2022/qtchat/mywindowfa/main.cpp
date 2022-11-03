#include "mainwindow.h"
#include "login.h"
#include <QApplication>
#include"zhuce.h"
#include<xuanze.h>
#include<duoliao.h>
#include<lianxi.h>
#include<QSqlDatabase>
#include<QtSql>
#include<QDebug>
#include<QPainter>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    zhuce w1;
    xuanze w2;
    duoliao w3;
    lianxi w4;
    //w.show();
    login lg;
    QObject::connect(&lg,&login::show1_main,&w,&MainWindow::show);//进入登陆页面

    QObject::connect(&lg,&login::show_zhu,&w1,&zhuce::show);//点注册进入注册界面
    QObject::connect(&w1,&zhuce::show3_zhu,&lg,&login::show);//点上一步回到登录页面
    QObject::connect(&w1,&zhuce::MainWindow,&lg,&login::show);//点击确定 注册成功回到登录页面

    QObject::connect(&lg,&login::show4_xuanze,&w2,&xuanze::show);//点登录进入选择页面
    QObject::connect(&w2,&xuanze::show4_xuanze,&lg,&login::show);//点推出进入登录页面

    QObject::connect(&w2,&xuanze::show1_main,&w,&MainWindow::show);//点击单人聊天进入聊天页面
    QObject::connect(&w,&MainWindow::show1_main,&w2,&xuanze::show);//点击返回进入选择页面

    QObject::connect(&w2,&xuanze::show2_duolaio,&w3,&duoliao::show);//点击多人聊天进入聊天页面
    QObject::connect(&w3,&duoliao::show4_xuanze,&w2,&xuanze::show);//点击返回进入选择页面

    QObject::connect(&w2,&xuanze::show_lianxi,&w4,&lianxi::show);//点击联系人进入联系人界面
    QObject::connect(&w4,&lianxi::show4_xuanze,&w2,&xuanze::show);//点击返回回到选择界面

    QObject::connect(&w4,&lianxi::show1_main,&w,&MainWindow::show);//点击聊天好友进入聊天界面
    QObject::connect(&w,&MainWindow::show_lianxi,&w4,&lianxi::show);//点击返回回到联系人界面

    w.cli->writeDatagram("port",50,w.addr,w.port);//发送端口号
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
        db.setHostName("127.0.0.1");
        db.setPort(3306);
        db.setDatabaseName("zsq");
        db.setUserName("root");
        db.setPassword("123456");
        bool ok = db.open();
        if (ok)
        {
         qDebug() << QString("数据库打开成功")<<endl;
        }
        else
        {
         qDebug() << "数据库打开失败!"<<db.lastError().text();
        }
        QSqlQuery result= db.exec(" select * from zhuce");
//        while(result.next())
//        {
//          qDebug()<<result.value(0).toString()<<result.value(1).toString()<<endl;
//        }
//        db.close();

    lg.show();
    return a.exec();
}

