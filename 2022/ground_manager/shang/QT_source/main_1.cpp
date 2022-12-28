#include "login.h"
#include "mainframe.h"
#include <QApplication>
#include <QSplashScreen>
#include <QSqlDatabase>
#include <QMovie>
#include <QLabel>
#include <QPainter>
#include <synchapi.h>
#include "database/sqlUser.h"
#include "database/sqlCarState.h"
#include "database/sqlpark.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile file("./logs/log.log");
    file.remove();
    sqlUser *dbUser = new sqlUser();
    sqlCarState *dbState = new sqlCarState();
    sqlPark *dbPark = new sqlPark();
    dbUser->initTable();
    dbState->initTable();
    dbPark->initTable();

    loger::getInstance().write("----------------------启动程序----------------------");
    loger::getInstance().write("软件版本:V2.1.2.0");

    Login w;
    w.show();

    return a.exec();
}

