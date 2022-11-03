
// ChatSystemClient76Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ChatSystemClient76.h"
#include "ChatSystemClient76Dlg.h"
#include "afxdialogex.h"

#include <string>

#include "ChatMsg.h"
#include "TcpSvr.h"
#include "MsgQueue.h"
#include "CDialogRegister.h"
#include "CDialogChatWin.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CChatSystemClient76Dlg 对话框



CChatSystemClient76Dlg::CChatSystemClient76Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATSYSTEMCLIENT76_DIALOG, pParent)
	, m_telnum_(_T(""))
	, m_passwd_(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatSystemClient76Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITTELNUM, m_telnum_);
	DDX_Text(pDX, IDC_EDITPASSWD, m_passwd_);
}

BEGIN_MESSAGE_MAP(CChatSystemClient76Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTLOGIN, &CChatSystemClient76Dlg::OnBnClickedButlogin)
	ON_BN_CLICKED(IDC_BUTREGISTER, &CChatSystemClient76Dlg::OnBnClickedButregister)
END_MESSAGE_MAP()


// CChatSystemClient76Dlg 消息处理程序

BOOL CChatSystemClient76Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CChatSystemClient76Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChatSystemClient76Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CChatSystemClient76Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




// 当登录被按钮被点击之后， 触发调用该函数
void CChatSystemClient76Dlg::OnBnClickedButlogin() {
	//1.获取用户的输入
	//获取输入控件当中最新的值
	UpdateData(TRUE);
	if (m_telnum_.IsEmpty() || m_passwd_.IsEmpty()) {
		MessageBox(TEXT("输入内容不能为空"));
		return;
	}
	//2.组织登录消息（ChatMsg）
	ChatMsg cm;
	cm.msg_type_ = Login;
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
	mq->Pop(Login_Resp, &msg);
	//6.获取登录应答
	cm.Clear();
	cm.PraseChatMsg(-1, msg);
	//7.判断登录应答当中的应答状态（LOGIN_SUCCESS/LOGIN_FAILED）
	if (cm.reply_status_ == LOGIN_SUCESSS) {
		//MessageBox("login success");
		CDialog::OnCancel();
		//跳转到聊天界面
		CDialogChatWin cdc(cm.user_id_);
		cdc.DoModal();
	}
	else {
		MessageBox("login failed, please retry...");
	}
}

// 当注册按钮被点击之后， 触发调用该函数
void CChatSystemClient76Dlg::OnBnClickedButregister() {
	// 展示注册界面
	CDialogRegister cdr;
	//模态展示界面， 当展示下面的界面是， 调用界面是不能被操作的。
	cdr.DoModal();
}
