#include "sqlPark.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>

sqlPark::sqlPark()
{

}

bool sqlPark::initTable()
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

    if(!isExistTable("park")){
        createTable("park");
        sqlPark z_park;
        park p = z_park.GetParkInfo(1);
        if(p.ID != 1){
            p.ID = 1;
            p.charge = 0;
            p.parkName = "";
            p.stallNumber = 0;
            p.usedStallNumber = 0;
            z_park.addPark(p);
        }
    }
    return false;
}
bool sqlPark::isExistTable(QString table)
{
    bool bRet = false;
    if(!sqldatabase.open("RDIPCS.db","123")){
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

bool sqlPark::createTable(QString table)
{
    if(!sqldatabase.open("RDIPCS.db","123")){
        return false;
    }
    QSqlQuery query(sqldatabase);
    if(table == QString("park")){

        bool success = query.exec("CREATE TABLE IF NOT EXISTS park("
                                  "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                  "parkName           CHAR(40),"
                                  "charge              INTEGER,"
                                  "stallNumber        INTEGER,"
                                  "usedStallNumber    INTEGER);");
        if(success){
            loger::getInstance().write("park表创建成功");
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

bool sqlPark::isExist(QString field, QString value)
{
    QString sqlStr = "Select 1 from park where %1 = :%2";
    sqlStr = sqlStr.arg(field).arg(value);
    auto sqlquery = QSqlQuery(sqldatabase);
    sqlquery.prepare(sqlStr);
    sqlquery.bindValue(0,value);
    sqlquery.exec();
    return  sqlquery.next();
}

park sqlPark::GetParkInfo(int ID)
{
    QString sqlStr = "Select * From park where ID = :ID";
    auto query = QSqlQuery(sqldatabase);
    query.prepare(sqlStr);
    query.bindValue(":ID",ID);
    if(!query.exec()){
        loger::getInstance().write(QString("[FIND ID = %1 ERROR!]").arg(ID) + query.lastError().text());
    }
    park p;
    while(query.next()){
        p.ID = query.value("ID").toInt();
        p.charge = query.value("charge").toInt();
        p.parkName = query.value("parkName").toString();
        p.stallNumber = query.value("stallNumber").toInt();
        p.usedStallNumber = query.value("usedStallNumber").toInt();
    }
    return p;
}

vector<park> sqlPark::GetAllPark()
{
    auto query = QSqlQuery(sqldatabase);
    query.exec("Select * From park");
    vector<park> parks;
    while(query.next()){
        park p;
        p.ID = query.value("ID").toInt();
        p.charge = query.value("charge").toInt();
        p.parkName = query.value("parkName").toString();
        p.stallNumber = query.value("stallNumber").toInt();
        p.usedStallNumber = query.value("usedStallNumber").toInt();
        parks.push_back(p);
    }
    return parks;
}

void sqlPark::addPark(const park &p)
{
    auto query = QSqlQuery(sqldatabase);
    query.prepare("Insert into park (ID, charge, parkName, stallNumber, usedStallNumber) "
                  "values (:ID, :charge, :parkName, :stallNumber, :usedStallNumber)");
    query.bindValue(0,p.ID );
    query.bindValue(1,p.charge);
    query.bindValue(2,p.parkName);
    query.bindValue(3,p.stallNumber);
    query.bindValue(4,p.usedStallNumber);
    if(!query.exec()){
        loger::getInstance().write("[INSERT ERROR!]" + query.lastError().text());
    }
}

void sqlPark::removePark(int parkId)
{

    auto query = QSqlQuery(sqldatabase);
    query.prepare("delete from park where ID = :ID");
    query.bindValue(0,parkId);
    if(!query.exec()){
        loger::getInstance().write(QString("[REMOVE ID = %1 ERROR!]").arg(parkId) + query.lastError().text());
    }
}

void sqlPark::modifyPark(const park &p)
{

    auto query = QSqlQuery(sqldatabase);
    query.prepare("update park set ID = :ID, charge = :charge, parkName = :parkName, "
                  "stallNumber = :stallNumber, usedStallNumber = :usedStallNumber where ID = :ID");
    query.bindValue(":ID",p.ID );
    query.bindValue(":charge",p.charge);
    query.bindValue(":parkName",p.parkName);
    query.bindValue(":stallNumber",p.stallNumber);
    query.bindValue(":usedStallNumber",p.usedStallNumber);
    if(!query.exec()){
        loger::getInstance().write("[UPDATE ERROR!]" + query.lastError().text());
    }
}

int sqlPark::getNewId()
{

    QString sqlStr = "Select IFNULL(MAX(ID), 0) as newId From park";
    auto query = QSqlQuery(sqldatabase);
    query.exec(sqlStr);
    if(query.next()){
        return query.value(0).toInt() + 1;
    }
    else {
        return 1;
    }
}
