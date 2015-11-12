// ScrollWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "ETCMeetingClient.h"
#include "ScrollWnd.h"


// CScrollWnd

IMPLEMENT_DYNAMIC(CScrollWnd, CButton)

CScrollWnd::CScrollWnd() : m_pImg(NULL)
{

}

CScrollWnd::~CScrollWnd()
{
}


BEGIN_MESSAGE_MAP(CScrollWnd, CButton)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CScrollWnd 消息处理程序

void CScrollWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nSBCode)
	{
		case SB_LEFT:
			m_nVertPos = 0;
			SetScrollPos(SB_VERT,m_nVertPos);
			DrawPic();
			break;
		 case SB_LINELEFT:
		 case SB_PAGELEFT:
			m_nVertPos -= m_nOneStep;
			if(m_nVertPos<0)
			{
				m_nVertPos = 0;
			}
			SetScrollPos(SB_VERT,m_nVertPos);
			DrawPic();
			break;
		 case SB_LINERIGHT:
		 case SB_PAGERIGHT:
			m_nVertPos += m_nOneStep;
			if(m_nVertPos>m_nVertMaxSize)
			{
				m_nVertPos = m_nVertMaxSize;
			}
			SetScrollPos(SB_VERT,m_nVertPos);
			DrawPic();
			break;
		 case SB_RIGHT:
			 m_nVertPos = m_nVertMaxSize;
			 SetScrollPos(SB_VERT,m_nVertPos);
			 DrawPic();
			 break;
		 case SB_THUMBPOSITION:
		 case SB_THUMBTRACK:
			 m_nVertPos = nPos;
			 SetScrollPos(SB_VERT,m_nVertPos);
			 DrawPic();
			 break;
		default:
				break;
	}
	CButton::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CScrollWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nSBCode)
	{
		case SB_LEFT:
			m_nHorzPos = 0;
			SetScrollPos(SB_HORZ,m_nHorzPos);
			DrawPic();
			break;
		 case SB_LINELEFT:
		 case SB_PAGELEFT:
			m_nHorzPos -= m_nOneStep;
			if(m_nHorzPos<0)
			{
				m_nHorzPos = 0;
			}
			SetScrollPos(SB_HORZ,m_nHorzPos);
			DrawPic();
			break;
		 case SB_LINERIGHT:
		 case SB_PAGERIGHT:
			m_nHorzPos += m_nOneStep;
			if(m_nHorzPos>m_nHorzMaxSize)
			{
				m_nHorzPos = m_nHorzMaxSize;
			}
			SetScrollPos(SB_HORZ,m_nHorzPos);
			DrawPic();
			break;
		 case SB_RIGHT:
			 m_nHorzPos = m_nHorzMaxSize;
			 SetScrollPos(SB_HORZ,m_nHorzPos);
			 DrawPic();
			 break;
		 case SB_THUMBPOSITION:
		 case SB_THUMBTRACK:
			 m_nHorzPos = nPos;
			 SetScrollPos(SB_HORZ,m_nHorzPos);
			 DrawPic();
			 break;
		default:
				break;
	}
	CButton::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CScrollWnd::DrawPic()
{
	m_pImg->BitBlt(GetDC()->GetSafeHdc(), -m_nHorzPos, -m_nVertPos);
}

void CScrollWnd::Init(int maxWidth, int maxHeight)
{
	m_nHorzMaxSize = maxWidth;
	m_nVertMaxSize = maxHeight;
	CRect rc;
	GetClientRect(&rc);
	CRect rect;
	
	
	m_nHorzPageSize = rc.Width();
	m_nVertPageSize = rc.Height();

	m_nOneStep = 100;
	m_nHorzPos = 0;
	m_nVertPos = 0;

	SetScrollRange( SB_HORZ,0,m_nHorzMaxSize);
	SetScrollRange( SB_VERT,0,m_nVertMaxSize);

	LPSCROLLINFO  lpsi = new SCROLLINFO;
	GetScrollInfo(SB_HORZ,lpsi);
    lpsi->nPage = m_nHorzPageSize;
	SetScrollInfo(SB_HORZ,lpsi);
	GetScrollInfo(SB_VERT,lpsi);
    lpsi->nPage = m_nVertPageSize;
	SetScrollInfo(SB_VERT,lpsi);
	delete lpsi;
}

void CScrollWnd::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{

	// TODO:  添加您的代码以绘制指定项
	DrawPic();
}

void CScrollWnd::OnSize(UINT nType, int cx, int cy)
{
	CButton::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rect;
	GetClientRect(&rect);
	m_nHorzPageSize = rect.Width();
	m_nVertPageSize = rect.Height();

	SCROLLINFO si;
	GetScrollInfo(SB_HORZ, &si);
	si.nMax = m_nHorzMaxSize;
    si.nPage = m_nHorzPageSize;
	SetScrollInfo(SB_HORZ, &si);

	GetScrollInfo(SB_VERT, &si);
    si.nPage = m_nVertPageSize;
	si.nMax = m_nVertMaxSize;
	SetScrollInfo(SB_VERT, &si);
}

int CScrollWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	EnableScrollBar(SB_HORZ);
	EnableScrollBar(SB_VERT);
	return 0;
}
