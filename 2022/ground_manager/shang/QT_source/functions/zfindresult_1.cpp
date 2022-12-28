#include "zfindresult.h"
#include "ui_zfindresult.h"
#include <QDebug>

zFindResult::zFindResult(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::zFindResult)
{
    ui->setupUi(this);
    this->setWindowTitle("查询结果");
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
//    this->setWindowFlags(Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
//    connect(ui->pb_ok, SIGNAL())
//    resize(600*400);

//    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确定"));
}

zFindResult::~zFindResult()
{
    delete ui;
}

void zFindResult::InitUI()
{
    ui->le_name->setText(car.Name);
    ui->le_carNumber->setText(car.CarNumber);
    ui->le_phoneNumber->setText(car.PhoneNumber);
    ui->le_age->setText(car.Age);
    ui->le_sex->setText(car.Sex);
    ui->le_balance->setText(QString::number(car.Balance));
}

void zFindResult::setFindHintLabel(QString arg)
{
    ui->label_findHint->setText(arg);
}

void zFindResult::accept()
{
    qDebug()<<"hello";
    QDialog::accept();
}

