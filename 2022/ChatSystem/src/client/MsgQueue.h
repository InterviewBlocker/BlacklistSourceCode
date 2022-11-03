#pragma once
#include <vector>
#include <queue>
#include <string>
#include <mutex>


class MsgQueue{
public:
	static MsgQueue* GetInstance();
	void Push(int msg_type, const std::string& msg);
	void Pop(int msg_type, std::string* msg);
private:
	MsgQueue();
	MsgQueue(const MsgQueue&);
	static MsgQueue* instance_;

	std::vector<std::queue<std::string>> v_msg_;
};

