// SeekBarWnd.cpp : implementation file
//

#include "stdafx.h"
#include "WcxPlayer.h"
#include "SeekBarWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSeekBarWnd

CSeekBarWnd::CSeekBarWnd()
{
	rectS.left=0;
	rectTH.left=0;
	rectS.top=0;
	rectTH.top=0;
	rectS.right=160;
	rectTH.right=234;
	rectS.bottom=10;
	rectTH.bottom=10;
	SetBitmap();
	m_iSeekValue=0;
	m_bMouseDown=false;
	m_pDShowMP3 = NULL;
	m_pBigTime = NULL;
	m_iThumbPos = 0;
//	m_bIsGraphRunning = false;
	m_Log.SetAttributes(true,true);
	m_iGraphCondition = NULL;
	m_bIsSeeking = false;
}

CSeekBarWnd::~CSeekBarWnd()
{
	if (m_hBitmapS)
        ::DeleteObject (m_hBitmapS);
	if (m_hBitmapTH)
        ::DeleteObject (m_hBitmapTH);
    m_hBitmapS = NULL;
	m_hBitmapTH = NULL;
}


BEGIN_MESSAGE_MAP(CSeekBarWnd, CStatic)
	//{{AFX_MSG_MAP(CSeekBarWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSeekBarWnd message handlers

void CSeekBarWnd::SetBitmap()
{
	HINSTANCE  hInstResource = NULL;
    
    // Find correct resource handle
    hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_BASESEEK), RT_BITMAP);
    
    // Load bitmap In
    m_hBitmapS = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(IDB_BASESEEK),
                                   IMAGE_BITMAP, 0, 0, 0);
	// Find correct resource handle
    hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_BASESEEKTHUMB), RT_BITMAP);
    
    // Load bitmap In
    m_hBitmapTH = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(IDB_BASESEEKTHUMB),
                                   IMAGE_BITMAP, 0, 0, 0);
}

BOOL CSeekBarWnd::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	//return CStatic::OnEraseBkgnd(pDC);
}

void CSeekBarWnd::DrawSeekBar()
{
	CClientDC dc(this);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CRect rect;
	GetClientRect(&rect);
	HBITMAP olBitmap = (HBITMAP)memDC.SelectObject(m_hBitmapS);
	dc.BitBlt(0,0,rect.Width(),rect.Height(),&memDC,0,0,SRCCOPY);
	memDC.SelectObject(olBitmap);
	
	olBitmap = (HBITMAP)memDC.SelectObject(m_hBitmapTH);
	
	REFERENCE_TIME rtNow;
	REFERENCE_TIME rtStop;
	if(SUCCEEDED(m_pDShowMP3->GetCurrentPosition2(&rtNow,&rtStop)))
	{
		m_rtNow=rtNow;
		float fMSPercentagePos=(float)rtNow/rtStop;
		CRect rect;
		GetClientRect(&rect);
		//int iPos = (rect.Width()- 22) * (fMSPercentagePos);
		int iPos = SEEK_SCROLL_START_AREA + (SEEK_SCROLL_WORK_AREA- SEEK_THUMB_WIDTH) * (fMSPercentagePos);
		m_iThumbPos = iPos;
		dc.BitBlt(iPos,0,rect.Width(),rect.Height(),&memDC,0,0,SRCCOPY);

	}
	else
	{
		//dc.BitBlt(0,0,rect.Width(),rect.Height(),&memDC,0,0,SRCCOPY);
		dc.BitBlt(SEEK_SCROLL_START_AREA,0,SEEK_SCROLL_WORK_AREA,SEEK_SCROLL_WINDOW_HEIGHT,&memDC,0,0,SRCCOPY);
		m_iThumbPos=0;
	}
	memDC.SelectObject(olBitmap);
	memDC.DeleteDC();

//	CString sNum;			
//	sNum.Format("%d",m_rtNow);			
//	m_Log.WriteData("Draw:m_rtNow :" + sNum);			
}

void CSeekBarWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	DrawSeekBar();
	// Do not call CStatic::OnPaint() for painting messages
}

void CSeekBarWnd::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(2);
	Invalidate();
	SetTimer(2,1000,NULL);
	CStatic::OnTimer(nIDEvent);
}

void CSeekBarWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRect rectThumb(m_iThumbPos,0,m_iThumbPos+SEEK_THUMB_WIDTH,SEEK_THUMB_HEIGHT);
	if(m_pDShowMP3->GetGraphState()==1 || m_pDShowMP3->GetGraphState()==2)
	{
		if(rectThumb.PtInRect(point))
		{
			m_pDShowMP3->m_bIsSeeking = true;
			SetCapture();
			m_bMouseDown = true;
			m_iMousePos = point.x;
			KillTimer(2);
			if(*m_iGraphCondition==2)
				m_pBigTime->Pause(true);
			//m_pDShowMP3->GraphPause(true);
			m_pDShowMP3->GraphStop();
			::SendMessage(m_hHwndMain,WM_BEGIN_SEEK,0,0);
		}
	}
	CStatic::OnLButtonDown(nFlags, point);
}

void CSeekBarWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bMouseDown)
	{
		ReleaseCapture();
		Invalidate();
		m_bMouseDown = false;
		SetTimer(2,1000,NULL);
		if(*m_iGraphCondition==2)
		{
		//	m_pDShowMP3->GraphPause(false);
			m_pDShowMP3->GraphRun();
			m_pBigTime->Pause(false);
		}
		if(*m_iGraphCondition==1)
		{
			m_pDShowMP3->GraphRun();
			m_pDShowMP3->GraphPause(true);
		}
		m_pDShowMP3->m_bIsSeeking = false;
//		m_pBigTime->SetTime(i);
		m_pBigTime->Invalidate();
	}
	CStatic::OnLButtonUp(nFlags, point);	
}

void CSeekBarWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bMouseDown)
	{	
		CRect rect;
		GetClientRect(&rect);
		int iSlide;
		if(rect.PtInRect(point) && point.x>=SEEK_SCROLL_START_AREA && point.x <=SEEK_SCROLL_END_AREA)
		{
			if(point.x>m_iMousePos)
			{
				iSlide = point.x-m_iMousePos;
				m_rtNow= m_rtNow+(iSlide*m_rtFactor);
			}
			else
			{
				iSlide = m_iMousePos-point.x;
				m_rtNow= m_rtNow-(iSlide*m_rtFactor);
			}
				 
			CRect rect;
			GetClientRect(&rect);
			if(m_rtNow>m_rtStop)
			{
				m_rtNow=m_rtStop;
			}
			if(m_rtNow<0)
			{
				m_rtNow=0;
			}
			m_pDShowMP3->SetCurrentPosition2(m_rtNow);
			m_pDShowMP3->m_bIsSeeked = true;
			m_iMousePos = point.x;
			UINT i = (UINT)(m_rtNow/10000000);
			m_pBigTime->SetTime(i);
		//	m_pBigTime->Invalidate();
			Invalidate();
		}
	}
	CStatic::OnMouseMove(nFlags, point);
}

void CSeekBarWnd::SetReferenceTime()
{
	if(m_pDShowMP3->GetGraphState()==1||m_pDShowMP3->GetGraphState()==2)
	{
		m_pDShowMP3->GetCurrentPosition(&m_rtNow,&m_rtStop);
		//CRect rect;
		//GetClientRect(&rect);
		//m_rtFactor= m_rtStop/(rect.Width()-22);
		m_rtFactor= m_rtStop/(SEEK_SCROLL_WORK_AREA-SEEK_THUMB_WIDTH);
	}
	else
	{
		m_rtNow=0;
		m_rtStop=0;
		m_rtFactor=0;
	}
	Invalidate();
}
