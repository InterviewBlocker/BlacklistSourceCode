#include "login.h"
#include "ui_login.h"
#include<QDebug>
#include<QSqlDatabase>
#include<QtSql>
#include<QMessageBox>
#include<QPainter>
login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    this->setWindowTitle("欢迎使用");

}

login::~login()
{
    delete ui;
}
// :\QTgc\build-mywindowfa-Desktop_Qt_5_14_2_MinGW_32_bit-Debug\debug> 打开另一个客户端
void login::on_btn_login1_clicked()
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
        QString user[50];//定义用户数量
        QString pwd[50];//定义密码数量
        while(query.next())
        {

            user[i]=query.value(0).toString();//将从数据库中读取的账号和密码暂存到数组中
            pwd[i]=query.value(1).toString();
            i++;
          //qDebug()<<query.value(0).toString()<<query.value(1).toString()<<endl;
        }

        for (int j=0;j<=i;j++) {
            qDebug()<<user[j]<<endl;//输出数据库中的账号
            qDebug()<<pwd[j]<<endl;//输出数据库中的密码
        }

     QMessageBox MBox;//定义弹窗
    //emit show1_main();
     //QString user=ui->txt_zhuce->text();
     QString user1=ui->txt_login->text();//抓取文本框输入的账号
     QString pwd1=ui->txt_pwd1->text();//抓取文本框输入的密码
     if(user1.isEmpty()||pwd1.isEmpty())//判断账号和密码文本框是否为空
     {
         qDebug()<<"密码或账户为空"<<endl;
        QMessageBox::warning(this, "Warning!", "用户名或密码不为空",
                             QMessageBox::Yes);//弹出警告弹窗

         return;

     }
     qDebug()<<user1<<endl;//输出登录界面文本框中抓取的账号
     qDebug()<<pwd1<<endl;//输出登录界面文本框中抓取的密码
     int flag=0;//定义一个索引
     for (int j=0;j<=i;j++) {
         if(user1==user[j]&&pwd1==pwd[j])//输入的账号密码与数组中的账号密码循环比对
         {
         qDebug()<<"登陆成功"<<endl;
         emit show4_xuanze();//如果输入的账号密码与数组中的账号密码循环比对成功切换到选择页面
         this->close();//关闭登录界面
         flag=1;//登录成功为1反之为0
         }
     }
     if(flag==0)//当flag=0时
     {
        qDebug()<<"密码或账户错误！";
     QMessageBox::warning(this, "Warning!", "用户名或密码错误",
                          QMessageBox::Yes);//弹出警告弹窗
     }

}

void login::on_btn_zhuce1_clicked()//登录页面点击注册按钮--槽函数
{

    emit show_zhu();//切换至注册页面
    this->close();//关闭登录页面
}

void login::on_btn_exit_clicked()//登录页面点击退出按钮--槽函数
{
    this->close();//关闭登录页面
}
void login::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);




}
