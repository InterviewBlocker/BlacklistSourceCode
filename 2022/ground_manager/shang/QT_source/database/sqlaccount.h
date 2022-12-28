#ifndef SQLACCOUNT_H
#define SQLACCOUNT_H

#include <QString>
#include <vector>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "common/adminInfo.h"
#include "common/loger.h"

class sqlAccount
{

private:
    QSqlDatabase sqlDatabase;
    bool isExistTable(QString table);
    bool createTable(QString table);

public:
    sqlAccount();

    bool initTable(); 
    bool isExist(QString field, QString value);
    void addAdmin(const AdminInfo& admin);
    int getNewId();
};

#endif // SQLACCOUNT_H
