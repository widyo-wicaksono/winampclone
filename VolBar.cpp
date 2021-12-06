// VolBar.cpp : implementation file
//

#include "stdafx.h"
#include "WcxPlayer.h"
#include "VolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVolBar

CVolBar::CVolBar()
{
	rectV1.left=0;
	rectV2.left=0;
	rectTH.left=0;
	rectV1.top=0;
	rectV2.top=0;
	rectTH.top=0;
	rectV1.right=60;
	rectV2.right=60;
	rectTH.right=14;
	rectV1.bottom=14;
	rectV2.bottom=14;
	rectTH.bottom=14;
	SetBitmap();
	m_iVolValue=0;
	m_bMouseDown=false;
	m_pDShowMP3 = NULL;
}

CVolBar::~CVolBar()
{
	if (m_hBitmapV1)
        ::DeleteObject (m_hBitmapV1);
	if (m_hBitmapV2)
        ::DeleteObject (m_hBitmapV2);
	if (m_hBitmapTH)
        ::DeleteObject (m_hBitmapTH);
    m_hBitmapV1 = NULL;
	m_hBitmapV2=NULL;
	m_hBitmapTH = NULL;
}


BEGIN_MESSAGE_MAP(CVolBar, CStatic)
	//{{AFX_MSG_MAP(CVolBar)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVolBar message handlers

void CVolBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
		DrawVolSlide();
	// Do not call CStatic::OnPaint() for painting messages
}

BOOL CVolBar::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	//return CStatic::OnEraseBkgnd(pDC);
}

void CVolBar::SetBitmap()
{
	HINSTANCE  hInstResource = NULL;
    /*
    // Find correct resource handle
    hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_VOL1), RT_BITMAP);
    
    // Load bitmap In
    m_hBitmapV1 = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(IDB_VOL1),
                                   IMAGE_BITMAP, 0, 0, 0);
	// Find correct resource handle
    hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_VOL1), RT_BITMAP);
    
    // Load bitmap In
    m_hBitmapV2 = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(IDB_VOL2),
                                   IMAGE_BITMAP, 0, 0, 0);
	// Find correct resource handle
    hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_VOLTHUMB), RT_BITMAP);
    
    // Load bitmap In
    m_hBitmapTH = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(IDB_VOLTHUMB),
                                   IMAGE_BITMAP, 0, 0, 0);

	*/
		// Find correct resource handle
    hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_SEEKVOL), RT_BITMAP);
    
    // Load bitmap In
    m_hBitmapV2 = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(IDB_SEEKVOL),
                                   IMAGE_BITMAP, 0, 0, 0);
	// Find correct resource handle
    hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_SEEKVOLTHUMB), RT_BITMAP);
    
    // Load bitmap In
    m_hBitmapTH = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(IDB_SEEKVOLTHUMB),
                                   IMAGE_BITMAP, 0, 0, 0);

}

void CVolBar::DrawVolSlide()
{
	m_iVolValue=(int)(m_pDShowMP3->GetCurrentVolume()+10000);
	CClientDC dc(this);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CRect rect;
	GetClientRect(&rect);
	//HBITMAP olBitmap = (HBITMAP)memDC.SelectObject(m_hBitmapV1);
	HBITMAP olBitmap = (HBITMAP)memDC.SelectObject(m_hBitmapV2);
	dc.BitBlt(0,0,rect.Width(),rect.Height(),&memDC,0,0,SRCCOPY);
	memDC.SelectObject(olBitmap);
	/*
//	olBitmap = (HBITMAP)memDC.SelectObject(m_hBitmapV2);

	float iPos;
	if(m_iVolValue>0)
		//iPos =7+ (1/(float)((float)10000/m_iVolValue))*(rect.Width()-14);
		iPos =THUMB_WIDTH_HALF+ (1/(float)((float)10000/m_iVolValue))*(SCROLL_WORK_AREA-THUMB_WIDTH);
	else
		iPos = THUMB_WIDTH_HALF;
		//iPos = 7;

	dc.BitBlt(0,0,(int)iPos,rect.Height(),&memDC,0,0,SRCCOPY);
	memDC.SelectObject(olBitmap);
*/

	float iPos;
	if(m_iVolValue>0)
		iPos =SCROLL_START_AREA+THUMB_WIDTH_HALF+ (1/(float)((float)10000/m_iVolValue))*(SCROLL_WORK_AREA-THUMB_WIDTH);
	else
		iPos = SCROLL_START_AREA+THUMB_WIDTH_HALF;

	olBitmap = (HBITMAP)memDC.SelectObject(m_hBitmapTH);
	//int x = (int)iPos - (rectTH.Width()/2);
	int x = (int)iPos - (THUMB_WIDTH_HALF);
	//if(x+rectTH.Width()>rect.Width())
	if(x+THUMB_WIDTH>SCROLL_END_AREA)
	{
		//x=x-(x+rectTH.Width()-rect.Width());
		x=x-(x+THUMB_WIDTH-SCROLL_END_AREA);
	}


	//dc.BitBlt(x,0,rectTH.Width(),rectTH.Height(),&memDC,0,0,SRCCOPY);
	dc.BitBlt(x,0,THUMB_WIDTH,THUMB_HEIGHT,&memDC,0,0,SRCCOPY);
	m_iThumbPos = x;

	memDC.SelectObject(olBitmap);
	memDC.DeleteDC();
}

void CVolBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	SetCapture();
	CRect rectThumb(m_iThumbPos,0,m_iThumbPos+THUMB_WIDTH,THUMB_HEIGHT);

	if(rectThumb.PtInRect(point))
	{
		m_bMouseDown = true;
		m_iMousePos = point.x;
	}
	CStatic::OnLButtonDown(nFlags, point);
}

void CVolBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	ReleaseCapture();
	Invalidate();
	m_bMouseDown = false;
	CStatic::OnLButtonUp(nFlags, point);
}

void CVolBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bMouseDown)
	{
		CRect rect;
		GetClientRect(&rect);
			
		if(rect.PtInRect(point) && point.x>=SCROLL_START_AREA && point.x <=SCROLL_END_AREA)
		{
			if(point.x>m_iMousePos)
			{
				int iSlide = point.x-m_iMousePos;
			//	CRect rect;
			//	GetClientRect(&rect);
				//float fPercentageMovement = ((float)iSlide/(rect.Width()-THUMB_WIDTH));
				float fPercentageMovement = ((float)iSlide/(SCROLL_WORK_AREA-THUMB_WIDTH));
				float fDeltaVol= 10000*fPercentageMovement;
				m_iVolValue = m_iVolValue + (int)fDeltaVol;
				m_iMousePos = point.x;
				if(m_iVolValue >10000)
					m_iVolValue =10000;
				m_pDShowMP3->SetCurrentVolume((long)(m_iVolValue-10000));
				Invalidate();
			}
			else if(point.x<m_iMousePos)
			{
				int iSlide = m_iMousePos-point.x;
			//	CRect rect;
			//	GetClientRect(&rect);
			//	float fPercentageMovement = ((float)iSlide/(rect.Width()-THUMB_WIDTH));
				float fPercentageMovement = ((float)iSlide/(SCROLL_WORK_AREA-THUMB_WIDTH));
				float fDeltaVol= 10000*fPercentageMovement;
				m_iVolValue = m_iVolValue - (int)fDeltaVol;
				m_iMousePos = point.x;
				if(m_iVolValue <0)
					m_iVolValue =0;
				m_pDShowMP3->SetCurrentVolume((long)(m_iVolValue-10000));
				Invalidate();
			}
		}
	}
	CStatic::OnMouseMove(nFlags, point);
}
