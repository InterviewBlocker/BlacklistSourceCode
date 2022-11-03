#include "lianxi.h"
#include "ui_lianxi.h"
#include<QSqlDatabase>
#include<QtSql>
#include<QPainter>
#include<QMessageBox>

lianxi::lianxi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::lianxi)
{
    ui->setupUi(this);
    this->setWindowIcon((QIcon(":/image/1.png")));
           //设置名称
           this->setWindowTitle("zsq");
}


void lianxi::on_pushButton_clicked()
{
    QString user[50]={0},user_add[50]={0};
    int count = 0;
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
        /*QString user3[50];*///定义用户数量
        QString pwd3[50];//定义密码数量
        while(query.next())
        {

            user[i]=query.value(0).toString();//将从数据库中读取的账号和密码暂存到数组中
            pwd3[i]=query.value(1).toString();
            i++;

          //qDebug( )<<query.value(0).toString()<<query.value(1).toString()<<endl;
        }
        count = i;
        int c=0,f=0;
        for (int m=0;m<count;m++)
        {
            f=0;
            for (int n=0;n<user_num;n++)
            {
                if(user[m]==user3[n])
                {
                  f=1;
                  break;
                }
            }
            if(f==0)
            {
                user_add[c++]=user[m];
            }
        }
        for (int a=user_num;a<user_num+c;a++) {
            user3[a]=user_add[a-user_num];
        }
        qDebug()<<"c="<<c;
        if(count!=user_num)
        {
           user_num=count;
           flag=0;
        }

        qDebug()<<count;
        QVector<QToolButton *>vector;
    if(flag==0)
    {
        for(int i=0;i<c;i++)
        {
            QToolButton *btn[count];
            btn[i]=new QToolButton(this);

            btn[i]->setText(user_add[i]);
            btn[i]->setAutoRaise(true);

            ui->verticalLayout->addWidget(btn[i]);

            qDebug()<<QString(count);
            btn[i]->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            vector.push_back(btn[i]);
            Isshow.push_back(false);



        }
        //ui->sum_num->setText(QString(count));
        ui->label_2->setText(QString::number(count));
        flag=1;

    }
    for(int i=0;i<10;i++)
    {
        connect(vector[i],&QToolButton::clicked,[=](){
          emit show1_main();
            this->close();

           if(Isshow[i])
           {
                QMessageBox::warning(this,"警告","重复打开聊天框");
                return;
            }
//           Isshow[i]=true;
//                   QQ *qq=new QQ(nullptr,vector[i]->text());
//                   qq->setWindowIcon(vector[i]->icon());
//                   qq->setWindowTitle(vector[i]->text());
//                   qq->show();
//                   //关闭时将isshow至为false,解决关闭后依然显示警告
//                   connect(qq,&QQ::closeQQ,this,[=](){
//                       Isshow[i]=false;
//                   });
        });
    }
}



lianxi::~lianxi()
{
    delete ui;
}



void lianxi::on_btn_tui2_clicked()//联系人界面点击返回键--槽函数
{
    emit show4_xuanze();//切换至选择页面
    this->close();//关闭联系人界面
}
void lianxi::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);
}

