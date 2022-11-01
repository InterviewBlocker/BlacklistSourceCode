#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <string.h>
#include <thread>

struct UserInfo {
public:
	std::string nickname_; 
	std::string school_;
	int user_id_;
	//历史和这个好友发送过的消息
	std::vector<std::string> history_msg_;
	//未读的消息个数
	int msg_cnt_;
};


// CDialogChatWin 对话框

class CDialogChatWin : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogChatWin)

public:
	CDialogChatWin(int userid, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDialogChatWin();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHAT };
#endif
	int user_id_;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnLbnSelchangeList1();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonsendmsg();
	CListBox m_userlist_;
	std::vector<struct UserInfo> fri_vec_;
	//刷新userlist区域
	void RefreshUserList();
	//当前消息要发送的好友id， 随着用户点击不同的好友， 该变量动态变化
	int send_user_id_;
	CListBox m_output_;
	CString m_input_;
	CEdit m_input_edit_;
	afx_msg void OnLbnSelchangeListuser();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButaddfriend();
};
