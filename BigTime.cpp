// BigTime.cpp : implementation file
//

#include "stdafx.h"
#include "WcxPlayer.h"
#include "BigTime.h"
#include "MemDC.h"
#include "Mmsystem.h"
#include "streams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBigTime

CBigTime::CBigTime()
{
	m_uTime=0;
	m_sTime="00:00";
//	CreateFont();
	m_uTimeForNextUpdate=1000;
}

CBigTime::~CBigTime()
{
}


BEGIN_MESSAGE_MAP(CBigTime, CStatic)
	//{{AFX_MSG_MAP(CBigTime)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBigTime message handlers

void CBigTime::Start()
{
	ShowWindow(SW_SHOW);
	SetTimer(2,1000,NULL);
}

void CBigTime::Stop()
{
	ShowWindow(SW_HIDE);
	KillTimer(2);
	m_uTime=0;
	
	m_sTime = "00:00";
	DrawBGText(m_sTime );
}

void CBigTime::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==2)
	{
		m_uTime++;
		KillTimer(2);
		m_sTime.Format(_T("%.2d:%.2d"), m_uTime / 60, m_uTime % 60);
		
		Invalidate();
		SetTimer(2,1000,NULL);
		m_uLastTimerCalled = timeGetTime();
	}
	CStatic::OnTimer(nIDEvent);
}

//DEL void CBigTime::DrawBG()
//DEL {
//DEL 	CClientDC hpDC(this);
//DEL 	//CDC dcMem;
//DEL 	CRect rect;
//DEL 	GetClientRect(&rect);
//DEL 
//DEL 	CMemDC memDC(&hpDC, &rect);
//DEL 	memDC.FillSolidRect(0,0,rect.Width(),rect.Height(),RGB(255,255,255));
//DEL /*
//DEL 	dcMem.CreateCompatibleDC(&hpDC);
//DEL 	CBitmap bmpTemp;
//DEL 	
//DEL 	
//DEL 	int iRet=bmpTemp.CreateCompatibleBitmap(&hpDC,144,rect.Height());
//DEL 	CBitmap *pOldBitmap = (CBitmap*)dcMem.SelectObject(&bmpTemp);
//DEL 	dcMem.FillSolidRect(0,0,rect.Width(),rect.Height(),RGB(255,255,255));
//DEL 
//DEL 	hpDC.BitBlt(0,0,rect.Width(),rect.Height(),&dcMem,0,0, SRCCOPY);
//DEL 	dcMem.SelectObject(pOldBitmap);
//DEL 	bmpTemp.DeleteObject();
//DEL 	dcMem.DeleteDC();
//DEL 	*/
//DEL }

void CBigTime::CreateFont()
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 46;
	strcpy(lf.lfFaceName, "Times New Roman");
	m_cFont.CreateFontIndirect(&lf);
}

void CBigTime::DrawBGText(LPCTSTR s)
{
	CString str = s;

	CClientDC hpDC(this);
	CDC dcMem;
	dcMem.CreateCompatibleDC(&hpDC);
	CBitmap bmpTemp;
	CRect rect;
	GetClientRect(&rect);
	
	int iRet=bmpTemp.CreateCompatibleBitmap(&hpDC,170,rect.Height());
	CBitmap *pOldBitmap = (CBitmap*)dcMem.SelectObject(&bmpTemp);
	dcMem.FillSolidRect(0,0,rect.Width(),rect.Height(),RGB(29,65,81));

	//CFont *olFont = (CFont*)dcMem.SelectObject(&m_cFont);
	//dcMem.DrawText(str,&rect,DT_CENTER);
	//dcMem.SelectObject(olFont);
	for(int i =0;i<5;i++)
	{
		CBitmap BMPNum;
		

		HINSTANCE  hInstResource = NULL;
		
		// Find correct resource handle
		CString sMat = str.Mid(i,1);
		UINT G_ID=GetNumGraphic(sMat);
		BMPNum.LoadBitmap(G_ID);
	//	hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(G_ID), RT_BITMAP);
	//	hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_BASESKIN4), RT_BITMAP);
		// Load bitmap In
	//	BMPNum.Attach((HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(G_ID),
				//					   IMAGE_BITMAP, 0, 0, 0));
    
		
		CDC dcNum;
		dcNum.CreateCompatibleDC(&hpDC);
		CBitmap *pOldBitmapNum = (CBitmap*)dcNum.SelectObject(&BMPNum);
		dcMem.BitBlt(i*30,0,30,37,&dcNum,0,0, SRCCOPY);
		dcNum.SelectObject(pOldBitmapNum);
		BMPNum.DeleteObject();
		dcNum.DeleteDC();
	}

	hpDC.BitBlt(0,0,rect.Width(),rect.Height(),&dcMem,0,0, SRCCOPY);
	dcMem.SelectObject(pOldBitmap);
	bmpTemp.DeleteObject();
	dcMem.DeleteDC(); 
}

BOOL CBigTime::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	//return CStatic::OnEraseBkgnd(pDC);
}

void CBigTime::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	DrawBGText(m_sTime);
	// Do not call CStatic::OnPaint() for painting messages
}

void CBigTime::Pause(bool bPause)
{
	if(bPause)
	{
		m_uPausedTime = timeGetTime();
		KillTimer(2);
	}

	else
	{
		UINT uDelta = m_uPausedTime-m_uLastTimerCalled;
		if(uDelta>1000)
			SetTimer(2,1000,NULL);
		else
		{
			UINT uTimeForNextUpdate = 1000 - (m_uPausedTime-m_uLastTimerCalled);

	//	CString s;
	//	s.Format("Next :%d, m_uPausedTime : %d,  m_uLastTimerCalled : %d",uTimeForNextUpdate,m_uPausedTime,m_uLastTimerCalled);
	//	DbgOutString(s +"\n");
			SetTimer(2,uTimeForNextUpdate,NULL);
		}
	}
}

void CBigTime::SetTime(UINT uTime)
{
	m_uTime = uTime;
	m_sTime.Format(_T("%.2d:%.2d"), m_uTime / 60, m_uTime % 60);
}

//DEL UINT CBigTime::UpdateTime()
//DEL {
//DEL 	m_uTime++;
//DEL 	m_sTime.Format(_T("%.2d:%.2d"), m_uTime / 60, m_uTime % 60);
//DEL 	
//DEL 	Invalidate();
//DEL 	m_uLastTimerCalled = timeGetTime();
//DEL 	return 0;
//DEL 	//return m_uTimeForNextUpdate;
//DEL }

UINT CBigTime::GetNumGraphic(LPCTSTR str)
{
	CString s=str;
	if(s=="0")
		return IDB_BITMAP0;
	else if(s=="1")
		return IDB_BITMAP1;
	else if(s=="2")
		return IDB_BITMAP2;
	else if(s=="3")
		return IDB_BITMAP3;
	else if(s=="4")
		return IDB_BITMAP4;
	else if(s=="5")
		return IDB_BITMAP5;
	else if(s=="6")
		return IDB_BITMAP6;
	else if(s=="7")
		return IDB_BITMAP7;
	else if(s=="8")
		return IDB_BITMAP8;
	else if(s=="9")
		return IDB_BITMAP9;
	else if(s==":")
		return IDB_SEMICOLON;
	else 
		return 0;
}
