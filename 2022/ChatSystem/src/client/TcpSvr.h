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
    TcpSvr() ; //默认构造函数
    TcpSvr(const TcpSvr& p);//拷贝构造函数
public:
    static TcpSvr* getInstance();
    int Send(std::string& msg);
    int Recv(std::string* msg);
private:
    int ConnectToSvr();
    static TcpSvr* instance_;
    SOCKET sockfd_;
};