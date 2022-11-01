#pragma once
#include <string>
#include <memory>
#include <sstream>
#if _WIN32
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif


#define TCP_DATA_MAX_LEN 1024
#define TCP_PORT 38989


enum chat_msg_type {
    Register = 0, //0�� ע������
    Register_Resp, //1�� ע��Ӧ��
    Login,   //2. ��¼����
    Login_Resp, //3�� ��¼Ӧ��
    AddFriend, //4�� ��Ӻ�������
    AddFriend_Resp, //5�� ��Ӻ�������Ӧ��
    SendMsg, //6�� ������Ϣ
    SendMsg_Resp, //7�� ������ϢӦ��
    PushMsg, //8�� ������Ϣ
    PushMsg_Resp, //9�� ������ϢӦ��
    PushAddFriendMsg, //10�� ������Ӻ�������
    PushAddFriendMsg_Resp, //11�� ������Ӻ��������Ӧ�� 
    GetFriendMsg, //12�� ��ȡȫ��������Ϣ
    GetFriendMsg_Resp //13�� ��ȡȫ��������ϢӦ��
    //�������Ҫ����ҵ�� �����ں���������������Ϣ����
};

enum reply_status {
    REGISTER_SUCCESS = 0, //0, ע��ɹ�
    REGISTER_FAILED, //1��ע��ʧ��
    LOGIN_SUCESSS, //2�� ��¼�ɹ�
    LOGIN_FAILED,  //3�� ��½ʧ��
    ADDFRIEND_SUCCESS, //4�� ��Ӻ��ѳɹ�
    ADDFRIEND_FAILED, //5�� ��Ӻ���ʧ��
    SENDMSG_SUCCESS, //6�� ������Ϣ�ɹ�
    SENDMSG_FAILED, //7�� ���͸���Ϣʧ��
    GETFRIEND_SUCCESS, //8����ȡ�����б�ɹ�
    GETFRIEND_FAILED  //9�� ��ȡ�����б�ʧ��
};

/*
 * ע���������Ϣ��ʽ
 *   sockfd_ (��Ϣ�ﵽ�����֮�� �ɷ���˽���֮�� ����sockfd_)
 *   msg_type_ : Register
 *   json_msg: {
 *      nickname : 'xxx'
 *      school : "xxx"
 *      telnum : "xxxx"
 *      passwd : "xxxx"
 *   }
 *
 * ע���Ӧ��
 *   msg_type_ : Register_Resp
 *   reply_status_ = REGISTER_SUCCESS / REGISTER_FAILED
 *      �����REGISTER_SUCCESS : [user_id_]
 *
 *
 *
 * ��¼��������Ϣ��ʽ
 *   sockfd_ (��Ϣ�ﵽ�����֮�� �ɷ���˽���֮�� ����sockfd_)
 *   msg_type_ : Login
 *   json_msg_ : {
 *      telnum : xxx
 *      passwd : xxx
 *   }
 *
 *   ��¼��Ӧ��
 *   msg_type : Login_Resp;
 *   reply_status_ : LOGIN_SUCCESS/LOGIN_FAILED
 *       �����LOGIN_SUCCESS : [user_id_]
 *
 *
 *
 * ��Ӻ�������
 *    msg_type_ : AddFriend
 *    json_msg_ :{
 *      fri_tel_num : xxxx
 *    }
 *
 *
 *  ������Ӻ��ѵ�����
 *      msg_type : PushAddFriendMsg
 *      sockfd_ : ����ӷ����׽���������
 *      json_msg_: {
 *          adder_nickname : xxx
 *          adder_school : xxx
 *          adder_userid : xxx
 *      }
 *
 * ������Ӻ��ѵ�Ӧ�𣨱���ӷ����͸�����˵ģ�
 *     msg_type : PushAddFriendMsg_Resp
 *     user_id : ����ӷ���id
 *     reply_status : ADDFRIEND_SUCCESS / ADDFRIEND_FAILED
 *         ���˵��ADDFRIEND_SUCCESS
 *             json_msg_ : ��ӷ���id
 *
 *  ��Ӻ��ѵ�Ӧ��
 *      msg_type: AddFriend_Resp
 *      reply_status : ADDFRIEND_FAILED / ADDFRIEND_SUCCESS
 *          ����ǳɹ���ADDFRIEND_SUCCESS
 *             json_msg_ :
 *                 BeAdd_nickname : ����ӷ�������
 *                 BeAdd_school �� ����ӷ���ѧУ
 *                 BeAdd_userid : ����ӷ���id
 * */




class ChatMsg {
public:
    ChatMsg();

    ~ChatMsg();

    /*
     * �ṩ�����л��Ľӿ�, �����������֮����з����л�
     * */
    int PraseChatMsg(int sockfd, const std::string& msg);


    /*
     * �ṩ���л��Ľӿ� - �ظ�Ӧ���ʱ��ʹ��
     *     msg �� ���Σ� ���ڻ�ȡ���л���ϵ��ַ���
     * */
    bool GetMsg(std::string* msg);

    /*
     * ��ȡjson_msg_���е�valueֵ
     * */
    std::string GetValue(const std::string& key);

    /*
     * ����json_msg_���е�kv��ֵ��
     * */

    void SetValue(const std::string& key, const std::string& value);
    void SetValue(const std::string& key, int value);
    void Clear() ;
public:
    //��ŵĿͻ����ļ����������� ���㷢���̣߳� ͨ�����ֶν����ݷ��͸���Ӧ�Ŀͻ���
    int sockfd_;

    //��Ϣ����
#if 0
    enum chat_msg_type {
        Register = 0, //0�� ע������
        Register_Resp, //1�� ע��Ӧ��
        Login,   //2. ��¼����
        Login_Resp, //3�� ��¼Ӧ��
        AddFriend, //4�� ��Ӻ�������
        AddFriend_Resp, //5�� ��Ӻ�������Ӧ��
        SendMsg, //6�� ������Ϣ
        SendMsg_Resp, //7�� ������ϢӦ��
        PushMsg, //8�� ������Ϣ
        PushMsg_Resp, //9�� ������ϢӦ��
        PushAddFriendMsg, //10�� ������Ӻ�������
        PushAddFriendMsg_Resp, //11�� ������Ӻ��������Ӧ�� 
        GetFriendMsg, //12�� ��ȡȫ��������Ϣ
        GetFriendMsg_Resp //13�� ��ȡȫ��������ϢӦ��
    };
#endif
    int msg_type_;

    //�û�id
    int user_id_;

    //Ӧ���״̬
#if 0
    enum reply_status {
        REGISTER_SUCCESS = 0, //0, ע��ɹ�
        REGISTER_FAILED, //1��ע��ʧ��
        LOGIN_SUCESSS, //2�� ��¼�ɹ�
        LOGIN_FAILED,  //3�� ��½ʧ��
        ADDFRIEND_SUCCESS, //4�� ��Ӻ��ѳɹ�
        ADDFRIEND_FAILED, //5�� ��Ӻ���ʧ��
        SENDMSG_SUCCESS, //6�� ������Ϣ�ɹ�
        SENDMSG_FAILED, //7�� ���͸���Ϣʧ��
        GETFRIEND_SUCCESS, //8����ȡ�����б�ɹ�
        GETFRIEND_FAILED  //9�� ��ȡ�����б�ʧ��
    };
#endif
    int reply_status_;

    /*
     * Json��Ϣ
     *   json��Ϣ�����ݻ�������Ϣ���͵Ĳ�ͬ�� �ֶβ�һ��
     * */
    Json::Value json_msg_;
};
