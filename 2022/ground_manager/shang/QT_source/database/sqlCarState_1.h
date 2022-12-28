#ifndef sqlCarState_H
#define sqlCarState_H

#include <QString>
#include <vector>
#include <QSqlDatabase>
#include "common/carstate.h"
#include "common/loger.h"

using namespace std;

class sqlCarState
{
private:
    QSqlDatabase sqldatabase;
    bool isExistTable(QString table);
    bool createTable(QString table);

public:
    sqlCarState();
    bool initTable();

    bool isExist(QString field,QString value);
    carState GetUserInfo(QString carNumber);
    vector<carState> GetUserInfoByState(QString carState);
    vector<carState> GetAllCarState();
    void addCarState(const carState &car);
    void removeCarState(int userId);
    void modifyCarState(const carState &car);
    int getNewId();
};

#endif // sqlCarState_H
