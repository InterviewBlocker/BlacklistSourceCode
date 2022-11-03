#include "zhuce.h"
#include "ui_zhuce.h"
#include<QSqlDatabase>
#include<QtSql>
#include<QDebug>
#include<QMessageBox>
#include<QPainter>

zhuce::zhuce(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::zhuce)
{
    ui->setupUi(this);
    this->setWindowTitle("欢迎注册");
}

zhuce::~zhuce()
{
    delete ui;
}


void zhuce::on_pushButton_2_clicked()
{
    emit show3_zhu();

    this->close();


}

void zhuce::on_tn_ture_clicked()
{
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

        QSqlQuery query(db);
        query.exec(" select * from zhuce");
        int i=0;
        QString user2[50];
        QString pwd2[50];
        while(query.next())
        {

            user2[i]=query.value(0).toString();
            pwd2[i]=query.value(1).toString();
            i++;
          // --qDebug()<<query.value(0).toString()<<query.value(1).toString()<<endl;
        }

        for (int j=0;j<=i;j++) {
            qDebug()<<user2[j]<<endl;
            qDebug()<<pwd2[j]<<endl;
        }


//       -- while(query.next())
//        {
//        --  qDebug()<<query.value(0).toString()<<query.value(1).toString()<<endl;
//        }
       QString user=ui->txt_zhuce->text();//提取
       QString pwd=ui->txt_pwd->text();
       for (int j=0;j<i;j++) {
           if(user==user2[j])
           {
           qDebug()<<"账户已存在"<<endl;
          //-- emit show4_xuanze();
           QMessageBox::warning(this, "Warning!", "账户已存在",
                                QMessageBox::Yes);
           //this->close();
           return;
           }
       }

       if(user.size()<11||pwd.size()<6)
       {
           qDebug()<<"密码不能小于6位"<<endl;
          QMessageBox::warning(this, "Warning!", "密码小于6位或者账户小于11位",
                               QMessageBox::Yes);
           return;
       }
       else
       {
           QMessageBox::warning(this, "success!", "注册成功",
                                QMessageBox::Yes);

       emit MainWindow();
           this->close();
       }
       qDebug()<<user<<endl;
       qDebug()<<pwd<<endl;
       //QString sql=QString(" insert into  zhuce(zhu,pwd) values(51111,52222)");
       QString sql=QString(" insert into  zhuce(zhu,pwd) values(%1,%2)").arg(user).arg(pwd);

       query.exec(sql);

        db.close();
}
void zhuce::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);




}
