#include "register.h"
#include "ui_register.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QString>
#include <QTime>

Register::Register(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    setWindowTitle("用户注册");
    z_sqlUser = new QSqlQuery;
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
    QString z_userName, z_carNumber, z_phoneNumber, z_age, z_sex, z_password, z_passwordSure;
    z_userName = ui->z_UserName->text();
    z_carNumber = ui->z_UserCarNumber->text();
    z_phoneNumber = ui->z_UserPhoneNumber->text();
    z_age = ui->z_UserAge->text();
    z_sex = ui->z_UserSex->text();
    z_password = ui->z_UserPassword->text();
    z_passwordSure = ui->z_UserPasswordSure->text();

    if(z_userName.isEmpty() || z_carNumber.isEmpty() || z_phoneNumber.isEmpty() || z_password.isEmpty() || z_passwordSure.isEmpty() || z_age.isEmpty() || z_sex.isEmpty()){
        QMessageBox::warning(this, "注册", "请将信息输入完整!!!");
        return;
    }else {
        if(IsValidPhoneNumber(z_phoneNumber)){
            //建立数据库将用注册用户的数据存入数据库
            QString insert_buf;
            insert_buf = QObject::tr("insert into UserInfo values('%1', '%2', '%3', '%4', '%5', '%6')")
                    .arg(z_userName, z_carNumber, z_phoneNumber, z_age, z_sex, z_password);
            z_sqlUser->exec(insert_buf);

            QMessageBox::information(this, "注册", "注册成功");
            ui->z_UserName->clear();
            ui->z_UserName->clear();
            ui->z_UserCarNumber->clear();
            ui->z_UserPhoneNumber->clear();
            ui->z_UserAge->clear();
            ui->z_UserSex->clear();
            ui->z_UserPassword->clear();
            ui->z_UserPasswordSure->clear();
        }else{
            QMessageBox::warning(this, "注册", "请输入正确的手机号码!!!");
            return;
        }
    }
}

void Register::on_z_pushButtonExit_clicked()
{
    this->close();
}
