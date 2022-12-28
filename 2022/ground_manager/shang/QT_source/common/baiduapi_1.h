#ifndef BAIDUAPI_H
#define BAIDUAPI_H

#include <QWidget>
#include <QImage>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtNetwork>
#include <QBuffer>
#include <QHttpMultiPart>
#include <QContextMenuEvent>
#include <QHttpPart>
#include "common/loger.h"

class BaiduAPI : public QWidget
{
    Q_OBJECT
public:
    explicit BaiduAPI(QWidget *parent = nullptr);

    void renewal_token();
    void networkPost();
    QString getCarNumber();

signals:

public slots:
    void replyFinished(QNetworkReply* reply);

    void replyToken(QNetworkReply* reply);

private:
    QString requestUrl;
    QString accessToken;
    QString AK;
    QString SK;
    QNetworkAccessManager *manager;
    QString carnumber;

};

#endif // BAIDUAPI_H
