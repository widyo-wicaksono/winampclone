// WinAmpAboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WcxPlayer.h"
#include "WinAmpAboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinAmpAboutDlg dialog


CWinAmpAboutDlg::CWinAmpAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWinAmpAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWinAmpAboutDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWinAmpAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWinAmpAboutDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWinAmpAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CWinAmpAboutDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinAmpAboutDlg message handlers

void CWinAmpAboutDlg::OnOK() 
{
	// TODO: Add extra validation here
	this->DestroyWindow();
	::PostMessage(m_hWndParrent,WM_ABOUT_EXIT,NULL,NULL);
	//CDialog::OnOK();
}

void CWinAmpAboutDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}
