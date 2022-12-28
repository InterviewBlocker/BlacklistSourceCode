#include "register.h"
#include "ui_register.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QString>
#include <QTime>
#include"QRegExpValidator"
#include"QValidator"
Register::Register(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    this->setWindowTitle("用户注册");
    this->setWindowIcon(QIcon(":/pic/logo.ico"));
    this->setFixedSize(379,349);

    ui->z_UserPassword->setEchoMode(QLineEdit::Password);
    ui->z_UserPasswordSure->setEchoMode(QLineEdit::Password);

    z_sqlUser = new QSqlQuery;
    account = new sqlAccount;
}

Register::~Register()
{
    delete ui;
}
//判断手机号码是否合法
bool Register::IsValidPhoneNumber(const QString &phoneNum)
{
    QRegExp regx("^1([358][0-9]|4[579]|66|7[0135678]|9[89])[0-9]{8}$");
    QRegExpValidator regs(regx, 0);
    QString pNum = phoneNum;
    int pos = 0;
    QValidator::State res = regs.validate(pNum, pos);
    if (QValidator::Acceptable == res) {
        return true;
    }
    else {
        return false;
    }
}

void Register::on_z_pushButtonOk_clicked()
{
    //获取注册输入的相关信息
    QString z_passwordSure;
    admin.AdminName = ui->z_UserName->text();
    admin.RealName = ui->z_RealName->text();
    admin.PhoneNumber = ui->z_UserPhoneNumber->text();
    admin.Age = ui->z_UserAge->text().toInt();
    admin.Password = ui->z_UserPassword->text();
    z_passwordSure = ui->z_UserPasswordSure->text();


//    qDebug()<<checkValid();


    qDebug()<<"注册";
    if(admin.AdminName.isEmpty() || admin.PhoneNumber.isEmpty() || admin.Password.isEmpty() || z_passwordSure.isEmpty() || ui->z_UserAge->text().isEmpty() || admin.RealName.isEmpty()){
        QMessageBox::warning(this, "注册", "请将信息输入完整!!!");
        return;
    }else if(ui->z_UserPassword->text() != z_passwordSure){
        QMessageBox::warning(this, "注册", "两次输入密码不一致！");
    }else{
        if(checkValid()){
            //建立数据库将用注册用户的数据存入数据库
            admin.ID = account->getNewId();

            account->addAdmin(admin);

            QMessageBox::information(this, "注册", "注册成功");
            ui->z_UserName->clear();
            ui->z_RealName->clear();
            ui->z_UserPhoneNumber->clear();
            ui->z_UserAge->clear();
            ui->z_UserPassword->clear();
            ui->z_UserPasswordSure->clear();
        }

        }
}

bool Register::checkValid()
{
    sqlAccount sa;
    if(sa.isExist("AdminName", ui->z_UserName->text())){
        QMessageBox::warning(this, "注册用户","用户名已存在,请重新输入");
        ui->z_UserName->clear();
        return false;
    }else if(sa.isExist("PhoneNumber", ui->z_UserPhoneNumber->text())){
        QMessageBox::warning(this, "注册用户","电话号码已存在,请重新输入");
        ui->z_UserPhoneNumber->clear();
            return false;
    }else{

    }
    return true;
}

void Register::on_z_pushButtonExit_clicked()
{
    this->close();
}
