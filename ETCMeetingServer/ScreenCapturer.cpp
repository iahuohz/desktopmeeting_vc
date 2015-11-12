#include "StdAfx.h"
#include "ScreenCapturer.h"

CScreenCapturer::CScreenCapturer(void)
{
	Init();
}

CScreenCapturer::~CScreenCapturer(void)
{
	Deinit();
}

void CScreenCapturer::Init()
{
	m_dcSource.CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	m_dcMemory.CreateCompatibleDC(&m_dcSource);
	m_nWidth = GetDeviceCaps(m_dcSource.GetSafeHdc(), HORZRES);
	m_nHeight = GetDeviceCaps(m_dcSource.GetSafeHdc(), VERTRES);

	m_bmpCompatible.CreateCompatibleBitmap(&m_dcSource, m_nWidth, m_nHeight);
	m_bmpOld = m_dcMemory.SelectObject(&m_bmpCompatible);

	m_pStream = NULL;
	CreateStreamOnHGlobal(NULL, TRUE, &m_pStream);
	GetHGlobalFromStream(m_pStream, &m_hGlobal);
}

void CScreenCapturer::Deinit()
{
	m_dcMemory.SelectObject(&m_bmpCompatible);

	m_dcMemory.DeleteDC();
	m_dcSource.DeleteDC();

	m_pStream->Release();
	GlobalFree(m_hGlobal);
}

ULONG CScreenCapturer::GetPicture(LPBYTE *pBits)
{
	// 捕捉桌面到内存中
	m_dcMemory.BitBlt(0, 0, m_nWidth, m_nHeight, &m_dcSource, 0, 0, SRCCOPY);

	// 处理鼠标捕捉
	CI.cbSize = sizeof(CURSORINFO);
	GetCursorInfo(&CI);
	ICONINFO IconInfo = { 0 };
	if (GetIconInfo(CI.hCursor, &IconInfo))
	{
		CI.ptScreenPos.x -= IconInfo.xHotspot;
		CI.ptScreenPos.y -= IconInfo.yHotspot;
		if (NULL != IconInfo.hbmMask)
			DeleteObject(IconInfo.hbmMask);
		if (NULL != IconInfo.hbmColor)
			DeleteObject(IconInfo.hbmColor);
	}
	DrawIconEx(m_dcMemory.GetSafeHdc(), CI.ptScreenPos.x, CI.ptScreenPos.y, CI.hCursor, 0, 0, 0, NULL, DI_NORMAL | DI_COMPAT);

	// 开始转换
	m_imgConvertor.Attach(m_bmpCompatible.operator HBITMAP());
	m_imgConvertor.Save(m_pStream, Gdiplus::ImageFormatJPEG);
	ULONG size = GlobalSize(m_hGlobal);
	*pBits = (LPBYTE) GlobalLock(m_hGlobal);

	m_imgConvertor.Detach();
	// 转换结束

	ULARGE_INTEGER pSeek;
	LARGE_INTEGER dlibMove = { 0 };
	m_pStream->Seek(dlibMove, STREAM_SEEK_SET, &pSeek);

	return size;
}
