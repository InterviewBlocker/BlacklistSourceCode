#include "baiduapi.h"

BaiduAPI::BaiduAPI(QWidget *parent) : QWidget(parent)
{
    AK="8IK1ddUmByMwQf2g1MLLH0s2";
    SK="ubEfVro4XXwuygreddBhC2Qz7WT6webh";
    requestUrl="https://aip.baidubce.com/rest/2.0/ocr/v1/license_plate";
    renewal_token(); //更新token值
    manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void BaiduAPI::renewal_token()
{
    //更新token值
    QNetworkAccessManager  *manager_token = new QNetworkAccessManager();
    connect(manager_token, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyToken(QNetworkReply*)));
    QString url_token="https://aip.baidubce.com/oauth/2.0/token";
    QString req_url = url_token+"?grant_type=client_credentials&client_id="+AK+"&client_secret="+SK;
    loger::getInstance().write("url_token:" + req_url);
    QUrl url(req_url);
    QNetworkRequest request(url);
    QByteArray data="";
    manager_token->post(request,data);
}

void BaiduAPI::networkPost()
{
    //设置url
    QUrl url(requestUrl+"?access_token="+accessToken);
    QNetworkRequest request(url);
    //设置http header
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
    //设置http body
    QString name = "./car1.jpg";
    QImage image(name);
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "jpg");
    QByteArray header = "image=";
    QByteArray postData = header+ba.toBase64().toPercentEncoding();
    //post方式发送
    manager->post(request,postData);
}

QString BaiduAPI::getCarNumber()
{
    return carnumber;
}

void BaiduAPI::replyFinished(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();

    QJsonDocument document=QJsonDocument::fromJson(data);
    QJsonObject result = document.object();

    if(result.contains("error_msg")){
        loger::getInstance().write("failed:" + result.value("error_msg").toString());
    }
    else{
        QJsonObject num= result.value("words_result").toObject();
        carnumber = num.value("number").toString();
    }
}

void BaiduAPI::replyToken(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
    QJsonDocument document=QJsonDocument::fromJson(data);
    QJsonObject result = document.object();

    if( result.contains("error")){
        qDebug()<<"refresh_token failed:"<<result.value("error_description").toString()<<endl;
        loger::getInstance().write("refresh_token failed:" + result.value("error_description").toString());
    }
    else{
        accessToken = result.value("access_token").toString();
        qDebug()<<"accessToken:"<<accessToken<<endl;
        loger::getInstance().write("accessToken:"+accessToken);
    }
}
