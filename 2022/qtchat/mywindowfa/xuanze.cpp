#include "xuanze.h"
#include "ui_xuanze.h"
#include<QPainter>
#include<QtSql>
xuanze::xuanze(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::xuanze)
{
    ui->setupUi(this);
    this->setWindowTitle("请选择");
}

xuanze::~xuanze()
{
    delete ui;
}

void xuanze::on_btn_tui1_clicked()
{
    emit show4_xuanze();
    this->close();
}



void xuanze::on_btn_dl_clicked()
{
    emit show1_main();
    //emit show
    this->close();
}

void xuanze::on_btn_dr_clicked()
{
    emit show2_duolaio();
    this->close();

}

void xuanze::on_btn_lx_clicked()
{
    emit show_lianxi();
    this->close();
}
void xuanze::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);




}
