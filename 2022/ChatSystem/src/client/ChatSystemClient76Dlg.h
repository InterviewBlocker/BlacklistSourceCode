
// ChatSystemClient76Dlg.h: 头文件
//

#pragma once


// CChatSystemClient76Dlg 对话框
class CChatSystemClient76Dlg : public CDialogEx
{
// 构造
public:
	CChatSystemClient76Dlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATSYSTEMCLIENT76_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	// 用户输入的电话号码
	CString m_telnum_;
	// 用户输入的密码
	CString m_passwd_;
public:
	afx_msg void OnBnClickedButlogin();
	afx_msg void OnBnClickedButregister();
};
