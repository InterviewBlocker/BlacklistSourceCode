// CDialogRegister.cpp: 实现文件
//

#include "pch.h"
#include "ChatSystemClient76.h"
#include "CDialogRegister.h"
#include "afxdialogex.h"

#include "ChatMsg.h"
#include "TcpSvr.h"
#include "MsgQueue.h"

// CDialogRegister 对话框

IMPLEMENT_DYNAMIC(CDialogRegister, CDialogEx)

CDialogRegister::CDialogRegister(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REGISTER, pParent)
	, m_nickname_(_T(""))
	, m_school_(_T(""))
	, m_telnum_(_T(""))
	, m_passwd_(_T(""))
{

}

CDialogRegister::~CDialogRegister()
{
}

void CDialogRegister::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITNICKNAME, m_nickname_);
	DDX_Text(pDX, IDC_EDITSCHOOL, m_school_);
	DDX_Text(pDX, IDC_EDITTELNUM, m_telnum_);
	DDX_Text(pDX, IDC_EDITPASSWD, m_passwd_);
}


BEGIN_MESSAGE_MAP(CDialogRegister, CDialogEx)
	ON_BN_CLICKED(IDC_BUTCOMMIT, &CDialogRegister::OnBnClickedButcommit)
END_MESSAGE_MAP()


// CDialogRegister 消息处理程序


void CDialogRegister::OnBnClickedButcommit()
{
	// TODO: 在此添加控件通知处理程序代码
		//1.获取用户的输入
	//获取输入控件当中最新的值
	UpdateData(TRUE);
	if (m_nickname_.IsEmpty() || 
		m_school_.IsEmpty()   ||
		m_telnum_.IsEmpty()   ||
		m_passwd_.IsEmpty()) {
		MessageBox(TEXT("输入内容不能为空"));
		return;
	}
	//2.组织登录消息（ChatMsg）
	ChatMsg cm;
	cm.msg_type_ = Register;
	cm.json_msg_["nickname"] = m_nickname_.GetString();
	cm.json_msg_["school"] = m_school_.GetString();
	cm.json_msg_["telnum"] = m_telnum_.GetString();
	cm.json_msg_["passwd"] = m_passwd_.GetString();
	//cm.SetValue("telnum", m_telnum_.GetString());
	//cm.SetValue("passwd", m_passwd_.GetString());
	std::string msg;
	cm.GetMsg(&msg);
	//3.获取TCP服务实例化指针
	TcpSvr* ts = TcpSvr::getInstance();
	if (ts == NULL) {
		MessageBox("获取tcp服务失败， 请重试..");
		return;
	}
	//4.发送登录消息
	ts->Send(msg);
	//5.获取消息队列的实例化指针
	MsgQueue* mq = MsgQueue::GetInstance();
	if (mq == NULL) {
		MessageBox("获取消息队列失败， 请联系开发人员...");
		return;
	}

	msg.clear();
	mq->Pop(Register_Resp, &msg);
	//6.获取登录应答
	cm.Clear();
	cm.PraseChatMsg(-1, msg);
	//7.判断登录应答当中的应答状态（LOGIN_SUCCESS/LOGIN_FAILED）
	if (cm.reply_status_ == REGISTER_SUCCESS) {
		MessageBox("register success");
		//退出当前的注册界面， 相当于回到了登录界面
		CDialog::OnCancel();
	}
	else {
		MessageBox("register failed, please retry...");
	}
}
