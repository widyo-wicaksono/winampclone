#if !defined(AFX_WINAMPABOUTDLG_H__F72A0CD3_E9DA_436D_82B5_524DF2A38396__INCLUDED_)
#define AFX_WINAMPABOUTDLG_H__F72A0CD3_E9DA_436D_82B5_524DF2A38396__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WinAmpAboutDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWinAmpAboutDlg dialog

class CWinAmpAboutDlg : public CDialog
{
// Construction
public:
	CWinAmpAboutDlg(CWnd* pParent = NULL);   // standard constructor
	HWND m_hWndParrent;
// Dialog Data
	//{{AFX_DATA(CWinAmpAboutDlg)
	enum { IDD = IDD_ABOUTBOX2 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinAmpAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWinAmpAboutDlg)
	virtual void OnOK();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINAMPABOUTDLG_H__F72A0CD3_E9DA_436D_82B5_524DF2A38396__INCLUDED_)
