#include "sqlAccount.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDebug>

sqlAccount::sqlAccount()
{

}

bool sqlAccount::initTable()
{
    if(QSqlDatabase::contains("qt_sql_default_connection")){
        sqlDatabase = QSqlDatabase::database("qt_sql_default_connection");
    }else{
        sqlDatabase = QSqlDatabase::addDatabase("QSQLITE");
        sqlDatabase.setDatabaseName("Parking_lot.db");
        sqlDatabase.setPassword("123");
        loger::getInstance().write("创建数据库文件Parking_lot.db");
//        qDebug()<<"数据库创建成功";
//        qDebug()<<"创建"
    }

    if(!sqlDatabase.open("Parking_lot.db","123")){
        loger::getInstance().write("Parking_lot.db数据库打开错误");
        return false;
    }

    loger::getInstance().write("数据库打开成功");
    if(!isExistTable("account")){
        createTable("account");
    }
    return false;
}

bool sqlAccount::isExistTable(QString table)
{
    bool bRet = false;
    if(!sqlDatabase.open("Parking_lot.db","123")){
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

bool sqlAccount::createTable(QString table)
{
    if(!sqlDatabase.open("Parking_lot.db","123")){
        return false;
    }
    QSqlQuery query(sqlDatabase);
    if(table == QString("account")){

        bool success = query.exec("CREATE TABLE IF NOT EXISTS account("
                                  "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                  "AdminName          CHAR(40) UNIQUE,"
                                  "RealName           CHAR(40),"
                                  "PhoneNumber        INTEGER UNIQUE,"
                                  "Age                INTEGER,"
                                  "Password           CHAR(40));");
//        qDebug()<<"创建SQLAccount" << success;
        if(success){
            loger::getInstance().write("account表创建成功");
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

bool sqlAccount::isExist(QString field, QString value)
{
//    qDebug()<<"判断是否存在";
    QString sqlStr = "Select * from account where %1 = :%2";
    sqlStr = sqlStr.arg(field).arg(value);
    auto sqlquery = QSqlQuery(sqlDatabase);
    sqlquery.prepare(sqlStr);
    sqlquery.bindValue(0, value);
    sqlquery.exec();
    return  sqlquery.next();
}

void sqlAccount::addAdmin(const AdminInfo &admin)
{
    auto query = QSqlQuery(sqlDatabase);
    query.prepare("Insert into account (ID, AdminName, RealName, PhoneNumber, Age, Password) "
                  "values (:ID, :AdminName, :RealName, :PhoneNumber, :Age, :Password)");
    query.bindValue(0,admin.ID);
    query.bindValue(1,admin.AdminName);
    query.bindValue(2,admin.AdminName);
    query.bindValue(3,admin.PhoneNumber);
    query.bindValue(4,admin.Age);
    query.bindValue(5,admin.Password);

//    query.

    if(!query.exec()){
        loger::getInstance().write("[INSERT ERROR!]" + query.lastError().text());
    }
}

int sqlAccount::getNewId()
{
    QString sqlStr = "Select IFNULL(MAX(ID), 0) as newId From account";
    auto query = QSqlQuery(sqlDatabase);
    query.exec(sqlStr);
    if(query.next()){
        return query.value(0).toInt() + 1;
    }
    else {
        return 1;
    }
}
