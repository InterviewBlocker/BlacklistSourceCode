#include "pay.h"
#include "ui_pay.h"
#include <QDebug>
#include <QMessageBox>

Pay::Pay(QWidget *parent) : QDialog(parent),
    ui(new Ui::Pay)
{
    ui->setupUi(this);
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(reject()));


    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确定"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));

}

Pay::~Pay()
{
    delete ui;
}

void Pay::accept()
{
    QMessageBox::information(this,"支付","支付成功");
    QDialog::accept();
}

void Pay::reject()
{
    QDialog::reject();
}
