#ifndef LOGER_H
#define LOGER_H
#include <QAtomicPointer>
#include <QFile>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

class loger
{
public:

    static loger& getInstance(void);

    loger(const loger &)=delete ;
    loger & operator=(const loger &)=delete;//禁止赋值拷贝函数

    void write(QString opstring);
    QString logPath;

private:
    static QAtomicPointer<loger> instance;
    loger();
};

#endif // LOGER_H
