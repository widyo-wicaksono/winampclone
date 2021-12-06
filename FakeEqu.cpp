// FakeEqu.cpp : implementation file
//

#include "stdafx.h"
//#include "SkinList.h"
#include "FakeEqu.h"
#include "Mmsystem.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFakeEqu

CFakeEqu::CFakeEqu()
{
	m_bIsPaused=false;
}

CFakeEqu::~CFakeEqu()
{
}


BEGIN_MESSAGE_MAP(CFakeEqu, CStatic)
	//{{AFX_MSG_MAP(CFakeEqu)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFakeEqu message handlers

BOOL CFakeEqu::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;
	//return CStatic::OnEraseBkgnd(pDC);
}

void CFakeEqu::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	static bool bIsFirst=true;
	if(bIsFirst)
	{
		GetClientRect(&m_rect);
		m_bIsRun=false;
		bIsFirst=false;
	}
	if(!m_bIsRun)
	{		
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap bmpTemp;
		int iRet=bmpTemp.CreateCompatibleBitmap(&dc,m_rect.Width(),m_rect.Height());
		CBitmap *pOldBitmap = (CBitmap*)dcMem.SelectObject(&bmpTemp);
		//dcMem.FillSolidRect(0,m_rect.top,m_rect.Width(),m_rect.Height(),RGB(255,255,255));
		dcMem.FillSolidRect(0,m_rect.top,m_rect.Width(),m_rect.Height(),RGB(29,65,81));
		dc.BitBlt(m_rect.left,m_rect.top,m_rect.Width(),m_rect.Height(),&dcMem,0,0, SRCCOPY);
		dcMem.SelectObject(pOldBitmap);
		bmpTemp.DeleteObject();
		dcMem.DeleteDC();
	}
	else
	{
		DrawEqualizer();
	}
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
}

//DEL void CFakeEqu::DrawEqu(CPaintDC * pDC, CRect * pRect)
//DEL {
//DEL 	CClientDC hpDC(this);
//DEL 	CDC dcMem;
//DEL 	dcMem.CreateCompatibleDC(&hpDC);
//DEL 	static UINT uLastCall=0;
//DEL 	if((timeGetTime() - uLastCall)>25)
//DEL 	{	
//DEL 		srand(timeGetTime());
//DEL 		for(int i=0;i<HISTOGRAM_COUNT;i++)
//DEL 		{
//DEL 			if(m_iLastHeight[i]>0)
//DEL 				m_iLastHeight[i]=m_iLastHeight[i]-8;
//DEL 			else
//DEL 				m_iLastHeight[i]=rand()%m_rect.Height();
//DEL 		}
//DEL 		uLastCall=timeGetTime();
//DEL 	}
//DEL 
//DEL 	CBitmap bmpTemp;
//DEL 	int iRet=bmpTemp.CreateCompatibleBitmap(&hpDC,m_rect.Width(),m_rect.Height());
//DEL 	CBitmap *pOldBitmap = (CBitmap*)dcMem.SelectObject(&bmpTemp);
//DEL 
//DEL 	int r2=48,g2=48,b2=48; //Any start color
//DEL 	int r1=255,g1=255,b1=255; //Any stop col
//DEL 	//dcMem.FillSolidRect(0,m_rect.top,m_rect.Width(),m_rect.Height(),RGB(255,255,255));
//DEL 	dcMem.FillSolidRect(0,m_rect.top,m_rect.Width(),m_rect.Height(),RGB(29,65,81));
//DEL 	
//DEL 	int iLeftSource =0;
//DEL 	for(int j=0;j<HISTOGRAM_COUNT;j++)
//DEL 	{
//DEL 		for(int i=0;i<m_rect.Height()-m_iLastHeight[j];i++)
//DEL 		{ 
//DEL 			int r,g,b;
//DEL 			r = r1 + (i * (r2-r1) / (m_rect.Height()-m_iLastHeight[j]));
//DEL 			g = g1 + (i * (g2-g1) / (m_rect.Height()-m_iLastHeight[j]));
//DEL 			b = b1 + (i * (b2-b1) / (m_rect.Height()-m_iLastHeight[j]));
//DEL 			dcMem.FillSolidRect(iLeftSource,m_rect.Height()-i,m_iBarWidth,1,RGB(r,g,b));
//DEL 		}
//DEL 		iLeftSource = iLeftSource+m_iBarWidth+m_iBarSpace;
//DEL 	}
//DEL 	iRet = hpDC.BitBlt(m_rect.left,m_rect.top,m_rect.Width(),m_rect.Height(),&dcMem,0,0, SRCCOPY);
//DEL 	dcMem.SelectObject(pOldBitmap);
//DEL 	bmpTemp.DeleteObject();
//DEL 	dcMem.DeleteDC();
//DEL }

void CFakeEqu::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==2)
	{
		KillTimer(2);
		UpdateTime();		
		Invalidate();
		SetTimer(2,26,NULL);
	}
	if(nIDEvent==3)
	{
		KillTimer(3);
		
		int iIsStopped =0;
		if(m_sStoped.Find("0",0)!=-1)
		{
			iIsStopped ++;
		}
		if(m_sStoped.Find("1",0)!=-1)
		{
			iIsStopped ++;
		}
		if(m_sStoped.Find("2",0)!=-1)
		{
			iIsStopped ++;
		}
		if(m_sStoped.Find("3",0)!=-1)
		{
			iIsStopped ++;
		}
		if(m_sStoped.Find("4",0)!=-1)
		{
			iIsStopped ++;
		}

		if(iIsStopped<5)
		{
			StopEqu();
			SetTimer(3,26,NULL);
		}
		else
		{
			m_bIsRun=false;
			if(!m_bIsPaused)
				::PostMessage(m_hParrentWnd ,WM_STOP_ANIM_FINISHED,NULL,NULL);
			Invalidate();
		}
		//StopEqu();
	}
	CStatic::OnTimer(nIDEvent);
}

void CFakeEqu::Start()
{
	m_sStoped ="";
	
	InitData();
	KillTimer(2);
	KillTimer(3);
//	SetTimer(2,26,NULL);
	m_bIsRun=true;
}

void CFakeEqu::Stop()
{
	KillTimer(2);
	KillTimer(3);
	SetTimer(3,26,NULL);
}

void CFakeEqu::StopEqu()
{
	CClientDC hpDC(this);
	CDC dcMem;
	dcMem.CreateCompatibleDC(&hpDC);
	static UINT uLastCall=0;
	if((timeGetTime() - uLastCall)>25)
	{	
		srand(timeGetTime());
		for(int i=0;i<HISTOGRAM_COUNT;i++)
		{
			if(m_iLastHeight[i]<m_rect.Height())
				m_iLastHeight[i]=m_iLastHeight[i]+8;
			else
			{
				CString s;
				s.Format("%d",i);
				if(m_sStoped.Find(s,0)==-1)
				{
					m_sStoped=m_sStoped+s;
				}
			}
		}
		uLastCall=timeGetTime();
	}

	CBitmap bmpTemp;
	int iRet=bmpTemp.CreateCompatibleBitmap(&hpDC,m_rect.Width(),m_rect.Height());
	CBitmap *pOldBitmap = (CBitmap*)dcMem.SelectObject(&bmpTemp);

	int r2=48,g2=48,b2=48; //Any start color
	int r1=217,g1=217,b1=217; //Any stop col
	//dcMem.FillSolidRect(0,m_rect.top,m_rect.Width(),m_rect.Height(),RGB(255,255,255));
	dcMem.FillSolidRect(0,m_rect.top,m_rect.Width(),m_rect.Height(),RGB(29,65,81));
	
	int iLeftSource =0;
	for(int j=0;j<HISTOGRAM_COUNT;j++)
	{
		for(int i=0;i<m_rect.Height()-m_iLastHeight[j];i++)
		{ 
			int r,g,b;
			r = r1 + (i * (r2-r1) / (m_rect.Height()-m_iLastHeight[j]));
			g = g1 + (i * (g2-g1) / (m_rect.Height()-m_iLastHeight[j]));
			b = b1 + (i * (b2-b1) / (m_rect.Height()-m_iLastHeight[j]));
			dcMem.FillSolidRect(iLeftSource,m_rect.Height()-i,m_iBarWidth,1,RGB(r,g,b));
		}
		iLeftSource = iLeftSource+m_iBarWidth+m_iBarSpace;
	}
	iRet = hpDC.BitBlt(m_rect.left,m_rect.top,m_rect.Width(),m_rect.Height(),&dcMem,0,0, SRCCOPY);
	dcMem.SelectObject(pOldBitmap);
	bmpTemp.DeleteObject();
	dcMem.DeleteDC();
}

void CFakeEqu::InitData()
{

	for(int i=0;i<HISTOGRAM_COUNT;i++)
	{
		m_iLastHeight[i]=m_rect.Height();
//		m_iLastPost[i]=0;
	}
	
	m_iBarSpace=1;
	m_iBarWidth = (m_rect.Width()- m_iBarSpace*4)/HISTOGRAM_COUNT;
	m_bIsPaused=false;
}

void CFakeEqu::UpdateTime()
{
	static UINT uLastCall=0;
	if((timeGetTime() - uLastCall)>25)
	{	
		srand(timeGetTime());
		for(int i=0;i<5;i++)
		{
			if(m_iLastHeight[i]>0)
				m_iLastHeight[i]=m_iLastHeight[i]-8;
			else
				m_iLastHeight[i]=rand()%m_rect.Height();
		}
		uLastCall=timeGetTime();
	}
}

void CFakeEqu::DrawEqualizer()
{
	CClientDC hpDC(this);
	CDC dcMem;
	dcMem.CreateCompatibleDC(&hpDC);
	CBitmap bmpTemp;
	int iRet=bmpTemp.CreateCompatibleBitmap(&hpDC,m_rect.Width(),m_rect.Height());
	CBitmap *pOldBitmap = (CBitmap*)dcMem.SelectObject(&bmpTemp);

	int r2=255,g2=255,b2=255; //Any start color
	//int r1=29,g2=65,b2=81; //Any start color
	int r1=29,g1=65,b1=81; //Any stop col
	//dcMem.FillSolidRect(0,m_rect.top,m_rect.Width(),m_rect.Height(),RGB(255,255,255));
	dcMem.FillSolidRect(0,m_rect.top,m_rect.Width(),m_rect.Height(),RGB(29,65,81));
	
	int iLeftSource =0;
	for(int j=0;j<HISTOGRAM_COUNT;j++)
	{
		for(int i=0;i<m_rect.Height()-m_iLastHeight[j];i++)
		{ 
			int r,g,b;
			r = r1 + (i * (r2-r1) / (m_rect.Height()-m_iLastHeight[j]));
			g = g1 + (i * (g2-g1) / (m_rect.Height()-m_iLastHeight[j]));
			b = b1 + (i * (b2-b1) / (m_rect.Height()-m_iLastHeight[j]));
			dcMem.FillSolidRect(iLeftSource,m_rect.Height()-i,m_iBarWidth,1,RGB(r,g,b));
		}
		iLeftSource = iLeftSource+m_iBarWidth+m_iBarSpace;
	}
	iRet = hpDC.BitBlt(m_rect.left,m_rect.top,m_rect.Width(),m_rect.Height(),&dcMem,0,0, SRCCOPY);
	dcMem.SelectObject(pOldBitmap);
	bmpTemp.DeleteObject();
	dcMem.DeleteDC();
}

void CFakeEqu::UpdateEqu()
{
	UpdateTime();		
	Invalidate();
}
