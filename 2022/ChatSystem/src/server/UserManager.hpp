

#pragma once
#include <pthread.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "DataBaseSvr.hpp"

enum UserStatus{
    OFFLINE, //0
    ONLINE //1
};

/*
 * 用户信息类
 * */
class UserInfo{
    public:
        //注册的时候，
        UserInfo(const std::string& nickname, const std::string& school, const std::string& telnum, const std::string& passwd, int userid){
            nickname_ = nickname;
            school_ = school;
            telnum_ = telnum;
            passwd_ = passwd;
            userid_ = userid;
            user_status_ = OFFLINE;
            tcp_socket_ = -1;
            friend_id_.clear();
        }

        UserInfo(){

        }

        ~UserInfo(){

        }

    public:
        std::string nickname_;
        std::string school_;
        std::string telnum_;
        std::string passwd_;

        int userid_;
        //用户状态 //OFFLINE ONLINE
        int user_status_;
        //登录的客户端对应的套接字描述符
        int tcp_socket_;

        std::vector<int> friend_id_;
};


class UserManager{
    public:
        UserManager(){
            user_map_.clear();
            pthread_mutex_init(&map_lock_, NULL);
            //如果一开始就从0进行分配， 一定是不对的
            //   因为用户管理类还会从数据库当中将已经存在的用户信息读回来
            prepare_id_ = -1;
            db_ = NULL;
        }

        ~UserManager(){
            pthread_mutex_destroy(&map_lock_);

            if(db_){
                delete db_;
                db_ = NULL;
            }
        }

        bool InitUserMana(){
            //1.连接数据库
            db_ = new DataBaseSvr();
            if(db_ == NULL){
                printf("create db case failed\n");
                return false;
            }

            if(db_->MysqlInit() == false){
                return false;
            }
            //2.查询所有用户信息， 维护起来
            Json::Value all_user;
            if(db_->GetAllUser(&all_user) == false){
                return false;
            }

            for(int i = 0; i < (int)all_user.size(); i++){
                //个人信息
                UserInfo ui;
                ui.nickname_ = all_user[i]["nickname"].asString();
                ui.school_ = all_user[i]["school"].asString();
                ui.telnum_ = all_user[i]["telnum"].asString();
                ui.passwd_ = all_user[i]["passwd"].asString();
                ui.userid_ = all_user[i]["userid"].asInt();
                ui.user_status_ = OFFLINE;

                //个人好友信息
                db_->GetFriend(ui.userid_, &ui.friend_id_);

                pthread_mutex_lock(&map_lock_);
                user_map_[ui.userid_] = ui;
                if(ui.userid_ > prepare_id_){
                    prepare_id_ = ui.userid_ + 1;
                }
                pthread_mutex_unlock(&map_lock_);
            }

            return true;
        }

        /*
         * 处理用户注册
         *     userid : 如果注册成功， 通过userid，告诉注册的客户端，他的id是什么
         * */
        int DealRegister(const std::string& nickname, const std::string& school, const std::string& tel, const std::string& passwd, int* userid){
            //1.判断注册信息是否为空
            if(nickname.size() == 0 || school.size() == 0 || tel.size() == 0 || passwd.size() == 0){
                *userid = -2;
                return -1;
            }
            //2.判断用户是否已经注册过了
            pthread_mutex_lock(&map_lock_);
            auto iter = user_map_.begin();
            while(iter != user_map_.end()){
                if(iter->second.telnum_ == tel){
                    *userid = -2;
                    pthread_mutex_unlock(&map_lock_);
                    return -1;
                }
                iter++;
            }
            //3.创建UserInfo， 分配userid, 保存用户信息
            UserInfo ui(nickname, school, tel, passwd, prepare_id_);
            *userid = prepare_id_;

            user_map_[prepare_id_] = ui;
            prepare_id_++;
            pthread_mutex_unlock(&map_lock_);
            //4.插入到数据库当中
            db_->InsertUser(ui.userid_, nickname, school, tel, passwd);
            return 0;
        }

        /*
         * 处理登录请求
         *    sockfd 是 服务端为登录客户端创建的新连接套接字
         * */
        int DealLogin(const std::string& tel, const std::string& passwd, int sockfd){
            //1.判断字段是否为空
            if(tel.size() == 0 || passwd.size() == 0){
                return -1;
            }
            //2.判断用户是否合法
            pthread_mutex_lock(&map_lock_);
            auto iter = user_map_.begin();
            while(iter != user_map_.end()){
                if(iter->second.telnum_ == tel){
                    break;
                }
                iter++;
            }
            if(iter == user_map_.end()){
                pthread_mutex_unlock(&map_lock_);
                return -1;
            }
            //3.校验密码是否正确
            if(iter->second.passwd_ != passwd){
                pthread_mutex_unlock(&map_lock_);
                return -1;
            }

            //4.更改用户的状态信息为ONLINE
            iter->second.user_status_ = ONLINE;
            int userid = iter->second.userid_;
            iter->second.tcp_socket_ = sockfd;
            pthread_mutex_unlock(&map_lock_);
            return userid;
        }

        int IsLogin(int userid){
            pthread_mutex_lock(&map_lock_);
            auto iter = user_map_.find(userid);
            if(iter == user_map_.end()){
                //这个用户都不存在
                pthread_mutex_unlock(&map_lock_);
                return -1;
            }

            if(iter->second.user_status_ == OFFLINE){
                pthread_mutex_unlock(&map_lock_);
                return OFFLINE;
            }
            pthread_mutex_unlock(&map_lock_);
            return ONLINE;
        }


        int IsLogin(const std::string& telnum, UserInfo* ui){
            pthread_mutex_lock(&map_lock_);
            auto iter = user_map_.begin();
            while(iter != user_map_.end()){
                if(iter->second.telnum_ == telnum){
                    break;
                }
                iter++;
            }

            if(iter == user_map_.end()){
                pthread_mutex_unlock(&map_lock_);
                return -1;
            }

            *ui = iter->second;
            if(iter->second.user_status_ == OFFLINE){
                pthread_mutex_unlock(&map_lock_);
                return OFFLINE;
            }
            pthread_mutex_unlock(&map_lock_);
            return ONLINE;
        }


        bool GetUserInfo(int userid, UserInfo* ui){
            pthread_mutex_lock(&map_lock_);
            auto iter = user_map_.find(userid);
            if(iter == user_map_.end()){
                //这个用户都不存在
                pthread_mutex_unlock(&map_lock_);
                return false;
            }

            *ui = iter->second;
            pthread_mutex_unlock(&map_lock_);
            return true;
        }

        bool GetFriends(int userid, std::vector<int>* fri){
            pthread_mutex_lock(&map_lock_);
            auto iter = user_map_.find(userid);
            if(iter == user_map_.end()){
                //这个用户都不存在
                pthread_mutex_unlock(&map_lock_);
                return false;
            }

            *fri = iter->second.friend_id_;
            pthread_mutex_unlock(&map_lock_);
            return true;

        }

        void SetFriend(int userid1, int userid2){
            //1.找userid1, 将userid2放到userid1的好友列表当中
            pthread_mutex_lock(&map_lock_);
            auto iter = user_map_.find(userid1);
            if(iter == user_map_.end()){
                //这个用户都不存在
                pthread_mutex_unlock(&map_lock_);
                return;
            }
            iter->second.friend_id_.push_back(userid2);
            
            //2.找userid2, 将userid1放到userid2的好友列表当中
            iter = user_map_.find(userid2);
            if(iter == user_map_.end()){
                //这个用户都不存在
                pthread_mutex_unlock(&map_lock_);
                return;
            }
            iter->second.friend_id_.push_back(userid1);
            pthread_mutex_unlock(&map_lock_);
            //3.插入到数据库当中
            db_->InsertFriend(userid1, userid2);
            db_->InsertFriend(userid2, userid1);
        }

        void SetUserOffLine(int sockfd){
            pthread_mutex_lock(&map_lock_);
            auto iter = user_map_.begin();
            while(iter != user_map_.end()){
                if(iter->second.tcp_socket_ == sockfd){
                    iter->second.user_status_ = OFFLINE;
                }
                iter++;
            }
            pthread_mutex_unlock(&map_lock_);
        }


    private:
        std::unordered_map<int, UserInfo> user_map_;
        pthread_mutex_t map_lock_;

        //针对注册用户分配的ID
        int prepare_id_;

        //数据库管理模块的实例化指针
        DataBaseSvr* db_;
};




























