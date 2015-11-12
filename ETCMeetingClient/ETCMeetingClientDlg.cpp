
// ETCMeetingClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ETCMeetingClient.h"
#include "ETCMeetingClientDlg.h"
#include "Common.h"
#include "SetDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CETCMeetingClientDlg 对话框

BOOL gShouldExit = FALSE;
BOOL gHasExited = FALSE;
CWnd* gWnd = NULL;
CImage gImg;
int gMeetingID;
int gPort;
HDC gDC;

CETCMeetingClientDlg::CETCMeetingClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CETCMeetingClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CETCMeetingClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC, m_Pic);
}

BEGIN_MESSAGE_MAP(CETCMeetingClientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATEPIC, &CETCMeetingClientDlg::OnUpdatePic)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CETCMeetingClientDlg 消息处理程序
UINT RecvPictureProc(LPVOID pParam);
BOOL CETCMeetingClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CDC* pDC = GetDC();
	m_nHorzMaxSize = pDC->GetDeviceCaps(HORZRES);
	m_nVertMaxSize = pDC->GetDeviceCaps(VERTRES);
	SetScrollRange( SB_HORZ,0,m_nHorzMaxSize);
	SetScrollRange( SB_VERT,0,m_nVertMaxSize);
	m_nOneStep = 100;
	m_nHorzPos = 0;
	m_nVertPos = 0;

	
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	gShouldExit = false;
	gHasExited = false;
	gWnd = this;
	gDC = GetDC()->GetSafeHdc();
	// 下列语句将大幅改进缩放后的图像显示效果，但性能开销也较大
	//SetStretchBltMode(gDC, HALFTONE); 

	// 设置MeetingID和端口号
	CSetDialog dlg;
	dlg.DoModal();
	gMeetingID = dlg.m_nMeetingID;
	gPort = dlg.m_nPort;

	AfxBeginThread(RecvPictureProc, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CETCMeetingClientDlg::OnPaint()
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
		//DrawPic();
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CETCMeetingClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


UINT RecvPictureProc(LPVOID pParam)
{
	// 创建接收Socket
	SOCKET recvSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 0, 0, 0);

	// 绑定接收端Socket
	sockaddr_in addr; 
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr =  htonl(INADDR_ANY);
	addr.sin_port = htons(gPort);
	bind(recvSocket, (sockaddr*)&addr, sizeof(addr));

	// 设置组播地址
	ip_mreq mcast;  
    mcast.imr_interface.S_un.S_addr = INADDR_ANY;  
    mcast.imr_multiaddr.S_un.S_addr = inet_addr(MULTICASTADDRESS[gMeetingID]);  
    setsockopt(recvSocket,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&mcast,sizeof(mcast));
	// 设置组播TTL
	int optval = 8;  
    setsockopt(recvSocket,IPPROTO_IP,IP_MULTICAST_TTL,(char*)&optval,sizeof(int));  
	// 设置是否发送给自己
    int loop = 1;  
    setsockopt(recvSocket,IPPROTO_IP,IP_MULTICAST_LOOP,(char*)&loop,sizeof(int));  


	IStream* pStream = NULL;
	HGLOBAL hGlobal;
	CreateStreamOnHGlobal(NULL, TRUE, &pStream);
	GetHGlobalFromStream(pStream, &hGlobal);
	char buf[MAXSENDLENGTH];	

	LPBYTE pBits = NULL;
	ULONG size = 0;
	int length = -1;
	ULONG num;

	ULARGE_INTEGER pSeek;    
	LARGE_INTEGER dlibMove = {0};
	int nRecv = 0;
	while(!gShouldExit)
	{
		nRecv = recvfrom(recvSocket, buf, MAXSENDLENGTH, 0, NULL, NULL); 
		if (nRecv <= 0)
		{
			continue;
		}
		if (strncmp(buf, STARTCODE, STARTCODELENGTH) == 0)		// 检查是否帧头
		{			
			pStream->Seek(dlibMove, STREAM_SEEK_SET , &pSeek);
			continue;
		}
		if (strncmp(buf, ENDCODE, ENDCODELENGTH) == 0)			// 检查是否帧结尾
		{
			if (!gImg.IsNull())
			{
				gImg.Destroy();
			}
			gImg.Load(pStream);
			gWnd->SendMessage(WM_UPDATEPIC);
			continue;
		}
		pStream->Write((LPVOID)buf, nRecv, &num);				// 如果是数据桢，直接写入内存
	}
	pStream->Release();
	GlobalFree(hGlobal);
	gHasExited = true;
	
	return 0;
}

void CETCMeetingClientDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	gShouldExit = true;
	if (!gHasExited)
	{
		Sleep(500);
	}
	CDialog::OnClose();
}

LRESULT CETCMeetingClientDlg::OnUpdatePic(WPARAM wParam, LPARAM lParam)
{
	if (!IsIconic())
	{
		if (!gImg.IsNull())
		{
			HDC hDC = m_Pic.GetDC()->GetSafeHdc();
			if (hDC != 0)
				gImg.BitBlt(m_Pic.GetDC()->GetSafeHdc(), -m_nHorzPos, -m_nVertPos);
		}
	}
	return 0;
}
void CETCMeetingClientDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rect;
	GetClientRect(&rect);
	if (m_Pic.m_hWnd != NULL)
		m_Pic.SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE);
	
	m_nHorzPageSize = rect.Width();
	m_nVertPageSize = rect.Height();
	m_nHorzPos = 0;
	m_nVertPos = 0;

	SCROLLINFO si;
	GetScrollInfo(SB_HORZ, &si);
	si.nMax = m_nHorzMaxSize;
    si.nPage = m_nHorzPageSize;
	si.nPos = 0;
	SetScrollInfo(SB_HORZ, &si);

	GetScrollInfo(SB_VERT, &si);
    si.nPage = m_nVertPageSize;
	si.nMax = m_nVertMaxSize;
	si.nPos = 0;
	SetScrollInfo(SB_VERT, &si);
}


void CETCMeetingClientDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CETCMeetingClientDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CETCMeetingClientDlg::DrawPic()
{
	this->Invalidate();
	OnUpdatePic(0, 0);
}
