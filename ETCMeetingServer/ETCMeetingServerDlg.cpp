
// ETCMeetingServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ETCMeetingServer.h"
#include "ETCMeetingServerDlg.h"
#include "ScreenCapturer.h"
#include "Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CETCMeetingServerDlg 对话框c

BOOL gShouldExit = FALSE;
BOOL gHasExited = FALSE;
int gInterval;
int gMeetingID;
int gPort;

CETCMeetingServerDlg::CETCMeetingServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CETCMeetingServerDlg::IDD, pParent)
	,m_nInteval(300)
	, m_nMeetingID(0)
	, m_nPort(8001)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CETCMeetingServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INTERVAL, m_nInteval);
	DDX_CBIndex(pDX, IDC_CMB_MEETINGID, m_nMeetingID);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
}

BEGIN_MESSAGE_MAP(CETCMeetingServerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_START, &CETCMeetingServerDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CETCMeetingServerDlg::OnBnClickedBtnStop)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CETCMeetingServerDlg 消息处理程序

BOOL CETCMeetingServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CETCMeetingServerDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CETCMeetingServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT SendPictureProc(LPVOID pParam);

void CETCMeetingServerDlg::OnBnClickedBtnStart()
{
	// TODO: 在此添加控件通知处理程序代码
	gShouldExit = false;
	gHasExited = false;

	UpdateData(TRUE);
	gInterval = m_nInteval;
	gMeetingID = m_nMeetingID;
	gPort = m_nPort;
	AfxBeginThread(SendPictureProc,NULL);

	GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_CMB_MEETINGID)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_INTERVAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(FALSE);
}

void CETCMeetingServerDlg::OnBnClickedBtnStop()
{
	// TODO: 在此添加控件通知处理程序代码
	gShouldExit = true;
	while (!gHasExited)
	{
		Sleep(500);
	}
	

	GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_CMB_MEETINGID)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_INTERVAL)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
}

UINT SendPictureProc(LPVOID pParam)
{
	// 创建发送Socket
	SOCKET sendSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 0, 0, 0);

	// 设置组播地址
	ip_mreq mcast;  
    mcast.imr_interface.S_un.S_addr = INADDR_ANY;  
    mcast.imr_multiaddr.S_un.S_addr = inet_addr(MULTICASTADDRESS[gMeetingID]);  
    int n =setsockopt(sendSocket,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&mcast,sizeof(mcast));
	// 设置组播TTL
	int optval = 8;  
    n = setsockopt(sendSocket,IPPROTO_IP,IP_MULTICAST_TTL,(char*)&optval,sizeof(int));  
	// 设置是否发送给自己
    int loop = 1;  
    n = setsockopt(sendSocket,IPPROTO_IP,IP_MULTICAST_LOOP,(char*)&loop,sizeof(int));  

	// 设置组播信息发送的目的地
	sockaddr_in remote;  
    remote.sin_addr.S_un.S_addr = inet_addr(MULTICASTADDRESS[gMeetingID]);  
    remote.sin_family = AF_INET;  
    remote.sin_port = htons(gPort);  


	CScreenCapturer cap;
	LPBYTE pBits = NULL;
	ULONG size = 0;
	int length = -1;	
	
	while(!gShouldExit)
	{
		// 抓取屏幕图片
		size = cap.GetPicture(&pBits);

		// 发送起始数据
		//sprintf(buf, "%s%d:%d", START_CODE, cap.GetScreenWidth(), cap.GetScreenHeight());
		//cap.GetScreenWidth();
		sendto(sendSocket, START_CODE, START_CODE_LENGTH, 0, (SOCKADDR*)&remote, sizeof(remote));

		// 发送各屏幕数据包
		int nums = size / MAX_SEND_SIZE + (size % MAX_SEND_SIZE == 0 ? 0 : 1);
		for(int i=0;i<nums-1;i++)
		{
			length = sendto(sendSocket, (char*)pBits, MAX_SEND_SIZE, 0, 
				(SOCKADDR*)&remote, sizeof(remote));
			pBits += length;
		}
		int remains = size % MAX_SEND_SIZE;
		if (remains != 0)
		{
			length = sendto(sendSocket, (char*)pBits, remains, 0, 
				(SOCKADDR*)&remote, sizeof(remote));
		}

		// 发送结束数据
		sendto(sendSocket, END_CODE, END_CODE_LENGTH, 0, (SOCKADDR*)&remote, sizeof(remote));

		Sleep(gInterval);
	}
	gHasExited = true;
	
	return 0;
}

void CETCMeetingServerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	while (gShouldExit && !gHasExited)
	{
		Sleep(500);
	}
	CDialog::OnClose();
}
