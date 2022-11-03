#pragma once


// CDialogRegister 对话框

class CDialogRegister : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogRegister)

public:
	CDialogRegister(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDialogRegister();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGISTER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CString m_nickname_;
	CString m_school_;
	CString m_telnum_;
	CString m_passwd_;
public:
	afx_msg void OnBnClickedButcommit();
};
