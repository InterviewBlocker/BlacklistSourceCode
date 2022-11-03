#include "duoliao.h"
#include "ui_duoliao.h"
#include<QPainter>
#include<QtSql>
duoliao::duoliao(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::duoliao)
{
    ui->setupUi(this);

    this->setWindowTitle("群聊");
}

duoliao::~duoliao()
{
    delete ui;
}

 void duoliao::on_btn_tui1_clicked()//多聊界面点击返回键--槽函数
{
    emit show4_xuanze();//切换至选择页面
    this->close();//关闭多聊界面
}
void duoliao::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);
}
