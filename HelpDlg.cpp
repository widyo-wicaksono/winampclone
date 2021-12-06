// HelpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WcxPlayer.h"
#include "HelpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHelpDlg dialog


CHelpDlg::CHelpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHelpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHelpDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetBitmap();
	m_iAnim = false;
}


void CHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHelpDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHelpDlg, CDialog)
	//{{AFX_MSG_MAP(CHelpDlg)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelpDlg message handlers

BOOL CHelpDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	GetClientRect(&rect);

	CDC dc;
	dc.CreateCompatibleDC(pDC);
	HBITMAP olBitmap;
	if(!m_iAnim)
		 olBitmap = (HBITMAP)dc.SelectObject(m_hBitmap1);
	else
		olBitmap = (HBITMAP)dc.SelectObject(m_hBitmap2);

	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);
	
	dc.SelectObject(olBitmap);
	dc.DeleteDC();
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

void CHelpDlg::SetBitmap()
{
	HINSTANCE  hInstResource = NULL;
    
    // Find correct resource handle
    hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_HELP1), RT_BITMAP);
    
    // Load bitmap In
    m_hBitmap1 = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(IDB_HELP1),
                                   IMAGE_BITMAP, 0, 0, 0);
	// Find correct resource handle
    hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_HELP2), RT_BITMAP);
    
    // Load bitmap In
    m_hBitmap2 = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(IDB_HELP2),
                                   IMAGE_BITMAP, 0, 0, 0);
    
}

void CHelpDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if (m_hBitmap1)
        ::DeleteObject (m_hBitmap1);
	if (m_hBitmap2)
        ::DeleteObject (m_hBitmap2);
    m_hBitmap1 = NULL;
	m_hBitmap2=NULL;
}

BOOL CHelpDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowLong (m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) |WS_EX_LAYERED/*|WS_EX_TOOLWINDOW*/ );
	m_pSetLayeredWindowAttributes (m_hWnd, RGB(255,0,0), 255, LWA_COLORKEY|LWA_ALPHA);
	SetTimer(2,1000,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHelpDlg::OnOK() 
{
	// TODO: Add extra validation here
	this->DestroyWindow();
	::PostMessage(m_hWndParrent,WM_HELP_EXIT,NULL,NULL);
	//CDialog::OnOK();
}

void CHelpDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CDialog::OnLButtonDown(nFlags, point);
}

void CHelpDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==2)
	{
		KillTimer(2);
		if(m_iAnim)
			m_iAnim=false;
		else
			m_iAnim=true;
		Invalidate();
		SetTimer(2,1000,NULL);
	}
	CDialog::OnTimer(nIDEvent);
}
