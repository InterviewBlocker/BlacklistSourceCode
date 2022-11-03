#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ser= new QUdpSocket(this);//创建了一个基于udp/ip协议的通信节点
    ser->bind(QHostAddress("127.0.0.2"),8002);

    connect(ser,&QUdpSocket::readyRead,this,&MainWindow::recv_data);
}

void MainWindow::recv_data()
{

    char data[50]={0};
    ser->readDatagram(data,50,&addr,&port);
    if(i==2)
        i=0;
    if(map.contains(port)==false)
    {
        map.insert(port,addr);
    }
    if(strcmp("port",data)==0)
    {
       p[i++]=port;
    }
    else
    {
        if(port==p[0])
            ser->writeDatagram(data,50,addr,p[1]);
        else
            ser->writeDatagram(data,50,addr,p[0]);
    }
    ui->txt_recv->append(data);
    qDebug()<<p[0]<<p[1];
}
MainWindow::~MainWindow()
{
    ser->close();
    delete ui;
}


void MainWindow::on_btn_Send_clicked()
{

    ser->writeDatagram(ui->txt_send->text().toUtf8(),50,addr,port);
}
