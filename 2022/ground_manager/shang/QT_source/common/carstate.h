#ifndef CARSTATE_H
#define CARSTATE_H
#include <QMetaType>
#include <QString>

class carState
{
public:
    carState();

    int ID=-1;
    QString CarNumber;
    QString State;
    QString InTime;
    QString OutTime;
    long long StopTime;
};
Q_DECLARE_METATYPE(carState)

#endif // CARSTATE_H

