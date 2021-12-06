#if !defined(AFX_BIGTIME_H__B51D4C0C_4A1E_4FDF_852E_86A10ED9AB59__INCLUDED_)
#define AFX_BIGTIME_H__B51D4C0C_4A1E_4FDF_852E_86A10ED9AB59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BigTime.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBigTime window

class CBigTime : public CStatic
{
// Construction
public:
	CBigTime();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBigTime)
	//}}AFX_VIRTUAL

// Implementation
public:
	UINT GetNumGraphic(LPCTSTR str);
	void SetTime(UINT uTime);
	void Pause(bool bPause);
	void DrawBGText(LPCTSTR s);
	void CreateFont();
	UINT m_uTime;
	void Stop();
	void Start();
	virtual ~CBigTime();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBigTime)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
private:
	CFont m_cFont;
	CString m_sTime;
	UINT m_uLastTimerCalled;
	UINT m_uPausedTime;
	UINT m_uTimeForNextUpdate;
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BIGTIME_H__B51D4C0C_4A1E_4FDF_852E_86A10ED9AB59__INCLUDED_)
