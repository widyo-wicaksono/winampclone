#if !defined(AFX_INFODLG_H__B71EB7C6_A612_492E_AB90_9B7B608447A8__INCLUDED_)
#define AFX_INFODLG_H__B71EB7C6_A612_492E_AB90_9B7B608447A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg dialog

#include "MP3\WMP3.h"

class CInfoDlg : public CDialog
{
// Construction
public:
	int GetTextLength(LPCTSTR sStr, CWnd * pWnd);
	void SetBitmap();
	HRESULT GetMP3Tag(MP3ID3V1TAG *sTag, LPCTSTR sPath);
	HWND m_hWndParrent;
	CString m_sPath;
	CInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInfoDlg)
	enum { IDD = IDD_INFO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInfoDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
private:
	HBITMAP m_hBitmap;
	HBRUSH m_brushHollow;
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFODLG_H__B71EB7C6_A612_492E_AB90_9B7B608447A8__INCLUDED_)
