// InfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WcxPlayer.h"
#include "InfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg dialog


CInfoDlg::CInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetBitmap();
	m_brushHollow = (HBRUSH) GetStockObject (HOLLOW_BRUSH);
}


void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CInfoDlg)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg message handlers

void CInfoDlg::OnOK() 
{
	// TODO: Add extra validation here
	this->DestroyWindow();
	::PostMessage(m_hWndParrent,WM_CHILD_EXIT,NULL,NULL);
//	CDialog::OnOK();
}

void CInfoDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	//CDialog::OnCancel();
}

HRESULT CInfoDlg::GetMP3Tag(MP3ID3V1TAG *sTag, LPCTSTR sPath)
{
	HANDLE hFileTag;
	CString sPathFile = sPath;
	if ((hFileTag = CreateFile(sPathFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL))
	!= INVALID_HANDLE_VALUE)
	{
		DWORD dwNumBytesRead;
		if (SetFilePointer(hFileTag,-128,NULL,FILE_END) != INVALID_SET_FILE_POINTER &&
			ReadFile(hFileTag,sTag,128,&dwNumBytesRead,NULL) &&
			dwNumBytesRead == 128 && memcmp(sTag->ident,"TAG",3) == 0)
		{
			CloseHandle(hFileTag);
			return S_OK;
			
		}
		CloseHandle(hFileTag);
	}
	return E_FAIL;
}

BOOL CInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	long lBitRate;
	long lSample;
	CString sChannel;
	WMp3 * pWmp3 = new WMp3;
	if(pWmp3->OpenMp3File(m_sPath,2000))
	{
		lBitRate = pWmp3->GetBitrate();
		sChannel = pWmp3->GetChannelMode();
		lSample=pWmp3->GetSamplingRate();
	}
	CString s;
	pWmp3->Close();
	delete pWmp3;
	GetDlgItem(IDC_PATH)->SetWindowText(m_sPath);
	s.Format("%d",lSample);
	GetDlgItem(IDC_SAMPLE)->SetWindowText("Sample Rate : " + s);
	s.Format("%d",lBitRate);
	GetDlgItem(IDC_BITRATE)->SetWindowText("Bit Rate : " + s);
	GetDlgItem(IDC_CHANNEL)->SetWindowText("Channel : " + sChannel);

	MP3ID3V1TAG mp3Tag;
	GetMP3Tag(&mp3Tag,m_sPath);
	s="Title : "; 
	s=s+ mp3Tag.title;
	
	CRect rect;
	GetDlgItem(IDC_TITLE)->GetClientRect(&rect);
	bool bIsTextTooLong=true;
	
	int h = s.Find("\n",0);
	if(h!=-1)
	{
		s=s.Left(h);
	}
	int i=0;
	while(bIsTextTooLong)
	{
		i=GetTextLength(s,GetDlgItem(IDC_TITLE));
		if(i>rect.Width())
		{
			s=s.Left(s.GetLength()-1);
		}
		else
		{
			bIsTextTooLong=false;
			break;
		}
	}
	GetDlgItem(IDC_TITLE)->SetWindowText(s);
	s="Artist : ";
	s=s + mp3Tag.artist;
	h = s.Find("\n",0);
	if(h!=-1)
	{
		s=s.Left(h);
	}
	bIsTextTooLong=true;
	while(bIsTextTooLong)
	{
		i=GetTextLength(s,GetDlgItem(IDC_ARTIST));
		if(i>rect.Width())
		{
			s=s.Left(s.GetLength()-1);
		}
		else
		{
			bIsTextTooLong=false;
			break;
		}
	}
	GetDlgItem(IDC_ARTIST)->SetWindowText(s);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CInfoDlg::OnEraseBkgnd(CDC* pDC) 
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
//	return CDialog::OnEraseBkgnd(pDC);
}

void CInfoDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if (m_hBitmap)
		::DeleteObject (m_hBitmap);

    m_hBitmap = NULL;
}

void CInfoDlg::SetBitmap()
{
	HINSTANCE  hInstResource = NULL;
    
    // Find correct resource handle
    hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_MP3INFO), RT_BITMAP);
    
    // Load bitmap In
    m_hBitmap = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(IDB_MP3INFO),
                                   IMAGE_BITMAP, 0, 0, 0);
}

HBRUSH CInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here

	// Make static controls transparent
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		// Make sure that it's not a slider control
		char lpszClassName[256];
		GetClassName (pWnd->m_hWnd, lpszClassName, 255);
		if (strcmp (lpszClassName, TRACKBAR_CLASS) == 0)
			return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

		pDC->SetBkMode (TRANSPARENT);
		return m_brushHollow;
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CInfoDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CDialog::OnLButtonDown(nFlags, point);
}

int CInfoDlg::GetTextLength(LPCTSTR sStr, CWnd * pWnd)
{
	CString s = sStr;
	int nExtent = 0;
	int nLen = s.GetLength();
	CSize size;

	if (nLen)
	{
		TEXTMETRIC tm;		
		CDC* pDC = GetDC();
		CFont* pFontOld = pDC->SelectObject(pWnd->GetFont()); 
		//CFont* pFontOld = pDC->SelectObject(&m_cFont); 
		pDC->GetTextMetrics(&tm);
		size = pDC->GetTextExtent(s, nLen);
		nExtent = size.cx;
		
		pDC->SelectObject(pFontOld);
		//pDC->DeleteDC();
		ReleaseDC(pDC);
	}	
	return nExtent;
}
