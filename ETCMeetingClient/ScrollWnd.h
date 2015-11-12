#pragma once


// CScrollWnd
class CScrollWnd : public CButton
{
	DECLARE_DYNAMIC(CScrollWnd)

public:
	CScrollWnd();
	virtual ~CScrollWnd();
	void SetImage(CImage* pImg){m_pImg = pImg;}
	void Init(int maxWidth, int maxHeight);

private:
	CImage* m_pImg;
	int m_nHorzMaxSize;
	int m_nVertMaxSize;
	int m_nHorzPageSize;
	int m_nVertPageSize;
	int m_nOneStep;
	int m_nHorzPos;
	int m_nVertPos;
	void DrawPic();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


