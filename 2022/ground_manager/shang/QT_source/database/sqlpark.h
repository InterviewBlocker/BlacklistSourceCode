#ifndef SQLPARK_H
#define SQLPARK_H
#include <QString>
#include <vector>
#include <QSqlDatabase>
#include "common/park.h"
#include "common/loger.h"
using namespace std;
class sqlPark
{
private:
    QSqlDatabase sqldatabase;
    bool isExistTable(QString table);
    bool createTable(QString table);

public:
    sqlPark();
    bool initTable();
    bool isExist(QString field,QString value);
    park GetParkInfo(int ID);
    vector<park> GetAllPark();
    void addPark(const park &p);
    void removePark(int parkId);
    void modifyPark(const park &p);
    int getNewId();
};

#endif // SQLPARK_H
