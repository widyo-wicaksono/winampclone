#if !defined(AFX_RUNINGTEXT_H__CE78B307_3298_4A9E_83E0_C3FD546977EA__INCLUDED_)
#define AFX_RUNINGTEXT_H__CE78B307_3298_4A9E_83E0_C3FD546977EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RuningText.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRuningText window

class CRuningText : public CStatic
{
// Construction
public:

	CRuningText();
	CWnd * m_pParrent;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRuningText)
	//}}AFX_VIRTUAL

// Implementation
public:
	void DrawBg();
	void UpdateTextPosition();
	void DrawBGText(LPCTSTR s);
	void SetDCText(LPCTSTR s);
	void CreateFont();
	int GetTextLength(LPCTSTR sStr);
	void Stop();
	void Start();
	virtual ~CRuningText();
	CString m_sText;
	// Generated message map functions
protected:
	//{{AFX_MSG(CRuningText)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	//}}AFX_MSG
private:
	int m_iLenFit;
	int m_iCurTextIndex;
	int m_iDir;
	int m_iCapacity;
	CString m_sBuffer;
	CFont m_cFont;
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RUNINGTEXT_H__CE78B307_3298_4A9E_83E0_C3FD546977EA__INCLUDED_)
