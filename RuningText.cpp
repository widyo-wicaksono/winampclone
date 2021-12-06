// RuningText.cpp : implementation file
//

#include "stdafx.h"
//#include "SkinList.h"
#include "RuningText.h"
#include "MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRuningText

CRuningText::CRuningText()
{
	m_pParrent=NULL;
	m_sBuffer="";
	CreateFont();
}

CRuningText::~CRuningText()
{
}


BEGIN_MESSAGE_MAP(CRuningText, CStatic)
	//{{AFX_MSG_MAP(CRuningText)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRuningText message handlers

void CRuningText::Start()
{
	int iTextLen = GetTextLength(m_sText);
	CRect rect;
	GetClientRect(&rect);
	CString s = m_sText;
	int i = 1;
	
	//if(rect.Width()<iTextLen)
	if(160<iTextLen)
	{
		//while(rect.Width()<iTextLen)
		while(160<iTextLen)
		{
			s=s.Left(s.GetLength()-i);
			iTextLen=GetTextLength(s);
			i++;
		}
		m_iCapacity=s.GetLength();
		//m_iCapacity=20;
		m_iCurTextIndex = 0;
		m_iDir=1;
		SetTimer(2,750,NULL);
	}
	else
	{
		//SetDCText(m_sText);
		m_sBuffer = m_sText;
		DrawBGText(m_sText);
	}
}

void CRuningText::Stop()
{
	KillTimer(2);
	DrawBg();
	//SetWindowText("");
	m_sBuffer="";
}

int CRuningText::GetTextLength(LPCTSTR sStr)
{
	CString s = sStr;
	int nExtent = 0;
	int nLen = s.GetLength();
	CSize size;

	if (nLen)
	{
		TEXTMETRIC tm;		
		CDC* pDC = GetDC();
		//CFont* pFontOld = pDC->SelectObject(this->GetFont()); 
		CFont* pFontOld = pDC->SelectObject(&m_cFont); 
		pDC->GetTextMetrics(&tm);
		size = pDC->GetTextExtent(s, nLen);
		nExtent = size.cx;
		
		pDC->SelectObject(pFontOld);
		ReleaseDC(pDC);
	}	
	return nExtent;
}

void CRuningText::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==2)
	{
		KillTimer(2);
		{		
			m_sBuffer = m_sText.Mid(m_iCurTextIndex,m_iCapacity);		
			DrawBGText(m_sBuffer);
			Invalidate();
			if(m_iDir==1)
			{
				m_iCurTextIndex++;
				if(m_iCurTextIndex+(m_iCapacity)>=m_sText.GetLength())
				{
					m_iDir=2;
				}
			}
			else
			{
				m_iCurTextIndex--;
				if(m_iCurTextIndex<=0)
				{
					m_iDir=1;
				}
			}

		}

		SetTimer(2,750,NULL);
	}
	CStatic::OnTimer(nIDEvent);
}

void CRuningText::DrawBg()
{
	CClientDC hpDC(this);
	CDC dcMem;
	dcMem.CreateCompatibleDC(&hpDC);
	CBitmap bmpTemp;
	CRect rect;
	GetClientRect(&rect);
	int iRet=bmpTemp.CreateCompatibleBitmap(&hpDC,170,rect.Height());
	CBitmap *pOldBitmap = (CBitmap*)dcMem.SelectObject(&bmpTemp);
	dcMem.FillSolidRect(0,0,170,rect.Height(),RGB(29,65,81));
	int iXPos=(rect.Width()/2)-(170/2);
	hpDC.BitBlt(iXPos,0,170,rect.Height(),&dcMem,0,0, SRCCOPY);
	dcMem.SelectObject(pOldBitmap);
	bmpTemp.DeleteObject();
	dcMem.DeleteDC(); 
}

BOOL CRuningText::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
//	MessageBox("BG");
	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}

void CRuningText::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	DrawBGText(m_sBuffer);
	//MessageBox("PAINT");
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
}


void CRuningText::CreateFont()
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 12;
	strcpy(lf.lfFaceName, "Verdana");
	m_cFont.CreateFontIndirect(&lf);
}

void CRuningText::SetDCText(LPCTSTR s)
{
	CString sBuffer = s;
	int i = GetTextLength(s);
	CClientDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	int iXPos=(rect.Width()/2)-(170/2);
			
	dc.SelectObject(m_cFont.m_hObject);
	dc.TextOut((iXPos+72)-(i/2),0,sBuffer);
}

void CRuningText::DrawBGText(LPCTSTR s)
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
	
	dcMem.FillSolidRect(0,0,170,rect.Height(),RGB(29,65,81));
	
	CFont *pOldFont=(CFont*)dcMem.SelectObject(&m_cFont);
	CRect dcrect;
	dcrect.left = 0;
	dcrect.right = 170;
	dcrect.top = 0;
	dcrect.bottom = rect.Height();
	dcMem.SetTextColor(RGB(255,255,255));
	dcMem.DrawText(str,&dcrect,DT_CENTER);
	dcMem.SelectObject(pOldFont);

	int iXPos=(rect.Width()/2)-(170/2);
	hpDC.BitBlt(iXPos,0,170,rect.Height(),&dcMem,0,0, SRCCOPY);
	
	dcMem.SelectObject(pOldBitmap);
	bmpTemp.DeleteObject();
	dcMem.DeleteDC(); 
}

void CRuningText::UpdateTextPosition()
{
	m_sBuffer = m_sText.Mid(m_iCurTextIndex,m_iCapacity);		
//	DrawBGText(m_sBuffer);
	Invalidate();
	if(m_iDir==1)
	{
		m_iCurTextIndex++;
		if(m_iCurTextIndex+(m_iCapacity)>=m_sText.GetLength())
		{
			m_iDir=2;
		}
	}
	else
	{
		m_iCurTextIndex--;
		if(m_iCurTextIndex<=0)
		{
			m_iDir=1;
		}
	}

}
