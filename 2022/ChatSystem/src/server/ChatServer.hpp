
#pragma once
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "UserManager.hpp"
#include "MsgQueue.hpp"
#include "ChatMsg.hpp"

#define THREAD_COUNT 4

struct Msg{
    Msg(){
        sockfd_ = -1;
        memset(buf, '\0', 1024);
    }
    int sockfd_;
    char buf[1024];
};

class ChatServer{
    public:
        ChatServer(){
            tcp_sock_ = -1;
            tcp_port_ = TCP_PORT;
            user_mana_ = NULL;
            epoll_fd_ = -1;
            thread_count_ = THREAD_COUNT;

            send_que_ = NULL;
            ready_sockfd_que_ = NULL;
            recv_que_ = NULL;
        }

        ~ChatServer(){

        }

        //初始化资源的函数
        int InitChatServer(uint16_t tcp_port = TCP_PORT, int thread_count=THREAD_COUNT){
            tcp_port_ = tcp_port;
            thread_count_ = thread_count;

            //tcp初始化
            tcp_sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if(tcp_sock_ < 0){
                perror("socket");
                return -1;
            }

            //端口重用
            int opt = 1;
            setsockopt(tcp_sock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(tcp_port_);
            addr.sin_addr.s_addr = inet_addr("0.0.0.0");
            int ret = bind(tcp_sock_, (struct sockaddr*)&addr, sizeof(addr));
            if(ret < 0){
                perror("bind");
                return -1;
            }

            ret = listen(tcp_sock_, 1024);
            if(ret < 0){
                perror("listen");
                return -1;
            }

            //epoll 初始化
            epoll_fd_ = epoll_create(5);
            if(epoll_fd_ < 0){
                return -1;
            }

            //用户管理模块
            user_mana_ = new UserManager();
            if(user_mana_ == NULL){
                return -1;
            }

            if(user_mana_->InitUserMana() == false){
                return -1;
            }


            recv_que_ = new MsgQueue<ChatMsg>();
            if(recv_que_ == NULL){
                return -1;
            }

            send_que_ = new MsgQueue<ChatMsg>();
            if(send_que_ == NULL){
                return -1;
            }

            ready_sockfd_que_ = new MsgQueue<int>();
            if(ready_sockfd_que_ == NULL){
                return -1;
            }

            return 0;
        }

        //启动各类线程的函数 - 主线程调用的
        int StartChatServer(){
            //1.创建epoll等待线程
            pthread_t tid;
            int ret  = pthread_create(&tid, NULL, epoll_wait_start, (void*)this);
            if(ret < 0){
                perror("pthread_create");
                return -1;
            }
            //2.创建接收线程
            ret  = pthread_create(&tid, NULL, recv_msg_start, (void*)this);
            if(ret < 0){
                perror("pthread_create");
                return -1;
            }
            //3.创建发送线程
            ret  = pthread_create(&tid, NULL, send_msg_start, (void*)this);
            if(ret < 0){
                perror("pthread_create");
                return -1;
            }
            //4.创建工作线程
            for(int i = 0; i < thread_count_; i++){
                ret = pthread_create(&tid, NULL, deal_start, (void*)this);
                if(ret < 0){
                    thread_count_--;
                }
            }

            if(thread_count_ <= 0){
                return -1;
            }
            
            //5.主线程循环接收新连接 & 将新连接的套接字放到epoll当中
            struct sockaddr_in cli_addr;
            socklen_t cli_addr_len = sizeof(cli_addr);
            while(1){
                int newsockfd = accept(tcp_sock_,(struct sockaddr*)&cli_addr, &cli_addr_len);
                if(newsockfd < 0){
                    continue;
                }

                //接收上了， 添加到epoll当中进行监控
                struct epoll_event ee;
                ee.events = EPOLLIN;
                ee.data.fd = newsockfd;
                epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, newsockfd, &ee);
            }

            return 0;
        }

        static void* epoll_wait_start(void* arg){
            pthread_detach(pthread_self());
            ChatServer* cs = (ChatServer*)arg;
            while(1){
                struct epoll_event arr[10];
                int ret = epoll_wait(cs->epoll_fd_, arr, sizeof(arr)/sizeof(arr[0]), -1);
                if(ret < 0){
                    continue;
                }

                //正常获取了就绪的事件结构, 一定全部都是新连接套接字
                for(int i = 0; i < ret; i++){
                    char buf[TCP_DATA_MAX_LEN] = {0};
                    //隐藏的问题： TCP粘包
                    ssize_t recv_size = recv(arr[i].data.fd, buf, sizeof(buf) - 1, 0);
                    if(recv_size < 0){
                        //接收失败了
                        std::cout << "recv failed : sockfd is " << arr[i].data.fd << std::endl;
                        continue;
                    }else if(recv_size == 0){
                        //对端关闭连接了
                        epoll_ctl(cs->epoll_fd_, EPOLL_CTL_DEL,arr[i].data.fd, NULL);
                        close(arr[i].data.fd);
                        //组织一个更改用户状态的消息 , 鸡贼做法（客户端退出的时候， 发送下线通知）
                        cs->user_mana_->SetUserOffLine(arr[i].data.fd);
                        continue;
                    }

                    printf("epoll_wait_start recv msg : %s  from sockfd is %d\n", buf, arr[i].data.fd);
                    //正常接收回来了, 将接收回来的数据放到接收线程的队列当中， 等到工作线程从队列当中获取消息， 进而进行处理
                    //3.将接收到的数据放到接收队列当当中
                    std::string msg;
                    msg.assign(buf, strlen(buf));

                    ChatMsg cm;
                    cm.PraseChatMsg(arr[i].data.fd, msg);

                    cs->recv_que_->Push(cm);
                }
            }
            return NULL;
        }

        static void* recv_msg_start(void* arg){
            pthread_detach(pthread_self());
            ChatServer* cs = (ChatServer*)arg;

            while(1){
            }


            return NULL;
        }

        static void* send_msg_start(void* arg){
            pthread_detach(pthread_self());
            ChatServer* cs = (ChatServer*)arg;

            while(1){
                //1.从队列拿出数据
                ChatMsg cm;
                cs->send_que_->Pop(&cm);

                std::string msg;
                cm.GetMsg(&msg);
                std::cout << "send thread: " << msg << std::endl;

                //2.发送数据
                send(cm.sockfd_, msg.c_str(), msg.size(), 0);
            }

            return NULL;
        }

        static void* deal_start(void* arg){
            pthread_detach(pthread_self());
            ChatServer* cs = (ChatServer*)arg;

            while(1){
                //1. 从接收队列当中获取消息
                ChatMsg cm;
                cs->recv_que_->Pop(&cm);
                //2. 通过消息类型分业务处理
                int msg_type = cm.msg_type_;
                switch(msg_type){
                    case Register:{
                       cs->DealRegister(cm); 
                        break;
                    }
                    case Login:{
                        cs->DealLogin(cm);
                        break;
                    }
                    case AddFriend:{
                        cs->DealAddFriend(cm);
                        break;
                    }
                    case PushAddFriendMsg_Resp:{
                        cs->DealAddFriendResp(cm);
                        break;
                    }
                    case SendMsg: {
                        cs->DealSendMsg(cm);
                        break;
                    }
                    case GetFriendMsg:{
                        cs->GetAllFriendInfo(cm);
                        break;
                    }
                    default:{
                        break;
                    }
                }

                //3. 组织应答
            }
            return NULL;
        }
        void DealRegister(ChatMsg& cm){
            //1.获取注册信息
            std::string nickname = cm.GetValue("nickname");
            std::string school = cm.GetValue("school");
            std::string telnum = cm.GetValue("telnum");
            std::string passwd = cm.GetValue("passwd");
            //2.调用用户管理系统当中的注册接口
            int userid = -1;
            int ret = user_mana_->DealRegister(nickname, school, telnum, passwd, &userid);
            //3.回复应答
            cm.Clear();
            cm.msg_type_ = Register_Resp;
            if(ret < 0){
                cm.reply_status_ = REGISTER_FAILED;
            }else{
                cm.reply_status_ = REGISTER_SUCCESS;
            }
            cm.user_id_ = userid;

            send_que_->Push(cm);
        }

        void DealLogin(ChatMsg& cm){
            //1.获取数据
            std::string telnum = cm.GetValue("telnum");
            std::string passwd = cm.GetValue("passwd");
            //2.调用用户管理模块的代码
            int ret = user_mana_->DealLogin(telnum, passwd, cm.sockfd_);
            //3.回复应答
            cm.Clear();
            cm.msg_type_ = Login_Resp;
            if(ret < 0){
                cm.reply_status_ = LOGIN_FAILED;
            }else{
                cm.reply_status_ = LOGIN_SUCESSS;
            }
            cm.user_id_ = ret;

            send_que_->Push(cm);
        }

        void DealAddFriend(ChatMsg& cm){
            //1.获取被添加方的电话号码
            std::string tel = cm.GetValue("telnum");
            //添加方的userid
            int add_userid = cm.user_id_;

            cm.Clear();
            //2.查询被添加方是否是登录状态
            UserInfo be_add_ui;
            int ret = user_mana_->IsLogin(tel, &be_add_ui);
            if(ret == -1){
                //用户不存在
                cm.json_msg_ = AddFriend_Resp;
                cm.reply_status_ = ADDFRIEND_FAILED;
                cm.SetValue("content", "user not exist, please check friend tel num.");
                send_que_->Push(cm);
                return;
            }else if(ret == OFFLINE){
                std::cout << be_add_ui.nickname_ + " status is OFFLINE" << std::endl;
                //将消息先缓存下来， 择机发送
                return;
            }
            //ONLINE状态的
            //3.给被添加方推送添加好友请求
            UserInfo add_ui;
            user_mana_->GetUserInfo(add_userid, &add_ui);

            cm.sockfd_ = be_add_ui.tcp_socket_;
            cm.msg_type_ = PushAddFriendMsg;
            cm.SetValue("adder_nickname", add_ui.nickname_);
            cm.SetValue("adder_school", add_ui.school_);
            cm.SetValue("adder_userid", add_ui.userid_);
            
            send_que_->Push(cm);
        }

        void DealAddFriendResp(ChatMsg& cm){
            //1.获取双方的用户信息
            int reply_status = cm.reply_status_;
            //获取被添加方的用户信息
            int be_add_user = cm.user_id_;
            UserInfo be_userinfo;
            user_mana_->GetUserInfo(be_add_user, &be_userinfo);

            //获取添加方的用户信息-通过应答， 获取添加方的UserId
            int addr_user_id = atoi(cm.GetValue("userid").c_str());
            UserInfo ui;
            user_mana_->GetUserInfo(addr_user_id ,&ui);

            //2.判断响应状态
            cm.Clear();
            cm.sockfd_ = ui.tcp_socket_;
            cm.msg_type_ = AddFriend_Resp;
            if(reply_status == ADDFRIEND_FAILED){
                cm.reply_status_ = ADDFRIEND_FAILED;
                std::string content = "add user " + be_userinfo.nickname_ + " failed";
                cm.SetValue("content", content);
            }else if(reply_status == ADDFRIEND_SUCCESS){
                cm.reply_status_ = ADDFRIEND_SUCCESS;
                std::string content = "add user " + be_userinfo.nickname_ + " success";
                cm.SetValue("content", content);
                cm.SetValue("peer_nick_name", be_userinfo.nickname_);
                cm.SetValue("peer_school", be_userinfo.school_);
                cm.SetValue("peer_userid", be_userinfo.userid_);

                //用户管理模块当中要维护好友信息
                user_mana_->SetFriend(addr_user_id, be_add_user);
            }

            //TODO
            if(ui.user_status_ == OFFLINE){
                //消息就放到缓存队列当中， 择机发送
            }

            //3.给添加方回复响应
            send_que_->Push(cm);
        }


        void GetAllFriendInfo(ChatMsg& cm){
            //1. 好友信息的数据从用户管理模块当中获取到
            int user_id = cm.user_id_;

            cm.Clear();
            std::vector<int> fri;
            bool ret = user_mana_->GetFriends(user_id, &fri);
            if(ret == false){
                cm.reply_status_ = GETFRIEND_FAILED;
            }else{
                cm.reply_status_ = GETFRIEND_SUCCESS;
            }
            cm.msg_type_ = GetFriendMsg_Resp;

            for(size_t i = 0; i < fri.size(); i++){
                UserInfo tmp;
                user_mana_->GetUserInfo(fri[i], &tmp);

                Json::Value val;
                val["nickname"] = tmp.nickname_;
                val["school"] = tmp.school_;
                val["userid"] = tmp.userid_;

                cm.json_msg_.append(val);
            }

            send_que_->Push(cm);
        }

        void DealSendMsg(ChatMsg& cm){
            int send_id = cm.user_id_;
            int recv_id = cm.json_msg_["recvmsgid"].asInt();
            std::string send_msg = cm.json_msg_["msg"].asString();

            cm.Clear();

            UserInfo recv_ui;
            bool ret = user_mana_->GetUserInfo(recv_id, &recv_ui);
            //区分用户不存在和不在线两种状态
            //   用户不存在 ： 消息发送失败
            //   用户不在线： 发送方发送的消息缓存下来， 择机发送
            if(ret == false || recv_ui.user_status_ == OFFLINE){
                cm.msg_type_ = SendMsg_Resp;
                cm.reply_status_ = SENDMSG_FAILED;
                send_que_->Push(cm);
                return;
            }

            //代码能走到这里， 说明要给接收方推送消息了
            cm.Clear();
            cm.msg_type_ = SendMsg_Resp;
            cm.reply_status_ = SENDMSG_SUCCESS;
            send_que_->Push(cm);


            //获取发送方的用户信息
            UserInfo send_ui;
            user_mana_->GetUserInfo(send_id, &send_ui);

            cm.Clear();
            cm.msg_type_ = PushMsg;
            cm.sockfd_ = recv_ui.tcp_socket_;
            cm.SetValue("peer_nickname", send_ui.nickname_);
            cm.SetValue("peer_school", send_ui.school_);
            cm.json_msg_["peer_userid"] = send_ui.userid_;
            cm.SetValue("peer_msg", send_msg);
            send_que_->Push(cm);
        }

    private:
        //侦听套接字
        int tcp_sock_;
        int tcp_port_;

        //用户管理模块的实例化指针
        UserManager* user_mana_;

        //epoll操作句柄
        int epoll_fd_;

        //工作线程的数量
        int thread_count_;

        //就绪的文件描述符队列
        MsgQueue<int>* ready_sockfd_que_;

        //接收线程的队列
        MsgQueue<ChatMsg>* recv_que_;

        //发送线程的队列
        MsgQueue<ChatMsg>* send_que_;
};
