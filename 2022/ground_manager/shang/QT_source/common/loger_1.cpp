#include "loger.h"

QAtomicPointer<loger> loger::instance=nullptr;

loger &loger::getInstance()
{
    if(loger::instance.testAndSetOrdered(0, 0))//第一次检测
    {

        loger::instance.testAndSetOrdered(0, new loger);//第二次检测。
    }

    return *loger::instance;
}

void loger::write(QString opstring)
{
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ");
    QDir dir;
    if(!dir.exists("./logs/"))
    {
        dir.mkpath("./logs/");
    }
    QFile file("./logs/log.log");
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << currentDateTime << opstring << "\r\n";
    file.flush();
    file.close();

}

loger::loger()
{

}


