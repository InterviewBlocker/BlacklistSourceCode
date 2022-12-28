#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>
#include <QTimer>
#include <QAtomicPointer>
#include "loger.h"

class tcpClient : public QObject
{
    Q_OBJECT
public:
    explicit tcpClient(QObject *parent = nullptr);

    static tcpClient& getInstance(void);
    tcpClient(const tcpClient &)=delete ;
    tcpClient & operator=(const tcpClient &)=delete;//禁止赋值拷贝函数。

private:
    QThread *mainThread=0;
    bool isConnected = false;
    QTimer *timer;
    QTimer errorTimer;
    static QAtomicPointer<tcpClient> instance;
    tcpClient();

private slots:
    void heartbeatReceive();
    void heartbeat();

signals:
    void connected();
    void disconnected();

public:
    ~tcpClient();
    bool connectTo(QTcpSocket *tcpSocket,QHostAddress ip,int port);
    bool connectStatus();

};

#endif // TCPCLIENT_H
