/*================================================================
*   Copyright (C) 2022 Sangfor Ltd. All rights reserved.
*   
*   文件名称：test_mysql.cpp
*   创 建 者：wudu
*   创建日期：2022年07月21日
*   描    述：
*
================================================================*/

#include <unistd.h>
#include <mysql/mysql.h>
#include <iostream>
using namespace std;

int main(){
    MYSQL* mysql_ = mysql_init(NULL);
    if(mysql_ == NULL){
        cout << "init mysql failed" << endl;
        return 0;
    }

    /*
        函数原型：
    *    MYSQL *
            mysql_real_connect(MYSQL *mysql, //mysql操作句柄
                   const char *host,  //服务端IP地址
                   const char *user,  //用户名
                   const char *passwd, //密码
                   const char *db,     //数据库
                   unsigned int port,  //端口
                   const char *unix_socket,  //是否使用本地域套接字
                   unsigned long client_flag  //数据库标志位， 通常为0， 采用默认属性
                   ) 
    */

    MYSQL* ret = mysql_real_connect(mysql_, "127.0.0.1", "root", "qwer@wu.888", "chatsystem_76", 3306, NULL, 0);
    if(ret == NULL){
        cout << "connect mysql failed" << endl;
        //需要释放msyql操作句柄
        mysql_close(mysql_);
        return 0;
    }

    mysql_set_character_set(mysql_, "utf8");
    cout << "connect success" << endl;


   // char buf[1024] = "insert into user(userid, nickname, school, telnum, passwd) values(1, 'wudu', 'bite', '123', '1')";
   // int func_ret = mysql_query(mysql_, buf);
   // if(func_ret != 0){
   //     cout << "exec sql failed: " <<buf << endl;
   // }
   // cout << "exec success" << endl;

    char buf[1024] = "select * from user;";
    int func_ret = mysql_query(mysql_, buf);
    if(func_ret != 0){
        cout << "exec sql failed: " <<buf << endl;
        mysql_close(mysql_);
        return 0;
    }
    cout << "exec success" << endl;

    MYSQL_RES* m_res = mysql_store_result(mysql_);
    if(m_res == NULL){
        cout << "get msyql res failed" << endl;
        mysql_close(mysql_);
        return 0;
    }

    int rows = mysql_num_rows(m_res);
    for(int i = 0; i < rows; i++){
        MYSQL_ROW row = mysql_fetch_row(m_res);
        cout << row[0] << ":" 
            << row[1] << ":"
            << row[2] << ":"
            << row[3] << ":"
            << row[4] << ":"
            << row[5] << endl;
    }

    mysql_free_result(m_res);


    mysql_close(mysql_);

    return 0;
}
