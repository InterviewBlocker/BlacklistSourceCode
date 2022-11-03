#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QPainter>
#include<QtSql>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{


    ui->setupUi(this);
    cli=new QUdpSocket(this);
    connect(cli,&QUdpSocket::readyRead,this,&MainWindow::recv_data);
    addr= QHostAddress("127.0.0.2");
    port=8002;
}

void MainWindow::recv_data()
{
    char data[50]={0};

    cli->readDatagram(data,50,&addr,&port);
    ui->txt_recv->append(data);


}


MainWindow::~MainWindow()
{
    delete ui;
}
//QString MainWindow::getName()
//{
//    return this->myname;
//}

void MainWindow::on_btn_Send_clicked()//maindow聊天界面点击发送按钮--槽函数
{
    cli->writeDatagram(ui->txt_send->text().toUtf8(),50,addr,port);//发送输入文本框的数据
    //ui->txt_recv->setAlignment(Qt::AlignLeft);
    ui->txt_recv->append(ui->txt_send->text());
    //ui->txt_recv->setAlignment(Qt::AlignRight);
    ui->txt_send->clear();


}

void MainWindow::on_btn_tui_clicked()//maindow聊天界面点击返回按钮--槽函数
{
    //emit show1_main();
    emit show_lianxi();
    this->close();
}
void MainWindow::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);
}

