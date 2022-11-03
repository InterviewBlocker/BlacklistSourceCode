#include "pch.h"
#include "MsgQueue.h"


std::mutex g_lock_;

MsgQueue::MsgQueue() {
	v_msg_.resize(20);
	//v_msg_.reserve(20);
}
MsgQueue* MsgQueue::instance_ = NULL;

MsgQueue* MsgQueue::GetInstance() {
	if (NULL == instance_) {
		g_lock_.lock();
		if (NULL == instance_) {
			instance_ = new MsgQueue();
		}
		g_lock_.unlock();
	}
	return instance_;
}

/*
	需要同步吗？
*/
void MsgQueue::Push(int msg_type, const std::string& msg) {
	g_lock_.lock();
	v_msg_[msg_type].push(msg);
	g_lock_.unlock();
}


/*
	注册线程：
	    发送数据
		接收 -》 队列
		    Pop->阻塞

*/

void MsgQueue::Pop(int msg_type, std::string* msg) {
	//1.判断相应的消息类型的消息到底有没有
	while (1) {
		if (v_msg_[msg_type].empty()) {
			Sleep(1); //单位是 毫秒
			continue;
		}

		//2.返回给调用者具体的消息
		g_lock_.lock();
		*msg = v_msg_[msg_type].front();
		v_msg_[msg_type].pop();
		g_lock_.unlock();
		break;
	}
}




