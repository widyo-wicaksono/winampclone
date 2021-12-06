// AdvDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WcxPlayer.h"
#include "AdvDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvDlg dialog


CAdvDlg::CAdvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdvDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetBitmap();
//	m_pDShowMP3=NULL;
}


void CAdvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvDlg, CDialog)
	//{{AFX_MSG_MAP(CAdvDlg)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvDlg message handlers

void CAdvDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
//	CDialog::OnCancel();
}

void CAdvDlg::OnOK() 
{
	// TODO: Add extra validation here
	this->DestroyWindow();
	::PostMessage(m_hWndParrent,WM_ADV_EXIT,NULL,NULL);
//	CDialog::OnOK();
}

BOOL CAdvDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	GetClientRect(&rect);

	CDC dc;
	dc.CreateCompatibleDC(pDC);
	HBITMAP olBitmap;
	olBitmap = (HBITMAP)dc.SelectObject(m_hBitmap);
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);
	
	dc.SelectObject(olBitmap);
	dc.DeleteDC();
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

BOOL CAdvDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowLong (m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) |WS_EX_LAYERED/*|WS_EX_TOOLWINDOW*/ );
	m_pSetLayeredWindowAttributes (m_hWnd, RGB(255,0,0), 255, LWA_COLORKEY|LWA_ALPHA);
	m_cVolBar.Create(NULL, WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE|WS_GROUP,CRect(0,0,56,14), this);
	m_cVolBar.SetWindowPos(NULL,40,12,56,14,SWP_NOZORDER);
	m_cSeekBar.Create(NULL, WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE|WS_GROUP,CRect(0,0,160,10), this);
	m_cSeekBar.SetWindowPos(NULL,18,42,160,10,SWP_NOZORDER);
	MoveWindow(m_rect.left,m_rect.top,196,64);
	m_cSeekBar.m_hHwndMain = m_hWndParrent;
//	SetTimer(2,10,NULL);

	m_cSeekBar.SetReferenceTime();
	m_cSeekBar.SetTimer(2,1000,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAdvDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	if (m_hBitmap)
        ::DeleteObject (m_hBitmap);
	m_hBitmap=NULL;
	// TODO: Add your message handler code here
	
}

void CAdvDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CDialog::OnLButtonDown(nFlags, point);
}

void CAdvDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(2==nIDEvent)
	{
		KillTimer(2);
		static int factor=0;
		m_pSetLayeredWindowAttributes (m_hWnd, RGB(255,0,0), factor, LWA_COLORKEY|LWA_ALPHA);
		factor = factor+10;
		if(factor<=255)
			SetTimer(2,10,NULL);
		else
			factor=0;
	}
	CDialog::OnTimer(nIDEvent);
}

void CAdvDlg::SetBitmap()
{
	HINSTANCE  hInstResource = NULL;
    
    // Find correct resource handle
    hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_ADVSKIN), RT_BITMAP);
    
    // Load bitmap In
    m_hBitmap = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(IDB_ADVSKIN),
                                   IMAGE_BITMAP, 0, 0, 0);

}
