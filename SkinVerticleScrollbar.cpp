// SkinVerticleScrollbar.cpp : implementation file
//

#include "stdafx.h"
//#include "SkinList.h"
#include "WcxPlayer.h"
#include "SkinVerticleScrollbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
double g_bSCMovePos=-100;
double g_bSCUpdatePos=-100;
int g_iThumbPos=-100;
*/

#define SCROLLBAR_THUMB_HEIGHT 13
#define SCROLLBAR_THUMB_MID 6
#define SCROLLBAR_THUMB_WIDTH 8
/////////////////////////////////////////////////////////////////////////////
// CSkinVerticleScrollbar

CSkinVerticleScrollbar::CSkinVerticleScrollbar()
{
	bMouseDown = false;
	bMouseDownArrowUp = false;
	bMouseDownArrowDown = false;
	bDragging = false;

	nThumbTop = 0;
	//nThumbTop = 36;
	dbThumbInterval = 0.000000;
	pList = NULL;

}

CSkinVerticleScrollbar::~CSkinVerticleScrollbar()
{
}


BEGIN_MESSAGE_MAP(CSkinVerticleScrollbar, CStatic)
	//{{AFX_MSG_MAP(CSkinVerticleScrollbar)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinVerticleScrollbar message handlers

BOOL CSkinVerticleScrollbar::OnEraseBkgnd(CDC* pDC) 
{
	return CStatic::OnEraseBkgnd(pDC);
}

void CSkinVerticleScrollbar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	CRect clientRect;
	GetClientRect(&clientRect);

	int nHeight = clientRect.Height() - 37;
	

	//CRect rectUpArrow(0,11,12,37);
	//CRect rectDownArrow(0,nHeight,12,nHeight+26);
	//CRect rectThumb(0,nThumbTop,12,nThumbTop+26);
	CRect rectThumb(0,nThumbTop,SCROLLBAR_THUMB_WIDTH,nThumbTop+SCROLLBAR_THUMB_HEIGHT);

	if(rectThumb.PtInRect(point))
	{
		bMouseDown = true;
	}
/*
	if(rectDownArrow.PtInRect(point))
	{
		bMouseDownArrowDown = true;
		SetTimer(2,250,NULL);
	}

	if(rectUpArrow.PtInRect(point))
	{
		bMouseDownArrowUp = true;
		SetTimer(2,250,NULL);
	}
	*/
	CStatic::OnLButtonDown(nFlags, point);
}

void CSkinVerticleScrollbar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	UpdateThumbPosition();
	KillTimer(1);
	ReleaseCapture();
	/*
	bool bInChannel = true;

	CRect clientRect;
	GetClientRect(&clientRect);
	int nHeight = clientRect.Height() - 37;
	CRect rectUpArrow(0,11,12,37);
	CRect rectDownArrow(0,nHeight,12,nHeight+26);
	CRect rectThumb(0,nThumbTop,12,nThumbTop+26);



	if(rectUpArrow.PtInRect(point) && bMouseDownArrowUp)
	{
		ScrollUp();	
		bInChannel = false;
	}

	if(rectDownArrow.PtInRect(point) && bMouseDownArrowDown)
	{
		ScrollDown();
		bInChannel = false;
	}

	if(rectThumb.PtInRect(point))
	{
		bInChannel = false;
	}

	if(bInChannel == true && !bMouseDown)
	{
		if(point.y > nThumbTop)
		{
			PageDown();
		}
		else
		{
			PageUp();
		}
	}
*/
	bMouseDown = false;
	bDragging = false;
	bMouseDownArrowUp = false;
	bMouseDownArrowDown = false;
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CSkinVerticleScrollbar::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect clientRect;
	GetClientRect(&clientRect);
//	static int iPos=-10;
/*	static int iPosMove = -10;	
	int iTmpi = iPosMove;
	if(nThumbTop<iPosMove)
	{
		int y=0;
	}

	iPosMove=nThumbTop;	*/
	if(bMouseDown)
	{
		
		nThumbTop = point.y-SCROLLBAR_THUMB_MID; //-13 so mouse is in middle of thumb

		double nMax = pList->GetScrollLimit(SB_VERT);
		
		int nPos = pList->GetScrollPos(SB_VERT);
/*
		if(nPos>0)
			int y=0;
		if(nThumbTop==33)
			int y =0;
		if(nPos<g_bSCMovePos)
		{
			int y=0;
		}
		g_bSCMovePos=nPos;
*/

		//double nHeight = clientRect.Height()-98;
		double nHeight = clientRect.Height()-SCROLLBAR_THUMB_HEIGHT;
		double nVar = nMax;
		dbThumbInterval = nHeight/nVar;

		//figure out how many times to scroll total from top
		//then minus the current position from it
		//int nScrollTimes = (int)((nThumbTop-36)/dbThumbInterval)-nPos;
		int nScrollTimes = (int)((nThumbTop)/dbThumbInterval)-nPos;

		//grab the row height dynamically
		//so if the font size or type changes
		//our scroll will still work properly
		CRect itemrect;
		pList->GetItemRect(0,&itemrect, LVIR_BOUNDS);

		CSize size;
		size.cx = 0;
		size.cy = nScrollTimes*itemrect.Height();
		

		pList->Scroll(size);
		LimitThumbPosition();

		Draw();
		
	}
	CStatic::OnMouseMove(nFlags, point);
}

void CSkinVerticleScrollbar::OnPaint() 
{
	CPaintDC dc(this); 
	
	Draw();
}

void CSkinVerticleScrollbar::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 1)
	{
		if(bMouseDownArrowDown)
		{
			ScrollDown();
		}
		
		if(bMouseDownArrowUp)
		{
			ScrollUp();
		}
	}
	else if(nIDEvent == 2)
	{
		if(bMouseDownArrowDown)
		{
			KillTimer(2);
			SetTimer(1, 50, NULL);
		}
		
		if(bMouseDownArrowUp)
		{
			KillTimer(2);
			SetTimer(1, 50, NULL);
		}
	}
	CStatic::OnTimer(nIDEvent);
}

void CSkinVerticleScrollbar::PageDown()
{
	pList->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEDOWN,0),NULL);
	UpdateThumbPosition();
}

void CSkinVerticleScrollbar::PageUp()
{
	pList->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEUP,0),NULL);
	UpdateThumbPosition();
}

void CSkinVerticleScrollbar::ScrollUp()
{
	pList->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP,0),NULL);
	UpdateThumbPosition();
}

void CSkinVerticleScrollbar::ScrollDown()
{
	pList->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN,0),NULL);
	UpdateThumbPosition();
}

void CSkinVerticleScrollbar::UpdateThumbPosition()
{
	CRect clientRect;
	GetClientRect(&clientRect);

	double nPos = pList->GetScrollPos(SB_VERT);
	/*
	if(nPos>0)
			int y=0;

		if(nPos<g_bSCUpdatePos)
		{
			int y=0;
		}
		g_bSCUpdatePos=nPos;
*/
	double nMax = pList->GetScrollLimit(SB_VERT);
	//double nHeight = (clientRect.Height()-98);
	double nHeight = (clientRect.Height()-SCROLLBAR_THUMB_HEIGHT);
	double nVar = nMax;

	dbThumbInterval = nHeight/nVar;

	double nNewdbValue = (dbThumbInterval * nPos);
	int nNewValue = (int)nNewdbValue;


	//nThumbTop = 36+nNewValue;
	nThumbTop = nNewValue;

	static int iPosi = 0;	
	int iTmpi = iPosi;
	if(nThumbTop<iPosi)
	{
		int y=0;
	}
	iPosi=nThumbTop;
	LimitThumbPosition();

	Draw();
}


void CSkinVerticleScrollbar::Draw()
{

	CClientDC dc(this);
	CRect clientRect;
	GetClientRect(&clientRect);
	CCustomMemDC memDC(&dc, &clientRect);
	memDC.FillSolidRect(&clientRect,  RGB(74,82,107));
	CDC bitmapDC;
	bitmapDC.CreateCompatibleDC(&dc);

	CBitmap bitmap;
	CBitmap* pOldBitmap;

	bitmap.LoadBitmap(IDB_VERTICLE_SCROLLBAR_SPAN2);
	pOldBitmap = bitmapDC.SelectObject(&bitmap);
	int nHeight = clientRect.Height();

	memDC.StretchBlt(clientRect.left, clientRect.top, SCROLLBAR_THUMB_WIDTH,nHeight+SCROLLBAR_THUMB_HEIGHT,&bitmapDC, 0,0, SCROLLBAR_THUMB_WIDTH, 1, SRCCOPY);

	bitmapDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	pOldBitmap = NULL;

	//If there is nothing to scroll then don't
	//show the thumb control otherwise show it
	/*
	static int iPosDraw = 0;	
	int iTmpi = iPosDraw;
	if(nThumbTop<iPosDraw)
	{
		int x,z;
		pList->GetScrollRange(SB_VERT,&x,&z);
		//int y=0;
	}
	iPosDraw=nThumbTop;
	*/
	if(pList->GetScrollLimit(SB_VERT) != 0)
	{
		//draw the thumb control
		bitmap.LoadBitmap(IDB_VERTICLE_SCROLLBAR_THUMB);
		pOldBitmap = bitmapDC.SelectObject(&bitmap);

		memDC.BitBlt(clientRect.left,clientRect.top+nThumbTop,SCROLLBAR_THUMB_WIDTH,SCROLLBAR_THUMB_HEIGHT,&bitmapDC,0,0,SRCCOPY);
		bitmapDC.SelectObject(pOldBitmap);
		bitmap.DeleteObject();
		pOldBitmap = NULL;
	}


}

void CSkinVerticleScrollbar::LimitThumbPosition()
{
	CRect clientRect;
	GetClientRect(&clientRect);

	//if(nThumbTop+26 > (clientRect.Height()-37))
	if(nThumbTop+SCROLLBAR_THUMB_HEIGHT > (clientRect.Height()))
	{
		nThumbTop = clientRect.Height()-SCROLLBAR_THUMB_HEIGHT;
		//nThumbTop = clientRect.Height()-37;
	}

	//if(nThumbTop < (clientRect.top+36))
	if(nThumbTop < (clientRect.top))
	{
		nThumbTop = clientRect.top;
	}
}

