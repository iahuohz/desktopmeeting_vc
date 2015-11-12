
// ETCMeetingServerDlg.h : 头文件
//

#pragma once


// CETCMeetingServerDlg 对话框
class CETCMeetingServerDlg : public CDialog
{
// 构造
public:
	CETCMeetingServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ETCMEETINGSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
	int m_nInteval;
	int m_nMeetingID;
	UINT m_nPort;
	afx_msg void OnClose();
};

