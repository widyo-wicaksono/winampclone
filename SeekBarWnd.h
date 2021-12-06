#if !defined(AFX_SEEKBARWND_H__05BA2136_DFDB_45EF_AB63_E97E566F2BF6__INCLUDED_)
#define AFX_SEEKBARWND_H__05BA2136_DFDB_45EF_AB63_E97E566F2BF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SeekBarWnd.h : header file
//
#include "DShowMP3.h"
#include "BigTime.h"
#include "FileLog.h"

#define SEEK_THUMB_WIDTH 25
#define SEEK_THUMB_HEIGHT 11
#define SEEK_THUMB_WIDTH_HALF 12
#define SEEK_SCROLL_WINDOW_WIDTH 293
#define SEEK_SCROLL_WINDOW_HEIGHT 11
#define SEEK_SCROLL_WORK_AREA 273 //WINDOW_WIDTH - 2*Circle things
#define SEEK_SCROLL_START_AREA 11
#define SEEK_SCROLL_END_AREA 283

/////////////////////////////////////////////////////////////////////////////
// CSeekBarWnd window

class CSeekBarWnd : public CStatic
{
// Construction
public:
	bool m_bIsSeeking;
	CSeekBarWnd();

// Attributes
public:
	CDShowMP3 * m_pDShowMP3;
	CBigTime * m_pBigTime;
	int * m_iGraphCondition;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSeekBarWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	HWND m_hHwndMain;
	void SetReferenceTime();
	void DrawSeekBar();
	void SetBitmap();
	virtual ~CSeekBarWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSeekBarWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
private:
	HBITMAP m_hBitmapS;
	HBITMAP m_hBitmapTH;
	CRect rectS;
	CRect rectTH;
	CFileLog m_Log;
	int m_iSeekValue;
	bool m_bMouseDown;
	int m_iThumbPos;
	int m_iMousePos;
//	bool m_bIsGraphRunning;
	REFERENCE_TIME m_rtNow;
	REFERENCE_TIME m_rtStop;
	REFERENCE_TIME m_rtFactor;
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEEKBARWND_H__05BA2136_DFDB_45EF_AB63_E97E566F2BF6__INCLUDED_)
