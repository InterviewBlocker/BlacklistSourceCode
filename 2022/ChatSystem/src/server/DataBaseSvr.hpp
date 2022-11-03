#pragma once
#include <mysql/mysql.h>
#include <vector>
#include <string>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <mutex>

#define HOST "127.0.0.1"
#define USER "root"
#define DB "chatsystem_76"
#define DBPORT 3306

class DataBaseSvr{
    public:
        DataBaseSvr(){
            mysql_ = NULL;
        }

        ~DataBaseSvr(){
            if(mysql_ != NULL){
                mysql_close(mysql_);
            }
        }

        /*
         * 初始化mysql操作句柄， 并且连接后台mysql服务端， 设置字符集
         * */
        bool MysqlInit(){
            mysql_ = mysql_init(NULL);
            if(mysql_ == NULL){
                std::cout << "mysql init failed" << std::endl;
                return false;
            }

            if(mysql_real_connect(mysql_, HOST, USER, PASSWD, DB, DBPORT,NULL, 0) == NULL){
                std::cout << "msyql connect failed" << std::endl;
                mysql_close(mysql_);
                return false;
            }

            mysql_set_character_set(mysql_, "utf8");
            return true;
        }

        /*
         * 获取 all user info, to usermanager model
         *    参数为Json对象， 是一个出参
         * */
        bool GetAllUser(Json::Value* all_user){
#define GETALLUSER "select * from user;"
            lock_.lock();
            //1.数据库查询
            if(MysqlQuery(GETALLUSER) == false){
                lock_.unlock();
                return false;
            }

            //2.获取结果集
            MYSQL_RES* res = mysql_store_result(mysql_);
            if(res == NULL){
                lock_.unlock();
                return false;
            }
            lock_.unlock();
            //3.获取单行数据
            int row_nums = mysql_num_rows(res);
            for(int i = 0; i < row_nums; i++){
                MYSQL_ROW row = mysql_fetch_row(res);
                //4.将单行数据按照格式， 组织起来。 传递给调用者
                Json::Value tmp;
                tmp["userid"] = atoi(row[0]);
                tmp["nickname"] = row[1];
                tmp["school"] = row[2];
                tmp["telnum"] = row[3];
                tmp["passwd"] = row[4];
                all_user->append(tmp);
            }

            mysql_free_result(res);
            return true;
        }

        /*
         * 获取单个用户的好友信息， 在程序初始化阶段， 让用户管理模块维护起来
         *    userid : 用户的id
         *    f_id : 该用户的所有好友id
         * */
        bool GetFriend(int userid, std::vector<int>* f_id){
#define GETFRIEND "select friend from friendinfo where userid='%d';"

            //1.格式化sql语句
            char sql[1204] = {0};
            sprintf(sql, GETFRIEND, userid);

            lock_.lock();
            //2.查询
            if(MysqlQuery(sql) == false){
                lock_.unlock();
                return false;
            }

            //3.获取结果集
            MYSQL_RES* res = mysql_store_result(mysql_);
            if(res == NULL){
                lock_.unlock();
                return false;
            }
            lock_.unlock();
            //4.获取单行数据
            int row_nums = mysql_num_rows(res);
            for(int i = 0; i < row_nums; i++){
                MYSQL_ROW row = mysql_fetch_row(res);
                f_id->push_back(atoi(row[0]));
            }

            mysql_free_result(res);
            return true;
        }

        /*
         *  当用户注册的时候， 进行插入使用的函数
         * */
        bool InsertUser(int userid, const std::string& nickname
                , const std::string& school, const std::string& telnum
                , const std::string& passwd){
#define INSERTUSER "insert into user(userid, nickname, school, telnum, passwd) values('%d', '%s', '%s', '%s', '%s');"

            char sql[1024] = {0};
            sprintf(sql, INSERTUSER, userid, nickname.c_str(), school.c_str(), telnum.c_str(), passwd.c_str());
            std::cout << "Insert User: " << sql << std::endl;

            //2.查询
            if(MysqlQuery(sql) == false){
                return false;
            }
            return true;
        }

        /*
         * 添加好友
         * */
        bool InsertFriend(int userid1, int userid2){
#define INSERTFRIEND "insert into friendinfo values('%d', '%d');"
            char sql[1024] = {0};
            sprintf(sql, INSERTFRIEND, userid1, userid2);

            //2.查询
            if(MysqlQuery(sql) == false){
                return false;
            }
            return true;
        }

    private:
        bool MysqlQuery(const std::string& sql){
            if(mysql_query(mysql_, sql.c_str()) != 0){
                std::cout << "exec failed sql: " << sql << std::endl;
                return false;
            }
            return true;
        }


    private:
        MYSQL* mysql_;
        std::mutex lock_;
};
