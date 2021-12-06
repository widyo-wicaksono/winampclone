#if !defined(AFX_ADVDLG_H__4E35F91C_F95E_4A4E_811C_C8D26F74D1F5__INCLUDED_)
#define AFX_ADVDLG_H__4E35F91C_F95E_4A4E_811C_C8D26F74D1F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvDlg.h : header file
//
#include "VolBar.h"
#include "SeekBarWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CAdvDlg dialog

class CAdvDlg : public CDialog
{
// Construction
public:
	CRect m_rect;
	void SetBitmap();
	CVolBar m_cVolBar;
	CSeekBarWnd m_cSeekBar;
	HWND m_hWndParrent;
	lpfnSetLayeredWindowAttributes m_pSetLayeredWindowAttributes;
	CAdvDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAdvDlg)
	enum { IDD = IDD_ADV };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdvDlg)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
private:
	bool m_iAnim;
	HBITMAP m_hBitmap;	
	
	
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVDLG_H__4E35F91C_F95E_4A4E_811C_C8D26F74D1F5__INCLUDED_)
