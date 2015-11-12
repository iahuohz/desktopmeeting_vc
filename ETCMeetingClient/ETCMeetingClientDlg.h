
// ETCMeetingClientDlg.h : 头文件
//

#pragma once

#define WM_UPDATEPIC 10001
#include "ScrollWnd.h"
#include "afxwin.h"

class ThreadData
{
public:
	CImage* m_pImage;
	CWnd* m_pDlg;
};

// CETCMeetingClientDlg 对话框
class CETCMeetingClientDlg : public CDialog
{
// 构造
public:
	CETCMeetingClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ETCMEETINGCLIENT_DIALOG };

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

private:
	int m_nHorzMaxSize;
	int m_nVertMaxSize;
	int m_nHorzPageSize;
	int m_nVertPageSize;
	int m_nOneStep;
	int m_nHorzPos;
	int m_nVertPos;
	void DrawPic();

public:
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	LRESULT OnUpdatePic(WPARAM wParam, LPARAM lParam);
	CStatic m_Pic;
};
