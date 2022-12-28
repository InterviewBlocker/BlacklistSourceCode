#include "zcarusermanage.h"
#include "ui_zcarusermanage.h"
#include <QPushButton>
#include <QString>
#include <QDebug>
#include <QMessageBox>

zcarUserManage::zcarUserManage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::zcarUserManage)
{
    ui->setupUi(this);
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(reject()));

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确定"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));
}

zcarUserManage::~zcarUserManage()
{
    delete ui;


}

void zcarUserManage::InitUI()
{
    ui->lin_userName->setText(car.Name);
    ui->line_carNumber->setText(car.CarNumber);
    ui->line_phoneNumber->setText(car.PhoneNumber);
    ui->line_age->setText(car.Age);
    ui->line_sex->setText(car.Sex);
    ui->spinBox_balance->setValue(car.Balance);
}

void zcarUserManage::accept()
{

    if(!checkValid()){
        return;
    }
    getValue();
    sqlUser su;
    if(isAdd){
        car.ID = su.getNewId();
        su.addCarUser(car);
    }
    else {

        su.modifyCarUser(car);
    }
    QDialog::accept();
}

void zcarUserManage::reject()
{
    QDialog::reject();
}

void zcarUserManage::getValue()
{
    car.Name = ui->lin_userName->text();
    car.CarNumber = ui->line_carNumber->text();

    car.PhoneNumber = ui->line_phoneNumber->text();
    car.Age = ui->line_age->text();
    car.Sex = ui->line_sex->text();
    car.Balance = ui->spinBox_balance->value();
}

bool zcarUserManage::checkValid()
{
    if(ui->lin_userName->text() == ""  || ui->line_carNumber->text() == "" || ui->line_phoneNumber->text() == ""){
        return false;
    }
    if(isAdd){
        sqlUser su;
        if(su.isExist("CarNumber", ui->line_carNumber->text())){
            QMessageBox::warning(this, "添加用户","车牌号已存在");
        }else if(su.isExist("PhoneNumber", ui->line_phoneNumber->text())){
            QMessageBox::warning(this, "添加用户","电话号码已存在");
//            return false;
        }
    }
    else{

    }
    return true;
}
