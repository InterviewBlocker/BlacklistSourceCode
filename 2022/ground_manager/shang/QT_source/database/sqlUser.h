#ifndef DATASETS_H
#define DATASETS_H

#include <QString>
#include <vector>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "common/caruser.h"
#include "common/loger.h"

using namespace std;

class sqlUser
{
private:
    QSqlDatabase sqlDatabase;
    bool isExistTable(QString table);
    bool createTable(QString table);

public:
    sqlUser();
    bool initTable();
    bool isExist(QString field,QString value);
    carUser GetUserInfoByCarNum(QString carNumber);
    carUser GetUserInfoByPhoneNumber(QString PhoneNumber);
    vector<carUser> GetAllUsers();
    void addCarUser(const carUser &car);
    void removeCarUser(int userId);
    void modifyCarUser(const carUser &car);
    int getNewId();
};

#endif // DATASETS_H
