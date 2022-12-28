#ifndef ADMININFO_H
#define ADMININFO_H

#include <QMetaType>
#include <QString>

class AdminInfo
{
public:
    AdminInfo();

    int ID = -1;
    QString AdminName;
    QString RealName;
    QString PhoneNumber;
    int Age;
    QString Password;
};

#endif // ADMININFO_H
