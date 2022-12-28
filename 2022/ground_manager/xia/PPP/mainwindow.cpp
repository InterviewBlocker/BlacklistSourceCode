#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"QFile"
#include"QTextStream"
#include"QtDebug"
#include"iostream"
using namespace std;
my_thread1 *tid1;
Ui::MainWindow *ui1;
#include"QProcess"
#include <QFile>
#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QIODevice>

QNetworkAccessManager *manager;
QNetworkReply *reply;
QUdpSocket *udp;
QFile *f;

MainWindow::MainWindow(QWidget *parent)    : QMainWindow(parent)    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //新建网络请求管理器
    manager = new QNetworkAccessManager(this);
    //关联finished请求完成信号
    //connect(manager,&QNetworkAccessManager::finished,this,&MainWindow::replayFinished);

    my_thread *tid = new
    my_thread(this);
    ui1=ui;

    tid->start();
    tid1 = new    my_thread1(this);

    //创建UDP 通信对象
    udp = new QUdpSocket(this);
    //绑定本地所有地址
    udp->bind(QHostAddress::Any,6666);
    //关联可读信号
    connect(udp, &QUdpSocket::readyRead,this,&MainWindow::readPendingDatagrams);
}

//读取UDP 数据报
void MainWindow::readPendingDatagrams()
{
    //判断是否有数据可读
    while (udp->hasPendingDatagrams())
    {
        char msg[1024]={0};
        udp->readDatagram(msg,1024);
        if(msg[0] == 'C' && msg[1] == 'P')
        {
            QString dd;
            QString msg1 = QString(msg).remove("CP:");
            qDebug() << msg1;

            if(msg1[0] == 'J')
            {
                QString msg2 = QString(msg1).remove("J");
                ui->label_4->setText(msg2);

                dd = QString("http://api.dzzui.com/api/tts?txt=欢迎光临%1").arg(msg2);
                reply = manager->get(QNetworkRequest(QUrl(dd)));

                f = new QFile("1.mp3");
                 f->open(QIODevice::WriteOnly|QIODevice::Truncate);

                //关联可读信号
                connect(reply, &QIODevice::readyRead, this, [=](){
                    f->write(reply->readAll());
                });

               connect(reply,SIGNAL(downloadProgress(qint64, qint64 )),this,SLOT(replayFinished(qint64, qint64)));
            }else if(msg1[0] == 'C')
            {
                QString msg2 = QString(msg1).remove("C");
                ui->label_4->setText(msg2);

                dd = QString("http://api.dzzui.com/api/tts?txt=%1慢走不送").arg(msg1);
                reply = manager->get(QNetworkRequest(QUrl(dd)));

                f = new QFile("1.mp3");
                 f->open(QIODevice::WriteOnly|QIODevice::Truncate);

                //关联可读信号
                connect(reply, &QIODevice::readyRead, this, [=](){
                    f->write(reply->readAll());
                });

               connect(reply,SIGNAL(downloadProgress(qint64, qint64 )),this,SLOT(replayFinished(qint64, qint64)));
            }
        }else{
            ui->label->setText(msg);

            QString dd = QString("http://api.dzzui.com/api/tts?txt=停车费用是%1元").arg(msg);
            reply = manager->get(QNetworkRequest(QUrl(dd)));

            f = new QFile("1.mp3");
             f->open(QIODevice::WriteOnly|QIODevice::Truncate);

            //关联可读信号
            connect(reply, &QIODevice::readyRead, this, [=](){
                f->write(reply->readAll());
            });

           connect(reply,SIGNAL(downloadProgress(qint64, qint64 )),this,SLOT(replayFinished(qint64, qint64)));
        }
    }
}

void MainWindow::replayFinished(qint64 rec, qint64 all)
{
    if(rec == all)//下载完毕
        {
            f->close();
            system("mplayer 1.mp3\n");
        }
}



void my_thread::run()
{
    QProcess *pro = new QProcess;
    pro->start("./main");
}

void my_thread1::run()
{
    QProcess *pro = new QProcess;
    pro->start("./main1");
    FILE *fp;
    fp=fopen("pipe1","r");
    if(fp==nullptr)
    {
        qDebug()<<"打开SB";
    }

    char buf[1024]={0};
    fread(buf,1,1024,fp);
    QString q(buf);
    ui1->label_4->setText(q);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//进库
void MainWindow::on_pushButton_clicked()
{
    QFile file("pipe");
    if (!file.open(QIODevice::ReadWrite| QIODevice::Text))
        return;
    QTextStream out(&file);
    out<<"ok";

    file.close();


}

//出库
void MainWindow::on_pushButton_2_clicked()
{
    QFile file("pipe");
    if (!file.open(QIODevice::ReadWrite| QIODevice::Text))
        return;
    QTextStream out(&file);
    out<<"no";

    file.close();
}

//刷卡出
void MainWindow::on_pushButton_4_clicked()
{

    //开启线程c
    tid1->start();

}

//刷卡进
void MainWindow::on_pushButton_3_clicked()
{
    //开启线程
    tid1->start();

}



