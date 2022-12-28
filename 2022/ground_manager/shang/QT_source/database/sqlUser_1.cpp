#include "sqlUser.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDebug>

sqlUser::sqlUser()
{
    //创建数据库

}
bool sqlUser::initTable()
{
    //检测连接名是否存在
    if(QSqlDatabase::contains("qt_sql_default_connection")){
        sqlDatabase = QSqlDatabase::database("qt_sql_default_connection");
    }else{
        sqlDatabase = QSqlDatabase::addDatabase("QSQLITE");
        sqlDatabase.setDatabaseName("Parking_lot.db");
        sqlDatabase.setPassword("123");
        loger::getInstance().write("创建数据库文件Parking_lot.db");
        qDebug()<<"数据库创建成功";
    }

    if(!sqlDatabase.open()){
        loger::getInstance().write("Parking_lot.db数据库打开错误");
        return false;
    }
    loger::getInstance().write("数据库打开成功");
    qDebug()<<"数据库打开成功";

    if(!isExistTable("UserInfo")){
        createTable("UserInfo");
    }
    return false;
}

bool sqlUser::isExistTable(QString table)
{
    bool bRet = false;
    if(!sqlDatabase.open()){
        return bRet;
    }
    QSqlQuery query(sqlDatabase);
    query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg(table));
    if(query.next()){
        if(query.value(0).toInt() > 0){
            bRet = true;
        }
    }
    return false;
}

bool sqlUser::createTable(QString table)
{
    if(!sqlDatabase.open()){
        return false;
    }
    QSqlQuery query(sqlDatabase);
    if(table == QString("UserInfo")){

        bool success = query.exec("CREATE TABLE IF NOT EXISTS UserInfo("
                                  "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                  "Name               CHAR(40),"
                                  "CarNumber          CHAR(40) UNIQUE,"
                                  "PhoneNumber        CHAR(40) UNIQUE,"
                                  "Age                CHAR(40),"
                                  "Sex                CHAR(40),"
                                  "Balance            REAL,"
                                  "password           CHAR(40));");
        if(success){
            loger::getInstance().write("UserInfo表创建成功");
            qDebug()<<table<<"创建成功";
            return true;
        }
        else{
            QSqlError lastError = query.lastError();
            loger::getInstance().write(lastError.text());
            return false;
        }
    }
    else{
        return false;
    }
}



bool sqlUser::isExist(QString field, QString value)
{
    QString sqlStr = "Select 1 from UserInfo where %1 = :%2";
    sqlStr = sqlStr.arg(field).arg(value);
    auto sqlquery = QSqlQuery(sqlDatabase);
    sqlquery.prepare(sqlStr);
    sqlquery.bindValue(0,value);
    sqlquery.exec();
    return  sqlquery.next();
}

carUser sqlUser::GetUserInfoByCarNum(QString carNumber)
{
    QString sqlStr = "Select * From UserInfo where carNumber = :carNumber";
    auto query = QSqlQuery(sqlDatabase);
    query.prepare(sqlStr);
    query.bindValue(":carNumber",carNumber);
    if(!query.exec()){
        qDebug()<<query.lastError();
        loger::getInstance().write(QString("[FIND carNumber = %1 ERROR!]").arg(carNumber) + query.lastError().text());
    }
    carUser car;
    while(query.next()){
        car.ID = query.value("ID").toInt();
        car.Name = query.value("Name").toString();
        car.CarNumber = query.value("CarNumber").toString();
        car.PhoneNumber = query.value("PhoneNumber").toString();
        car.Age = query.value("Age").toString();
        car.Sex = query.value("Sex").toString();
        car.Balance = query.value("Balance").toFloat();
    }
    query.finish();
    return car;
}

carUser sqlUser::GetUserInfoByPhoneNumber(QString PhoneNumber)
{
    QString sqlStr = "Select * From UserInfo where PhoneNumber = :PhoneNumber";
    auto query = QSqlQuery(sqlDatabase);
    query.prepare(sqlStr);
    query.bindValue(":PhoneNumber",PhoneNumber);
    qDebug()<<"电话号码："<<PhoneNumber;
    if(!query.exec()){
        loger::getInstance().write(QString("[FIND PhoneNumber = %1 ERROR!]").arg(PhoneNumber) + query.lastError().text());
    }
    carUser car;
    while(query.next()){
        car.ID = query.value("ID").toInt();
        car.Name = query.value("Name").toString();
        car.CarNumber = query.value("CarNumber").toString();
        car.PhoneNumber = query.value("PhoneNumber").toString();
        car.Age = query.value("Age").toString();
        car.Sex = query.value("Sex").toString();
        car.Balance = query.value("Balance").toFloat();
    }
    query.finish();
    return car;
}

vector<carUser> sqlUser::GetAllUsers()
{
    auto query = QSqlQuery(sqlDatabase);
    query.exec("Select * From UserInfo");
    vector<carUser> users;
    while(query.next()){
        carUser car;
        car.ID = query.value("ID").toInt();
        car.Name = query.value("Name").toString();
        car.CarNumber = query.value("CarNumber").toString();
        car.PhoneNumber = query.value("PhoneNumber").toString();
        car.Age = query.value("Age").toString();
        car.Sex = query.value("Sex").toString();
        car.Balance = query.value("Balance").toFloat();
        users.push_back(car);
    }
    return users;
}

void sqlUser::addCarUser(const carUser &car)
{
    auto query = QSqlQuery(sqlDatabase);
    query.prepare("Insert into UserInfo (ID, Name, CarNumber, PhoneNumber, Age,Sex, Balance) "
                  "values (:ID, :Name, :CarNumber, :PhoneNumber, :Age, :Sex, :Balance)");
    query.bindValue(0,car.ID);
    query.bindValue(1,car.Name);
    query.bindValue(2,car.CarNumber);
    query.bindValue(3,car.PhoneNumber);
    query.bindValue(4,car.Age);
    query.bindValue(5,car.Sex);
    query.bindValue(6,car.Balance);
    if(!query.exec()){
        loger::getInstance().write("[INSERT ERROR!]" + query.lastError().text());
    }
}

void sqlUser::removeCarUser(int userId)
{
    auto query = QSqlQuery(sqlDatabase);
    query.prepare("delete from UserInfo where ID = :ID");
    query.bindValue(0,userId);
    if(!query.exec()){
        loger::getInstance().write(QString("[REMOVE ID = %1 ERROR!]").arg(userId) + query.lastError().text());
    }
}

void sqlUser::modifyCarUser(const carUser &car)
{
    auto query = QSqlQuery(sqlDatabase);
    query.prepare("update UserInfo set Name = :Name, CarNumber = :CarNumber, PhoneNumber = :PhoneNumber, "
                  " Age = :Age, Sex = :Sex, Balance = :Balance where ID = :ID");
    query.bindValue(":ID",car.ID);
    query.bindValue(":Name",car.Name);
    query.bindValue(":CarNumber",car.CarNumber);
    query.bindValue(":PhoneNumber",car.PhoneNumber);
    query.bindValue(":Age",car.Age);
    query.bindValue(":Sex",car.Sex);
    query.bindValue(":Balance",car.Balance);
//    qDebug()<<car.Name;
    if(!query.exec()){
        loger::getInstance().write("[UPDATE ERROR!]" + query.lastError().text());
        qDebug()<<"更新失败";
    }
    qDebug()<<"更新成功";
}

int sqlUser::getNewId()
{
    QString sqlStr = "Select IFNULL(MAX(ID), 0) as newId From UserInfo";
    auto query = QSqlQuery(sqlDatabase);
    query.exec(sqlStr);
    if(query.next()){
        return query.value(0).toInt() + 1;
    }
    else {
        return 1;
    }
}
