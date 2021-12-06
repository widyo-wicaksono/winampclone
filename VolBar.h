#if !defined(AFX_VOLBAR_H__B72D5286_802F_4D6B_8B02_FE5E9A4F7212__INCLUDED_)
#define AFX_VOLBAR_H__B72D5286_802F_4D6B_8B02_FE5E9A4F7212__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VolBar.h : header file
//
#define THUMB_WIDTH 21
#define THUMB_HEIGHT 11
#define THUMB_WIDTH_HALF 10
#define SCROLL_WINDOW_WIDTH 228
#define SCROLL_WINDOW_HEIGHT 11
#define SCROLL_WORK_AREA 208 //WINDOW_WIDTH - 2*Circle things
#define SCROLL_START_AREA 11
#define SCROLL_END_AREA 218

#include "DShowMP3.h"
/////////////////////////////////////////////////////////////////////////////
// CVolBar window

class CVolBar : public CStatic
{
// Construction
public:
	CVolBar();

// Attributes
public:
	CDShowMP3 * m_pDShowMP3;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	void DrawVolSlide();
	void SetBitmap();
	virtual ~CVolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVolBar)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
private:
	HBITMAP m_hBitmapV1;
	HBITMAP m_hBitmapV2;
	HBITMAP m_hBitmapTH;
	CRect rectV1;
	CRect rectV2;
	CRect rectTH;
	int m_iVolValue;
	bool m_bMouseDown;
	int m_iThumbPos;
	int m_iMousePos;
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOLBAR_H__B72D5286_802F_4D6B_8B02_FE5E9A4F7212__INCLUDED_)
