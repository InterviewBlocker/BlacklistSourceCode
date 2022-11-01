#pragma once
#pragma warning(disable:4996)
#include <Winsock2.h>
#include <string>
#include <mutex>
#include <iostream>
#include "ChatMsg.h"

#pragma comment(lib, "ws2_32.lib")

class TcpSvr{
private:
    TcpSvr() ; //Ĭ�Ϲ��캯��
    TcpSvr(const TcpSvr& p);//�������캯��
public:
    static TcpSvr* getInstance();
    int Send(std::string& msg);
    int Recv(std::string* msg);
private:
    int ConnectToSvr();
    static TcpSvr* instance_;
    SOCKET sockfd_;
};