#ifndef ZDATA_H
#define ZDATA_H

#include <QWidget>
#include <QSqlQueryModel>
#include <QPixmap>
#include <QStandardItemModel>
#include <QTcpSocket>
#include <QDateTime>
#include <QMessageBox>
#include "register.h"
#include "database/sqlpark.h"
#include "database/sqlUser.h"
#include "common/caruser.h"
#include "common/tcpclient.h"
#include "zfindresult.h"
#include "zcarusermanage.h"

namespace Ui {
class zData;
}

class zData : public QWidget
{
    Q_OBJECT

public:
    explicit zData(QWidget *parent = 0);
    ~zData();
    QTcpSocket *tcpSocket;
    void loadDefaultData();
    void initPark();

signals:
    void tcpReadyRead(QTcpSocket *);
    void sigTcpStateUpdate(QAbstractSocket::SocketState);

    void sigParkNameTextChanged(const QString);
    void sigStallNumberValueChanged(const QString);
    void sigPriceValueChanged(const QString);

private slots:
    void on_pbtn_connect_clicked();
    void slotTcpStateDeal();
    void slotStateChanged(QAbstractSocket::SocketState state);

    void on_btnAddCarUser_clicked();//添加
    void on_btnDeleteCarUser_clicked();//删除
    void on_btnModifyCarUser_clicked();//修改
    void on_btnFindCarUser_clicked();//查找
    void on_comboBox_currentTextChanged(const QString &arg1);//修改查询的关键字
    void on_le_parkName_textChanged(const QString &arg1);//修改停车场的名字
    void on_spinBox_stallNumber_valueChanged(const QString &arg1);//修改车位数量
    void on_spinBox_price_valueChanged(const QString &arg1);//修改金额

private:
    Ui::zData *ui;
    void loadAllData(int selectRow);
    QStandardItemModel *model;
    bool isOpen;
};

#endif // ZDATA_H
