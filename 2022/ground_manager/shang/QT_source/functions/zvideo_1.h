#ifndef ZVIDEO_H
#define ZVIDEO_H

#include <QWidget>
#include <QWidget>
#include <QDateTime>
#include <QMessageBox>

#include "common/baiduapi.h"
#include "common/tcpclient.h"
#include "database/sqlUser.h"
#include "database/sqlpark.h"
#include "database/sqlCarState.h"
#include "common/baiduapi.h"

namespace Ui {
class zVideo;
}

class zVideo : public QWidget
{
    Q_OBJECT

public:
    explicit zVideo(QWidget *parent = 0);
    ~zVideo();
    void getUsedStalNumber();

private slots:
    void sloShowReadPic(QTcpSocket *tcpSocket);
    void sloInNetworkPost();
    void sloOutNetworkPost();

    void sloChangeParkName(const QString parkName);
    void sloChangeStallNumberValue(const QString stallNumber);
    void sloChangePriceValue(const QString price);

    void on_pb_pass_clicked();

    void on_pb_nopass_clicked();

    void on_rbtn_on1_clicked();

    void on_rbtn_off1_clicked();

    void on_rbtn_on2_clicked();

    void on_rbtn_off2_clicked();

    void on_label_usedStallNumber_linkActivated(const QString &link);

signals:
    void sigVideoState(int videoNum,bool state);
    void sigPass();
    void sigNotPass();
    void updateTableView();
    void updateStallNumber(int usedStall,int residueStall);

private:
    Ui::zVideo *ui;

    BaiduAPI *api;
    int pic_len;
    int state = 1;
    char pic_buf[1024*1024];

};

#endif // ZVIDEO_H
