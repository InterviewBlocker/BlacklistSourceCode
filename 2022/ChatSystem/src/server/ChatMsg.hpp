

#pragma once
#include <string>
#include <memory>
#include <sstream>
#include <jsoncpp/json/json.h>

#define TCP_DATA_MAX_LEN 1024
#define TCP_PORT 38989

enum chat_msg_type{
    Register = 0, //0， 注册请求
    Register_Resp, //1， 注册应答
    Login,   //2. 登录请求
    Login_Resp, //3， 登录应答
    AddFriend, //4， 添加好友请求
    AddFriend_Resp, //5， 添加好友请求应答
    SendMsg, //6， 发送消息
    SendMsg_Resp, //7， 发送消息应答
    PushMsg, //8， 推送消息
    PushMsg_Resp, //9， 推送消息应答
    PushAddFriendMsg, //10， 推送添加好友请求
    PushAddFriendMsg_Resp, //11， 推送添加好友请求的应答 
    GetFriendMsg, //12， 获取全部好友信息
    GetFriendMsg_Resp, //13， 获取全部好友信息应答
    SetUserOffLine //14
    //后续如果要增加业务， 可以在后面增加其他的消息类型
};

enum reply_status{
    REGISTER_SUCCESS = 0, //0, 注册成功
    REGISTER_FAILED, //1，注册失败
    LOGIN_SUCESSS, //2， 登录成功
    LOGIN_FAILED,  //3， 登陆失败
    ADDFRIEND_SUCCESS, //4， 添加好友成功
    ADDFRIEND_FAILED, //5， 添加好友失败
    SENDMSG_SUCCESS, //6， 发送消息成功
    SENDMSG_FAILED, //7， 发送给消息失败
    GETFRIEND_SUCCESS, //8，获取好友列表成功
    GETFRIEND_FAILED  //9， 获取好友列表失败
};

/*
 * 注册请求的消息格式
 *   sockfd_ (消息达到服务端之后， 由服务端接收之后， 打上sockfd_)
 *   msg_type_ : Register
 *   json_msg: {
 *      nickname : 'xxx'
 *      school : "xxx"
 *      telnum : "xxxx"
 *      passwd : "xxxx"
 *   }
 *
 * 注册的应答：
 *   msg_type_ : Register_Resp
 *   reply_status_ = REGISTER_SUCCESS / REGISTER_FAILED
 *      如果是REGISTER_SUCCESS : [user_id_]
 *
 *
 *
 * 登录的请求消息格式
 *   sockfd_ (消息达到服务端之后， 由服务端接收之后， 打上sockfd_)
 *   msg_type_ : Login
 *   json_msg_ : {
 *      telnum : xxx
 *      passwd : xxx
 *   }
 *
 *   登录的应答：
 *   msg_type : Login_Resp;
 *   reply_status_ : LOGIN_SUCCESS/LOGIN_FAILED
 *       如果是LOGIN_SUCCESS : [user_id_]
 *
 *
 *
 * 添加好友请求：
 *    msg_type_ : AddFriend
 *    json_msg_ :{
 *      fri_tel_num : xxxx
 *    }
 *
 *
 *  推送添加好友的请求
 *      msg_type : PushAddFriendMsg
 *      sockfd_ : 被添加方的套接字描述符
 *      json_msg_: {
 *          adder_nickname : xxx
 *          adder_school : xxx
 *          adder_userid : xxx
 *      }
 *
 * 推送添加好友的应答（被添加方发送给服务端的）
 *     msg_type : PushAddFriendMsg_Resp
 *     user_id : 被添加方的id
 *     reply_status : ADDFRIEND_SUCCESS / ADDFRIEND_FAILED
 *         如果说是ADDFRIEND_SUCCESS
 *             json_msg_ : 添加方的id
 *
 *  添加好友的应答：
 *      msg_type: AddFriend_Resp
 *      reply_status : ADDFRIEND_FAILED / ADDFRIEND_SUCCESS
 *          如果是成功：ADDFRIEND_SUCCESS
 *             json_msg_ : 
 *                 BeAdd_nickname : 被添加方的名字
 *                 BeAdd_school ： 被添加方的学校
 *                 BeAdd_userid : 被添加方的id
 * */
class JsonUtil{
    public:
        static bool Serialize(const Json::Value& value, std::string* body) {
            Json::StreamWriterBuilder swb;
            std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());
        
            std::stringstream ss;
            int ret = sw->write(value, &ss);
            if (ret != 0) {
                return false;
            }
            *body = ss.str();
            return true;
        }
        
        static bool UnSerialize(const std::string& body, Json::Value* value) {
            Json::CharReaderBuilder crb;
            std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
        
            std::string err;
            bool ret = cr->parse(body.c_str(), body.c_str() + body.size(), value, &err);
            if (ret == false) {                                                                                                                                                                  
                return false;
            }
            return true;
        }
};



class ChatMsg{
    public:
        ChatMsg(){
            sockfd_ = -1;
            msg_type_ = -1;
            user_id_ = -1;
            reply_status_ = -1;
            json_msg_.clear();
        }

        ~ChatMsg(){
            
        }

        /*
         * 提供反序列化的接口, 接收完毕请求之后进行反序列化 
         * */
        int PraseChatMsg(int sockfd, const std::string& msg){
            //1.调用jsoncpp的反序列化接口
            Json::Value tmp;
            bool ret = JsonUtil::UnSerialize(msg, &tmp);
            if(ret == false){
                return -1;
            }
            //2.赋值给成员变量
            sockfd_ = sockfd;
            msg_type_ = tmp["msg_type"].asInt();
            user_id_ = tmp["user_id"].asInt();
            reply_status_ = tmp["reply_status"].asInt();
            json_msg_ = tmp["json_msg"];
            return 0;
        }


        /*
         * 提供序列化的接口 - 回复应答的时候使用
         *     msg ： 出参， 用于获取序列化完毕的字符串
         * */
        bool GetMsg(std::string* msg){
            Json::Value tmp;
            tmp["msg_type"] = msg_type_;
            tmp["user_id"] = user_id_;
            tmp["reply_status"] = reply_status_;
            tmp["json_msg"] = json_msg_;

            return JsonUtil::Serialize(tmp, msg);
        }

        /*
         * 获取json_msg_当中的value值
         * */
        std::string GetValue(const std::string& key){
            if(!json_msg_.isMember(key)){
                return "";
            }
            return json_msg_[key].asString();
        }

        /*
         * 设置json_msg_当中的kv键值对
         * */

        void SetValue(const std::string& key, const std::string& value){
            json_msg_[key] = value;
        }

        void SetValue(const std::string& key, int value){
            json_msg_[key] = value;
        }
        void Clear(){
            msg_type_ = -1;
            user_id_ = -1;
            reply_status_ = -1;
            json_msg_.clear();
        }
    public:
        //存放的客户端文件名描述符， 方便发送线程， 通过该字段将数据发送给对应的客户端
        int sockfd_;

        //消息类型
        #if 0
        enum chat_msg_type{
            Register = 0, //0， 注册请求
            Register_Resp, //1， 注册应答
            Login,   //2. 登录请求
            Login_Resp, //3， 登录应答
            AddFriend, //4， 添加好友请求
            AddFriend_Resp, //5， 添加好友请求应答
            SendMsg, //6， 发送消息
            SendMsg_Resp, //7， 发送消息应答
            PushMsg, //8， 推送消息
            PushMsg_Resp, //9， 推送消息应答
            PushAddFriendMsg, //10， 推送添加好友请求
            PushAddFriendMsg_Resp, //11， 推送添加好友请求的应答 
            GetFriendMsg, //12， 获取全部好友信息
            GetFriendMsg_Resp //13， 获取全部好友信息应答
        };
        #endif
        int msg_type_;

        //用户id
        int user_id_;

        //应答的状态
        #if 0
        enum reply_status{
            REGISTER_SUCCESS = 0, //0, 注册成功
            REGISTER_FAILED, //1，注册失败
            LOGIN_SUCESSS, //2， 登录成功
            LOGIN_FAILED,  //3， 登陆失败
            ADDFRIEND_SUCCESS, //4， 添加好友成功
            ADDFRIEND_FAILED, //5， 添加好友失败
            SENDMSG_SUCCESS, //6， 发送消息成功
            SENDMSG_FAILED, //7， 发送给消息失败
            GETFRIEND_SUCCESS, //8，获取好友列表成功
            GETFRIEND_FAILED  //9， 获取好友列表失败
        };
        #endif
        int reply_status_;

        /*
         * Json消息
         *   json消息的内容会随着消息类型的不同， 字段不一样
         * */
        Json::Value json_msg_;
};
