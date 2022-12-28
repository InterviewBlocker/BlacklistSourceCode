#include "tcpclient.h"

QAtomicPointer<tcpClient> tcpClient::instance = nullptr;

tcpClient& tcpClient::getInstance()
{
    return *tcpClient::instance;
}

tcpClient::tcpClient()
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &tcpClient::heartbeat);
    connect(&errorTimer, &QTimer::timeout, this, &tcpClient::heartbeatReceive);
}

void tcpClient::heartbeatReceive()
{
    loger::getInstance().write("长时间未收到响应，关闭TCP连接");
    emit disconnected();
}

void tcpClient::heartbeat()
{
    timer->stop();
    errorTimer.start(5000);
}
tcpClient::~tcpClient()
{

}
bool tcpClient::connectTo(QTcpSocket *tcpSocket, QHostAddress ip, int port)
{
//    qDebug()<<"ip:"<<ip;
//    qDebug()<<"port:"<<port;

    try{
        tcpSocket->connectToHost(ip, port);
//        if(tcpSocket->waitForConnected(1000))
//        {
//            qDebug()<<"连接成功进入if";
//        }
    }
    catch(...)
    {
        return false;
    }
//    qDebug()<<"连接中....";
    return true;
}
bool tcpClient::connectStatus()
{
    return isConnected;
}
