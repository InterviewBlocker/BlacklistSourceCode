#include "sqlCarState.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>

sqlCarState::sqlCarState()
{

}

bool sqlCarState::initTable()
{
    if(QSqlDatabase::contains("qt_sql_default_connection")){
        sqldatabase = QSqlDatabase::database("qt_sql_default_connection");
    }else{
        sqldatabase = QSqlDatabase::addDatabase("QSQLITE");
        sqldatabase.setDatabaseName("Parking_lot.db");
        sqldatabase.setPassword("123");
    }

    if(!sqldatabase.open("Parking_lot.db","123")){
        loger::getInstance().write("Parking_lot.db数据库打开错误");
        return false;
    }

    if(!isExistTable("sqlCarState")){
        createTable("sqlCarState");
    }
    return false;
}

bool sqlCarState::isExistTable(QString table)
{
    bool bRet = false;
    if(!sqldatabase.open("Parking_lot.db","123")){
        return bRet;
    }
    QSqlQuery query(sqldatabase);
    query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg(table));
    if(query.next()){
        if(query.value(0).toInt() > 0){
            bRet = true;
        }
    }
    return false;
}

bool sqlCarState::createTable(QString table)
{
    if(!sqldatabase.open("Parking_lot.db","123")){
        return false;
    }
    QSqlQuery query(sqldatabase);
    if(table == QString("sqlCarState")){

        bool success = query.exec("CREATE TABLE IF NOT EXISTS sqlCarState("
                                  "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                  "CarNumber          CHAR(40),"
                                  "State              CHAR(40),"
                                  "InTime             CHAR(40),"
                                  "OutTime            CHAR(40),"
                                  "StopTime           INTEGER);");
        if(success){
            loger::getInstance().write("sqlCarState表创建成功");
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

bool sqlCarState::isExist(QString field, QString value)
{
    QString sqlStr = "Select 1 from sqlCarState where %1 = :%2";
    sqlStr = sqlStr.arg(field).arg(value);
    auto sqlquery = QSqlQuery(sqldatabase);
    sqlquery.prepare(sqlStr);
    sqlquery.bindValue(0,value);
    sqlquery.exec();
    return  sqlquery.next();
}

carState sqlCarState::GetUserInfo(QString carNumber)
{
    QString sqlStr = "Select * From sqlCarState where carNumber = :carNumber";
    auto query = QSqlQuery(sqldatabase);
    query.prepare(sqlStr);
    query.bindValue(":carNumber",carNumber);
    if(!query.exec()){
        loger::getInstance().write(QString("[FIND carNumber = %1 ERROR!]").arg(carNumber) + query.lastError().text());
    }
    carState car;
    while(query.next()){
        car.ID = query.value("ID").toInt();
        car.State = query.value("State").toString();
        car.InTime = query.value("InTime").toString();
        car.OutTime = query.value("OutTime").toString();
        car.CarNumber = query.value("CarNumber").toString();
        car.StopTime = query.value("StopTime").toLongLong();
    }
    return car;
}

vector<carState> sqlCarState::GetUserInfoByState(QString State)
{
    QString sqlStr = "Select * From sqlCarState where State = :State";
    auto query = QSqlQuery(sqldatabase);
    query.prepare(sqlStr);
    query.bindValue(":State",State);
    if(!query.exec()){
        loger::getInstance().write(QString("[FIND State = %1 ERROR!]").arg(State) + query.lastError().text());
    }
    vector<carState> users;
    while(query.next()){
        carState car;
        car.ID = query.value("ID").toInt();
        car.State = query.value("State").toString();
        car.InTime = query.value("InTime").toString();
        car.OutTime = query.value("OutTime").toString();
        car.CarNumber = query.value("CarNumber").toString();
        car.StopTime = query.value("StopTime").toLongLong();
        users.push_back(car);
    }
    return users;
}

vector<carState> sqlCarState::GetAllCarState()
{
    auto query = QSqlQuery(sqldatabase);
    query.exec("Select * From sqlCarState");
    vector<carState> users;
    while(query.next()){
        carState car;
        car.ID = query.value("ID").toInt();
        car.State = query.value("State").toString();
        car.InTime = query.value("InTime").toString();
        car.OutTime = query.value("OutTime").toString();
        car.CarNumber = query.value("CarNumber").toString();
        car.StopTime = query.value("StopTime").toLongLong();
        users.push_back(car);
    }
    return users;
}

void sqlCarState::addCarState(const carState &car)
{
    auto query = QSqlQuery(sqldatabase);
    query.prepare("Insert into sqlCarState (ID, State, InTime, OutTime, CarNumber, StopTime) "
                  "values (:ID, :State, :InTime, :OutTime, :CarNumber, :StopTime)");
    query.bindValue(0,car.ID);
    query.bindValue(1,car.State);
    query.bindValue(2,car.InTime);
    query.bindValue(3,car.OutTime);
    query.bindValue(4,car.CarNumber);
    query.bindValue(5,car.StopTime);
    if(!query.exec()){
        loger::getInstance().write("[INSERT ERROR!]" + query.lastError().text());
    }
}

void sqlCarState::removeCarState(int userId)
{
    auto query = QSqlQuery(sqldatabase);
    query.prepare("delete from sqlCarState where ID = :ID");
    query.bindValue(0,userId);
    if(!query.exec()){
        loger::getInstance().write(QString("[REMOVE ID = %1 ERROR!]").arg(userId) + query.lastError().text());
    }
}


void sqlCarState::modifyCarState(const carState &car)
{
    auto query = QSqlQuery(sqldatabase);
    query.prepare("update sqlCarState set State = :State, InTime = :InTime, OutTime = :OutTime, "
                  "CarNumber = :CarNumber, StopTime = :StopTime where ID = :ID");
    query.bindValue(":ID",car.ID);
    query.bindValue(":Balanace",car.State);
    query.bindValue(":Name",car.InTime);
    query.bindValue(":IDCard",car.OutTime);
    query.bindValue(":CarNumber",car.CarNumber);
    query.bindValue(":PhoneNumber",car.StopTime);
    if(!query.exec()){
        loger::getInstance().write("[UPDATE ERROR!]" + query.lastError().text());
    }
}

int sqlCarState::getNewId()
{
    QString sqlStr = "Select IFNULL(MAX(ID), 0) as newId From sqlCarState";
    auto query = QSqlQuery(sqldatabase);
    query.exec(sqlStr);
    if(query.next()){
        return query.value(0).toInt() + 1;
    }
    else {
        return 1;
    }

}
