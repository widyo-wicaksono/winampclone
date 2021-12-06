#if !defined(AFX_FAKEEQU_H__33153B23_8ECE_420D_A770_E82196E866DC__INCLUDED_)
#define AFX_FAKEEQU_H__33153B23_8ECE_420D_A770_E82196E866DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FakeEqu.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFakeEqu window

class CFakeEqu : public CStatic
{
// Construction
public:
	CFakeEqu();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFakeEqu)
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_iLastHeight[HISTOGRAM_COUNT];
	void UpdateEqu();
	bool m_bIsPaused;
	void DrawEqualizer();
	void UpdateTime();
	void InitData();
	void StopEqu();
	void Stop();
	void Start();
	virtual ~CFakeEqu();
	HWND m_hParrentWnd;
	// Generated message map functions
protected:
	//{{AFX_MSG(CFakeEqu)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
private:
	//int m_iLastHeight[5];
	//int m_iLastPost[5];
	int m_iBarWidth;
	int m_iBarSpace;
	int m_iStopped;
	CRect m_rect;
	CString m_sStoped;
	bool m_bIsRun;
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FAKEEQU_H__33153B23_8ECE_420D_A770_E82196E866DC__INCLUDED_)
