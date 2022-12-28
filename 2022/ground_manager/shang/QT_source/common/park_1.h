#ifndef PARK_H
#define PARK_H
#include <QMetaType>
#include <QString>

class park
{
public:
    park();
    int ID=-1;
    QString parkName;
    int charge;
    int stallNumber;
    int usedStallNumber;
};
Q_DECLARE_METATYPE(park)
#endif // PARK_H
