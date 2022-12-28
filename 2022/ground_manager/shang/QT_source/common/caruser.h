#ifndef CARUSER_H
#define CARUSER_H
#include <QMetaType>
#include <QString>

class carUser
{
public:
    carUser();
    int ID=-1;
    QString Name;
    QString CarNumber;
    QString PhoneNumber;
    QString Age;
    QString Sex;
    float Balance;

};
Q_DECLARE_METATYPE(carUser)   //宏是为了让QMetaType知道Type这个数据类型，并提供一个默认的拷贝构造函数和析构函数。
#endif // CARUSER_H
