#pragma once

class CScreenCapturer
{
private:
	CImage m_imgConvertor;
	CDC m_dcSource;
	CDC m_dcMemory;
	CBitmap m_bmpCompatible;
	CBitmap* m_bmpOld;
	int m_nWidth;
	int m_nHeight;
	IStream* m_pStream;
	HGLOBAL m_hGlobal;
	CURSORINFO CI;
	ICONINFO Info;

public:
	CScreenCapturer(void);
	~CScreenCapturer(void);

private:
	void Init();
	void Deinit();

public:
	ULONG GetPicture(LPBYTE* pBits);
	int GetScreenWidth(){return m_nWidth;}
	int GetScreenHeight(){return m_nHeight;}
};
