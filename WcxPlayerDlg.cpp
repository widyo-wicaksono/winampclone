// WcxPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WcxPlayer.h"
#include "WcxPlayerDlg.h"
#include "comutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
/*
UINT g_uPrevBigTime=0;
UINT g_uPrevRunningText=0;
UINT g_uBigTimeDelta=1000;

UINT g_uPrevEquUpdate=0;
UINT g_uPrevIdle=0;
*/
UINT g_uPrevT3Call=0;
UINT g_uLastPaused=0;
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWcxPlayerDlg dialog

CWcxPlayerDlg::CWcxPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWcxPlayerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWcxPlayerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pWmp3 = new WMp3;
	m_bForcedStop = false;
	m_pInfoDlg = NULL;
	m_pAboutDlg =NULL;
	m_hBitmap = NULL;
	m_pHelDlg = NULL;
//	m_pAdvDlg = NULL;
	SetBitmap();
	m_pSetLayeredWindowAttributes = NULL;
	m_iCurSelectionIndex=-1;
	m_IsHide = false;
	m_iGrState = 0;
	m_hHeap=HeapCreate(0,0,0);
	m_uSampleBufferTimeStamp=0;
	m_BufferManager.m_hHeap = m_hHeap;

	pFFTrans=NULL;
    pFFTransStereo=NULL;
    pSampleIter=NULL; 
	m_pLeftBuff =NULL;
	m_pFFTThread = NULL;

	m_uLastHistogramUpdate = timeGetTime();
}

void CWcxPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWcxPlayerDlg)
	DDX_Control(pDX, IDC_TIME, m_cBigTime);
	DDX_Control(pDX, IDC_RUNTEXT, m_cRunText);
	DDX_Control(pDX, IDC_FAKE_EQU, m_cFake);
	DDX_Control(pDX, IDC_PLAYLIST, m_cList);
	DDX_Control(pDX, IDC_BUTTON_PAUSE, m_cBtnPause);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_cBtnStop);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_cBtnPlay);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWcxPlayerDlg, CDialog)
	//{{AFX_MSG_MAP(CWcxPlayerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_NOTIFY(NM_RCLICK, IDC_PLAYLIST, OnRclickPlaylist)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, OnButtonPause)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(IDC_DELETE, OnRemoveFromList)
	ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphMsg)
	ON_MESSAGE(WM_STOP_ANIM_FINISHED, OnStopAnimFinished)
	ON_MESSAGE(WM_ABOUT_EXIT, OnAboutExit)
	ON_MESSAGE(WM_ADV_EXIT, OnAdvExit)
	ON_MESSAGE(WM_SAMPLEISREADY, OnSampleIsReady)
	ON_MESSAGE(WM_MEDIA_PLAY, RunMedia)
	ON_MESSAGE(WM_BEGIN_SEEK,OnBeginSeek)
	ON_COMMAND(IDC_GETINFO,OnMP3Info)
	ON_COMMAND(IDC_ABOUT,OnShowAbout)
	ON_COMMAND(IDC_EXIT,OnExit)
	ON_COMMAND(IDC_ADV,OnAdvShow)
	ON_COMMAND(IDC_HELPD,OnShowHelp)
	ON_COMMAND(IDC_REMALL,RemoveAllPlayListItems)
	ON_COMMAND(IDC_MINIMIZETRAY,MinimizeToTray)
	ON_COMMAND(IDC_RESTOREAPP,OnRestore)
	ON_COMMAND(IDC_EXITMINIMIZE,OnExit)
	ON_MESSAGE(WM_MY_TRAYMESSAGE,HandleTrayMessage)
	ON_MESSAGE(WM_CHILD_EXIT, OnChildExit)
	ON_MESSAGE(WM_HELP_EXIT, OnHelpExit)

	//ON_THREAD_MESSAGE(WM_UPDATE_HISTOGRAM, OnUpdateHistogram)
	ON_MESSAGE(WM_UPDATE_HISTOGRAM, OnUpdateHistogram)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWcxPlayerDlg message handlers

BOOL CWcxPlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_DShow.m_WndMain = this; 

	HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
	m_pSetLayeredWindowAttributes = 
                       (lpfnSetLayeredWindowAttributes)GetProcAddress(hUser32, 
                       "SetLayeredWindowAttributes");
	if(!m_pSetLayeredWindowAttributes)
	{
		MessageBox("Only for 2000/XP or greater","Error");
		PostQuitMessage(1);
		return TRUE;
	}
	else
	{
		SetWindowLong (m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) |WS_EX_LAYERED/*|WS_EX_TOOLWINDOW*/ );
		//m_pSetLayeredWindowAttributes(m_hWnd, RGB(255,0,0), 0, LWA_COLORKEY);
		m_pSetLayeredWindowAttributes (m_hWnd, RGB(255,0,0), 0, LWA_COLORKEY|LWA_ALPHA);
	}
	/*
	m_pFFTThread = (CModTimer *)AfxBeginThread(RUNTIME_CLASS(CModTimer),
                                    THREAD_PRIORITY_NORMAL,
                                    0, // stack size
                                    CREATE_SUSPENDED);

	m_pFFTThread->m_hMainWnd=m_hWnd;
	m_pFFTThread->m_hHeap=m_hHeap;
	m_pFFTThread->m_psc=&m_cs;
	m_pFFTThread->ResumeThread();
*/
	m_DShow.m_hSampleHeap = m_hHeap;
	if(FAILED(m_DShow.CreateInterface()))
	{
		MessageBox("Failed to initialize DShow","Error");
		PostQuitMessage(1);
		return TRUE;
	}
	else if(FAILED(m_DShow.CreateDefaultMP3Interface()))
	{
		MessageBox("Failed to initialize DShow","Error");
		PostQuitMessage(1);
		return TRUE;
	}
	else if(FAILED(m_DShow.CreateHistogramEffectFilter()))
	{
		MessageBox("Failed to initialize DShow","Error");
		PostQuitMessage(1);
		return TRUE;
	}

	m_cBtnPlay.SetSkin(IDB_BTNPLAYNRM,IDB_BTNPLAYPRS,IDB_BTNPLAYFCS,IDB_BTNPLAYNRM,0,IDB_BTNMASK3,0,0,0);
	m_cBtnStop.SetSkin(IDB_BTNSTOPNRM,IDB_BTNSTOPPRS,IDB_BTNSTOPFCS,IDB_BTNSTOPNRM,0,IDB_BTNMASK3,0,0,0);
	m_cBtnPause.SetSkin(IDB_BTNPAUSENRM,IDB_BTNPAUSEPRS,IDB_BTNPAUSEFCS,IDB_BTNPAUSENRM,0,IDB_BTNMASK3,0,0,0);
	this->SetWindowText("WinAmpun ver 0.0.2");

	m_cList.Init();
	//m_cList.SetBkColor(RGB(134,30,0));
//	m_cList.SetBkColor(RGB(29,65,81));
	//m_cList.SetTextColor(RGB(248,121,16));
	m_cList.SetTextColor(RGB(255,255,255));

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 12;
	strcpy(lf.lfFaceName, "Verdana");
	font.CreateFontIndirect(&lf);
	m_cList.SetFont(&font, TRUE);
	m_cList.SetRedraw(FALSE);
	
	m_cList.InsertColumn ( 0, _T("Title"), LVCFMT_LEFT, 430);
    m_cList.InsertColumn ( 1, _T("Duration"), LVCFMT_RIGHT,50, 1);

	m_cList.SetRedraw(TRUE);
	ListView_SetExtendedListViewStyle(m_cList.m_hWnd, LVS_EX_FULLROWSELECT  | LVS_EX_HEADERDRAGDROP);

	m_cFake.m_hParrentWnd = this->m_hWnd;

	m_hAccel = LoadAccelerators ( AfxGetResourceHandle(),
                                  MAKEINTRESOURCE(IDR_ACCMAIN) );
	
	m_cVolBar.m_pDShowMP3 = &(this->m_DShow);
	m_cVolBar.Create(NULL, WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE|WS_GROUP,CRect(0,0,56,14), this);
	m_cVolBar.SetWindowPos(NULL,208,371,228,11,SWP_NOZORDER);
	
	m_cSeekBar.m_pDShowMP3 = &m_DShow;
	m_cSeekBar.m_pBigTime = &m_cBigTime;
	m_cSeekBar.m_iGraphCondition = &m_iGrState;
	m_cSeekBar.Create(NULL, WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE|WS_GROUP,CRect(0,0,160,10), this);
	m_cSeekBar.SetWindowPos(NULL,164,315,293,11,SWP_NOZORDER);
//	MoveWindow(m_rect.left,m_rect.top,196,64);
	m_cSeekBar.m_hHwndMain = m_hWnd;

//	m_cSeekBar.SetReferenceTime();
//	m_cSeekBar.SetTimer(2,1000,NULL);

	SetTimer(2,10,NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWcxPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWcxPlayerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWcxPlayerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

HRESULT CWcxPlayerDlg::GetMP3Tag(MP3ID3V1TAG *sTag, LPCTSTR sPath)
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

void CWcxPlayerDlg::PopulateList(LPCTSTR sPass)
{
	CString sPath = sPass;
	HANDLE hFile;
	WIN32_FIND_DATA FindFileData;

	hFile=FindFirstFile(sPath, &FindFileData);
	if(hFile!=INVALID_HANDLE_VALUE)
	{
		CString s = FindFileData.cFileName;
		if(FindFileData.dwFileAttributes !=FILE_ATTRIBUTE_DIRECTORY && s.Right(3)=="mp3"  )
		{
			if(m_pWmp3->OpenMp3File(sPath,2000))
			{
				DWORD dwLengthSec=m_pWmp3->GetSongLength();
				
				CString strFileInfo;
				strFileInfo.Format(_T("%.2d:%.2d"), dwLengthSec / 60, dwLengthSec % 60);
				MP3ID3V1TAG sID3V1;
				MP3Data stData;
				CString sTittle;
				if(SUCCEEDED(GetMP3Tag(&sID3V1,sPath)))
				{
					char strTemp[31]; strTemp[30] = 0; // make a temporary null-terminated buffer
					memcpy(strTemp,sID3V1.title,30); 
					sTittle= strTemp;
				}

				sTittle.TrimRight();
				sTittle.TrimLeft();
				if(sTittle!="")
				{
					m_cList.InsertItem(m_cList.GetItemCount(),sTittle);
				}
				else
					m_cList.InsertItem(m_cList.GetItemCount(),FindFileData.cFileName);
				strcpy(stData.sPath,sPath);
				strcpy(stData.sTitle,sTittle);
				strcpy(stData.sFileName,s);
				m_arrMP3.Add(stData);
				
				m_cList.SetItemText(m_cList.GetItemCount()-1, 1, strFileInfo);
				
			}
			m_pWmp3->Close();			
		}
		else
		{
			hFile=FindFirstFile(sPath+ "\\*.*", &FindFileData);
			while(true)
			{
				if(FindNextFile(hFile,&FindFileData))
				{	
					s = FindFileData.cFileName;
					if(FindFileData.dwFileAttributes !=FILE_ATTRIBUTE_DIRECTORY && s.Right(3)=="mp3")
					{		
						if(m_pWmp3->OpenMp3File(sPath + "\\" + FindFileData.cFileName,2000))
						{
							DWORD dwLengthSec=m_pWmp3->GetSongLength();
							
							CString strFileInfo;
							strFileInfo.Format(_T("%.2d:%.2d"), dwLengthSec / 60, dwLengthSec % 60);
							MP3ID3V1TAG sID3V1;
							MP3Data stData;
							CString sTittle;
							if(SUCCEEDED(GetMP3Tag(&sID3V1,sPath + "\\" + FindFileData.cFileName)))
							{
								char strTemp[31]; strTemp[30] = 0; // make a temporary null-terminated buffer
								memcpy(strTemp,sID3V1.title,30); 
								sTittle= strTemp;
							}

							sTittle.TrimRight();
							sTittle.TrimLeft();
							if(sTittle!="")
							{
								m_cList.InsertItem(m_cList.GetItemCount(),sTittle);
							}
							else
								m_cList.InsertItem(m_cList.GetItemCount(),FindFileData.cFileName);
							strcpy(stData.sPath,sPath + "\\" + FindFileData.cFileName);
							strcpy(stData.sTitle,sTittle);
							strcpy(stData.sFileName,s);
							m_arrMP3.Add(stData);
							
							m_cList.SetItemText(m_cList.GetItemCount()-1, 1, strFileInfo);							
						}
						m_pWmp3->Close();						
					}
					else
					{
						CString sTmp = FindFileData.cFileName;
						if(sTmp!="..")
						{
							PopulateList(sPath+"\\"+FindFileData.cFileName);
						}						
					}
				}
				else
					break;
			}
		}		
		FindClose(hFile);
	}	
	
}

void CWcxPlayerDlg::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	UINT        uNumFiles;
	TCHAR       szNextFile [MAX_PATH];
	LVFINDINFO  rlvFind = { LVFI_STRING, szNextFile };
	int         nIndex = m_cList.GetItemCount();
    // Get the # of files being dropped.

    uNumFiles = DragQueryFile ( hDropInfo, (UINT)-1, NULL, 0 );
	m_cList.SetRedraw(FALSE);
    for ( UINT uFile = 0; uFile < uNumFiles; uFile++ )
        {
        // Get the next filename from the HDROP info.
        if ( DragQueryFile ( hDropInfo, uFile, szNextFile, MAX_PATH ) > 0 )
            {
            // If the filename is already in the list, skip it.
            if ( -1 != m_cList.FindItem ( &rlvFind, -1 ))
                continue;
			PopulateList(szNextFile);
            }
        }   // end for
	m_cList.SetRedraw(TRUE);
	m_cList.ForceVThumbPaint();
    DragFinish ( hDropInfo );
//	CDialog::OnDropFiles(hDropInfo);
}

void CWcxPlayerDlg::OnButtonPlay() 
{
	// TODO: Add your control notification handler code here
	int i = GetCurrentSelectionIndex();
	if(i==-1)
		return;

	m_iGrState=2;
	m_bForcedStop = false;
	m_iCurSelectionIndex = i;
	m_cRunText.m_sText = m_arrMP3[i].sFileName;

	if(strcmp(m_arrMP3[i].sTitle,""))
		m_cRunText.m_sText = m_cRunText.m_sText+"-" + m_arrMP3[i].sTitle;
	m_cRunText.m_pParrent = this;
	
	m_cRunText.Stop();
	m_cRunText.Start();

	m_cBigTime.Stop();
	m_cBigTime.Start();
	m_cFake.Start();
	
	if(m_IsHide)
	{
		if(m_iCurSelectionIndex!=-1)
		{
			_tcscpy(m_tnd.szTip, m_arrMP3[m_iCurSelectionIndex].sFileName  );
			Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
		}
	}
	
	CString s = m_arrMP3[i].sPath;
//	_bstr_t sPath(s);
	//m_DShow.m_bMediaPath = s.AllocSysString();
	m_DShow.m_bMediaPath = s;

	m_DShow.GraphStop();
	m_DShow.RemoveFilter();

	m_DShow.m_bIsSeeked = false;
	if(m_pFFTThread)
		StopHistogramThread();

	if(FAILED(m_DShow.BuildGraphManually()))
	{
		m_cFake.Stop();
		return;
	}
	
/*	
	m_cs.Lock();
	m_pFFTThread->m_iSampleRate = m_DShow.GetCurrentSampleRate();
	m_pFFTThread->m_iChannels = m_DShow.GetChannelCount();
	m_pFFTThread->m_iBit = m_DShow.GetCurrentSampleSize();
	m_pFFTThread->m_iBytePerSecond = m_DShow.GetCurrentAvgByteperSecond();
	//m_pFFTThread->m_bIsPlaying=true;
	m_cs.Unlock();

	m_pFFTThread->PostThreadMessage(WM_MEDIA_PLAY,0,0);
	*/
	PostMessage(WM_MEDIA_PLAY,0,0);
	UpdateYahooMSN(m_arrMP3[i].sFileName);
	/*
	m_DShow.GraphRun();
	if(m_pAdvDlg)
	{
		m_pAdvDlg->m_cSeekBar.SetReferenceTime();
	}
	*/
	
/*
	if(pFFTrans)
	{
		delete pFFTrans;pFFTrans=NULL;
		delete pFFTransStereo;pFFTransStereo=NULL;
		delete pSampleIter;pSampleIter=NULL;
	}

	pFFTrans          = new FFTransform (m_DShow.GetCurrentSampleRate(), SAMPLE_BUFFER/(2 * (16/8)));
    pFFTransStereo    = new FFTransform (m_DShow.GetCurrentSampleRate(), SAMPLE_BUFFER/(2 * (16/8)));
    pSampleIter       = new SampleIter();
*/
	//SetTimer(4,100,NULL);
	//UINT uDur = GetDurationOf1SampleUnit();
	//SetTimer(4,HISTOGRAM_PROCESS_INTERVAL,NULL);
}

void CWcxPlayerDlg::OnButtonStop() 
{
	// TODO: Add your control notification handler code here
	int iState = m_DShow.GetGraphState();
	if(iState==1 || iState==2)
	{
		m_bForcedStop = true;
		m_DShow.GraphStop();
		m_DShow.RemoveFilter();
			
		m_cFake.Stop();
		m_cRunText.Stop();
		m_cBigTime.Stop();
		m_iGrState= 0;
		StopHistogramThread();
		m_cSeekBar.KillTimer(2);
		m_cSeekBar.Invalidate();
	}	
}

int CWcxPlayerDlg::GetCurrentSelectionIndex()
{
	int nSelItem;
/*
    for ( nSelItem = m_cList.GetNextItem ( -1, LVNI_SELECTED );
          nSelItem != -1;
          nSelItem = m_cList.GetNextItem ( nSelItem, LVNI_SELECTED ) )
        {
        //c_FileList.DeleteItem ( nSelItem );
        nSelItem--;
        }
		*/
	nSelItem = m_cList.GetNextItem ( -1, LVNI_SELECTED );
	return nSelItem;
}

void CWcxPlayerDlg::OnRclickPlaylist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint ptCursorPos;
	CMenu  menu;

    // We'll show the menu at the current cursor position.

    GetCursorPos ( &ptCursorPos );

    VERIFY ( menu.LoadMenu ( IDR_LISTMENU ));
    ASSERT ( NULL != menu.GetSubMenu(0) );  // check that there's a popup menu at index 0.

    // Show the menu!

    menu.GetSubMenu(0)->TrackPopupMenu ( TPM_RIGHTBUTTON, ptCursorPos.x,
                                         ptCursorPos.y, this );
	*pResult = 0;
}

void CWcxPlayerDlg::OnRemoveFromList()
{
	int i = GetCurrentSelectionIndex();
	if(i==-1)
		return;
	m_cList.DeleteItem(i);
	m_iCurSelectionIndex =-1;
	m_arrMP3.RemoveAt(i);	
}

void CWcxPlayerDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	/*
	m_arrMP3.RemoveAll();
	if(m_pWmp3)
	{
		delete m_pWmp3;
		m_pWmp3 = NULL;
	}
	*/
	CDialog::OnClose();
}

void CWcxPlayerDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	// TODO: Add your message handler code here	

	if(m_pFFTThread)
		StopHistogramThread();
	m_arrMP3.RemoveAll();
	/*
	if(m_pAdvDlg)
	{
		m_pAdvDlg->DestroyWindow();
		delete(m_pAdvDlg);
	}
	*/
	if(m_pWmp3)
		delete m_pWmp3;
	if(m_pAboutDlg)
	{
		m_pAboutDlg->DestroyWindow();
		delete(m_pAboutDlg);
	}
	if(m_pInfoDlg)
	{
		m_pInfoDlg->DestroyWindow();
		delete(m_pInfoDlg);
	}
	if(m_pHelDlg)
	{
		m_pHelDlg->DestroyWindow();
		delete(m_pHelDlg);
	}

	if(m_DShow.GetGraphState()==1 || m_DShow.GetGraphState()==2)
	{
		m_DShow.GraphStop();
		m_DShow.RemoveFilter();
		m_cSeekBar.KillTimer(2);
	}
	m_DShow.ReleaseMP3Interface();
	m_DShow.ReleaseHistogramFilter();
	m_DShow.ReleaseInterface();
	if (m_hBitmap)
        ::DeleteObject (m_hBitmap);
    m_hBitmap = NULL;
	Shell_NotifyIcon(NIM_DELETE,&m_tnd);
	HeapDestroy(m_hHeap);	
	m_cVolBar.DestroyWindow();
	m_cSeekBar.DestroyWindow();
}

LRESULT CWcxPlayerDlg::OnGraphMsg(WPARAM wParam, LPARAM lParam)
{
	int i = m_DShow.TranslateEvent();
	if(i==1)
	{
		m_cFake.Stop();
		m_cRunText.Stop();
		m_cBigTime.Stop();
		StopHistogramThread();
		m_cSeekBar.KillTimer(2);
	}
	return 1;
}

LRESULT CWcxPlayerDlg::OnStopAnimFinished(WPARAM wParam, LPARAM lParam)
{
//	m_bIsPlaying=false;
	if(!m_bForcedStop)
	{
		int i =  m_cList.GetNextItem ( -1, LVNI_SELECTED );
		if(i!=-1)
		{
			if(i<m_cList.GetItemCount()-1)
			{
				m_cList.SetItemState(i+1, LVIS_SELECTED, LVIS_SELECTED);
				m_iCurSelectionIndex = i+1;
			}
			else
			{
				m_cList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
				m_iCurSelectionIndex = 0;
			}
		}
		else
		{
			if(m_iCurSelectionIndex<m_cList.GetItemCount()-1)
			{
				m_cList.SetItemState(m_iCurSelectionIndex, LVIS_SELECTED, LVIS_SELECTED);
				m_iCurSelectionIndex++;
			}
			else
			{
				m_cList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
				m_iCurSelectionIndex = 0;
			}
		
		}
		m_cList.SetRedraw(FALSE);
		BOOL h = m_cList.EnsureVisible(m_iCurSelectionIndex,FALSE);
		m_cList.SetRedraw(TRUE);
		m_cList.ForceVThumbPaint();
		OnButtonPlay();
	}
	else
	{
		//OnButtonPlay();
	}
	return 1;
}

void CWcxPlayerDlg::OnButtonPause() 
{
	// TODO: Add your control notification handler code here
	int iState = m_DShow.GetGraphState();
	if(iState==1 || iState==2)
	{
		if(iState==2)
		{
			m_DShow.GraphPause(true);
			m_cBigTime.Pause(true);
			m_iGrState =1;
			m_cFake.m_bIsPaused= true;
			m_cFake.Stop();
		}
		else if(iState==1)
		{
			m_iGrState=2;
			//m_DShow.GraphRun();
			m_DShow.GraphPause(false);
			m_cBigTime.Pause(false);
			m_cFake.Start();
		}
	}
	/*
	CFileLog log;
	CString s="PAUSE";
	log.SetAttributes(true,true);
	log.WriteData(s);	
	*/
}

void CWcxPlayerDlg::OnMP3Info()
{
	if(m_pInfoDlg)
	{
		m_pInfoDlg->DestroyWindow();
		delete m_pInfoDlg;
		m_pInfoDlg=NULL;
	}
	int i = GetCurrentSelectionIndex();
	if(i==-1)
		return ;
	m_pInfoDlg = new CInfoDlg();
	m_pInfoDlg->m_hWndParrent = this->m_hWnd;
	m_pInfoDlg->m_sPath = m_arrMP3[i].sPath;
	m_pInfoDlg->Create(IDD_INFO,this);
	m_pInfoDlg->ShowWindow(SW_SHOW);
}

LRESULT CWcxPlayerDlg::OnChildExit(WPARAM wParam, LPARAM lParam)
{
	delete m_pInfoDlg;
	m_pInfoDlg=NULL;
	return 1;
}

void CWcxPlayerDlg::OnShowAbout()
{
	if(m_pAboutDlg)
	{
		m_pAboutDlg->DestroyWindow();
		delete m_pAboutDlg;
		m_pAboutDlg=NULL;
	}
	m_pAboutDlg = new CWinAmpAboutDlg();
	m_pAboutDlg->m_hWndParrent = this->m_hWnd;
	m_pAboutDlg->Create(IDD_ABOUTBOX2,this);
	m_pAboutDlg->ShowWindow(SW_SHOW);
}

LRESULT CWcxPlayerDlg::OnAboutExit(WPARAM wParam, LPARAM lParam)
{
	delete m_pAboutDlg;
	m_pAboutDlg=NULL;
	return 1;
}

BOOL CWcxPlayerDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if ( NULL != m_hAccel && 
         pMsg->message >= WM_KEYFIRST  &&  pMsg->message <= WM_KEYLAST )
        {
        return TranslateAccelerator ( m_hWnd, m_hAccel, pMsg );
        }
    else
        return CDialog::PreTranslateMessage(pMsg);
	//return CDialog::PreTranslateMessage(pMsg);
}

void CWcxPlayerDlg::OnExit()
{
	/*
	m_cs.Lock();
	m_pFFTThread->m_bIsPlaying=false;
	m_cs.Unlock();
	m_pFFTThread->PostThreadMessage(WM_MEDIA_STOP, 0, 0);
	m_pFFTThread->PostThreadMessage(WM_QUIT, 0, 0);
	WaitForSingleObject(m_pFFTThread->m_hThread,INFINITE);
	*/
	//if(m_pFFTThread)
	//	StopHistogramThread();
	PostQuitMessage(0);
}

HRESULT CWcxPlayerDlg::SetBitmap()
{
	HINSTANCE  hInstResource = NULL;
    
    // Find correct resource handle
    hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_BASESKIN4), RT_BITMAP);
    
    // Load bitmap In
    m_hBitmap = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(IDB_BASESKIN4),
                                   IMAGE_BITMAP, 0, 0, 0);
    
    if(m_hBitmap)
		return S_OK;
	return E_FAIL;
}

BOOL CWcxPlayerDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	GetClientRect(&rect);

	CDC dc;
	dc.CreateCompatibleDC(pDC);
	HBITMAP olBitmap = (HBITMAP)dc.SelectObject(m_hBitmap);

	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);
	
	dc.SelectObject(olBitmap);
	dc.DeleteDC();
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

void CWcxPlayerDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CDialog::OnLButtonDown(nFlags, point);
}

void CWcxPlayerDlg::OnTimer(UINT nIDEvent) 
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
	}
	/*
	else if(3==nIDEvent)
	{
		KillTimer(3);
		UpdateHistogram();
		SetTimer(4,100,NULL);
	}
	else if(4==nIDEvent)
	{
		KillTimer(4);
		if(m_DShow.GetGraphState()==2)
		{			
			g_uPrevT3Call= timeGetTime();
			//m_uSampleBufferTimeStamp=m_uSampleBufferTimeStamp+100;
			//m_uSampleBufferTimeStamp=m_uSampleBufferTimeStamp+HISTOGRAM_PROCESS_INTERVAL;
			//UpdateHistogram();
					
		}
		//SetTimer(4,100,NULL);
	}
	*/
	CDialog::OnTimer(nIDEvent);
}

void CWcxPlayerDlg::OnShowHelp()
{
	if(m_pHelDlg)
	{
		m_pHelDlg->DestroyWindow();
		delete m_pHelDlg;
		m_pHelDlg=NULL;
	}
	m_pHelDlg=new CHelpDlg();
	m_pHelDlg->m_hWndParrent = m_hWnd;
	m_pHelDlg->m_pSetLayeredWindowAttributes = m_pSetLayeredWindowAttributes;
	m_pHelDlg->Create(IDD_HELPDLG,this);
	m_pHelDlg->ShowWindow(SW_SHOW);
}

//void CWcxPlayerDlg::OnHelpExit()
LRESULT CWcxPlayerDlg::OnHelpExit(WPARAM wParam, LPARAM lParam)
{
	delete m_pHelDlg;
	m_pHelDlg=NULL;
	return 1;
}

void CWcxPlayerDlg::OnAdvShow()
{	
	/*
	if(m_pAdvDlg)
	{
		m_pAdvDlg->DestroyWindow();
		delete m_pAdvDlg;
		m_pAdvDlg=NULL;
	}
	m_pAdvDlg=new CAdvDlg();
	CRect rect;
	GetWindowRect(&rect);
	m_pAdvDlg->m_rect =rect; 
	m_pAdvDlg->m_hWndParrent = m_hWnd;
	m_pAdvDlg->m_cSeekBar.m_iGraphCondition = &m_iGrState;
	m_pAdvDlg->m_cVolBar.m_pDShowMP3 = &m_DShow;
	m_pAdvDlg->m_cSeekBar.m_pDShowMP3 = &m_DShow;
	m_pAdvDlg->m_cSeekBar.m_pBigTime = &m_cBigTime;
	m_pAdvDlg->m_pSetLayeredWindowAttributes = m_pSetLayeredWindowAttributes;
	m_pAdvDlg->Create(IDD_ADV,this);
	m_pAdvDlg->ShowWindow(SW_SHOW);
	*/
}

LRESULT CWcxPlayerDlg::OnAdvExit(WPARAM wParam, LPARAM lParam)
{
	/*
	delete m_pAdvDlg;
	m_pAdvDlg=NULL;
	*/
	return 1;
}

void CWcxPlayerDlg::RemoveAllPlayListItems()
{
	int i = GetCurrentSelectionIndex();
	if(i==-1)
		return;
	m_cList.DeleteAllItems();
	m_iCurSelectionIndex =-1;
	m_arrMP3.RemoveAll();	
	m_cList.ForceVThumbPaint();
}

void CWcxPlayerDlg::MinimizeToTray()
{
	ZeroMemory(&m_tnd,sizeof(m_tnd));
	m_tnd.cbSize = sizeof(NOTIFYICONDATA);
    m_tnd.hWnd   = this->m_hWnd;
    m_tnd.uID    = 0;
    m_tnd.hIcon  = AfxGetApp()->LoadIcon(IDR_MAINFRAME);;
    m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_tnd.uCallbackMessage = WM_MY_TRAYMESSAGE;
	
	if(m_iCurSelectionIndex!=-1)
	{
		_tcscpy(m_tnd.szTip, m_arrMP3[m_iCurSelectionIndex].sFileName  );
	}
	if(Shell_NotifyIcon(NIM_ADD, &m_tnd))
	{
		//this->ShowWindow(SW_MINIMIZE);
		this->ShowWindow(SW_HIDE);
		/*
		WINDOWPLACEMENT wPlacement;
		ZeroMemory(&wPlacement,sizeof(wPlacement));
		this->GetWindowPlacement(&wPlacement);
		wPlacement.showCmd=SW_HIDE;
		this->SetWindowPlacement(&wPlacement);
		*/

	}
	m_IsHide = true;
}

LRESULT CWcxPlayerDlg::HandleTrayMessage(WPARAM wParam, LPARAM lParam)
{
	UINT uID; 
	UINT uMsg; 

	uID = (UINT) wParam;
	uMsg = (UINT) lParam; 
	CPoint pt;
	switch (uMsg ) 
	{ 
		case WM_LBUTTONDOWN:
			//RestoreWindow();
			break;
		case WM_RBUTTONDOWN:
		case WM_CONTEXTMENU:
			GetCursorPos(&pt);
			//HandleTrayMenu(pt);
			{	
				CMenu menu;
				menu.LoadMenu(IDR_MINMENU);
				menu.GetSubMenu(0)->TrackPopupMenu(TPM_BOTTOMALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,pt.x,pt.y,this);
			}
			break;
	}
	return FALSE;
}

void CWcxPlayerDlg::OnRestore()
{
	ShowWindow(SW_RESTORE);
	Shell_NotifyIcon(NIM_DELETE, &m_tnd);
	m_IsHide = false;
}

//DEL LRESULT CWcxPlayerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
//DEL {
//DEL 	// TODO: Add your specialized code here and/or call the base class
//DEL 	DWORD QueueStatus;
//DEL 	LRESULT resValue;
//DEL 	bool OnIdleRetVal = true;
//DEL 	static UINT uCount=0;
//DEL 	if(message == WM_IDLE) {
//DEL 		OnIdleRetVal = OnIdle((UINT)wParam);
//DEL 		if(!OnIdleRetVal)
//DEL 			wParam = 0;
//DEL 	} else
//DEL 		resValue = CDialog::WindowProc(message, wParam, lParam);
//DEL 
//DEL 	QueueStatus = GetQueueStatus(QS_ALLINPUT);
//DEL 
//DEL 	UINT uNow=timeGetTime();
//DEL 	if(HIWORD(QueueStatus) == 0)
//DEL 	{
//DEL 		//if(uNow-g_uPrevIdle>=1)
//DEL 		{
//DEL 			g_uPrevIdle = uNow;
//DEL 			PostMessage(WM_IDLE, wParam + (OnIdleRetVal ? 1 : 0), 0);
//DEL 		}
//DEL 	} 
//DEL 	
//DEL 	return resValue;
//DEL 	//return CDialog::WindowProc(message, wParam, lParam);
//DEL }

//DEL bool CWcxPlayerDlg::OnIdle(LONG lCount)
//DEL {
//DEL 	static UINT uIdleLastCall=timeGetTime();
//DEL 	uIdleLastCall=timeGetTime();
//DEL 	if(m_iGrState==2)
//DEL 	{
//DEL 		if(uIdleLastCall-g_uPrevBigTime>=g_uBigTimeDelta)
//DEL 		{
//DEL 			g_uPrevBigTime = uIdleLastCall;
//DEL 			g_uBigTimeDelta=1000;
//DEL 			m_cBigTime.UpdateTime();
//DEL 		}
//DEL 		
//DEL 		if(uIdleLastCall-g_uPrevEquUpdate>=26)
//DEL 		{
//DEL 			g_uPrevEquUpdate=uIdleLastCall;
//DEL 			m_cFake.UpdateEqu();
//DEL 		}
//DEL 		
//DEL 	}
//DEL 	if(m_iGrState==1||m_iGrState==2)
//DEL 	{
//DEL 		if(uIdleLastCall-g_uPrevRunningText>=750)
//DEL 		{
//DEL 			g_uPrevRunningText=uIdleLastCall;
//DEL 			m_cRunText.UpdateTextPosition();
//DEL 		}
//DEL 	}
//DEL 	return true;
//DEL 
//DEL }

LRESULT CWcxPlayerDlg::OnSampleIsReady(WPARAM wParam, LPARAM lParam)
{	
	if(m_pFFTThread)
	{
		if(m_DShow.GetGraphState()==0 || m_DShow.GetGraphState()==1 || m_DShow.m_bIsSeeking)
		{		
			//HeapFree(m_hHeap,0,pBuff);
			if(m_pLeftBuff)
			{
				HeapFree(m_hHeap,0,m_pLeftBuff);
				NULL;
			}
			HeapFree(m_hHeap,0,(LPVOID)wParam);
			HeapFree(m_hHeap,0,(LPVOID)lParam);
			return 1;
		}

		if(m_DShow.m_bIsSeeked)
		{
			LONGLONG llPos1;
			LONGLONG llPos2;
			m_DShow.GetCurrentPosition(&llPos1,&llPos2);
			SampleInfo *pSI = (SampleInfo *)wParam;			
			LPVOID pSample=HeapAlloc(m_hHeap,HEAP_ZERO_MEMORY,sizeof(AudioSampleMemory));
			AudioSampleMemory *pSM = (AudioSampleMemory *)pSample;
							
			pSM->iSize = HeapSize(m_hHeap,0,pSample);
			if(pSI->lStartTime>=0)
				pSM->st = pSI->lStartTime;
			if(pSI->lFinishTime>=0)
				pSM->ft = pSI->lFinishTime;
			
			pSM->llCurrentMediaPos=llPos1;
			pSM->uLastMediaRunTime = m_DShow.m_uStartTimeAfterSeek;
			pSM->uCurrentSystemTime = timeGetTime();
			pSM->pActualStartAddress = (LPVOID)lParam;	
			pSM->llMediaPosWhenGraphRun = m_DShow.m_llPosAfterSeek;
			HeapFree(m_hHeap,0,(LPVOID)wParam);

			m_pFFTThread->PostThreadMessage(WM_SAMPLESEEKEDISREADY_2, 0, (LPARAM)pSM);			
			return 1;
		}

		LONGLONG llPos1;
		LONGLONG llPos2;
		m_DShow.GetCurrentPosition(&llPos1,&llPos2);
		SampleInfo *pSI = (SampleInfo *)wParam;
		/*
		if(pSI->lFinishTime<llPos1)
		{
			HeapFree(m_hHeap,0,(LPVOID)wParam);
			HeapFree(m_hHeap,0,(LPVOID)lParam);
			return 1;
		}
		*/
		//int iSize=HeapSize(m_hHeap,0,(LPVOID)lParam);
		LPVOID pSample=HeapAlloc(m_hHeap,HEAP_ZERO_MEMORY,sizeof(AudioSampleMemory));
		AudioSampleMemory *pSM = (AudioSampleMemory *)pSample;
		
		pSM->iSize = HeapSize(m_hHeap,0,pSample);
		if(pSI->lStartTime>=0)
			pSM->st = pSI->lStartTime;
		if(pSI->lFinishTime>=0)
			pSM->ft = pSI->lFinishTime;

		pSM->llCurrentMediaPos=llPos1;
		pSM->uCurrentSystemTime = timeGetTime();
		pSM->pActualStartAddress = (LPVOID)lParam;
		pSM->uLastMediaRunTime = m_DShow.m_uStartTimeAfterSeek;;
		HeapFree(m_hHeap,0,(LPVOID)wParam);
		m_pFFTThread->PostThreadMessage(WM_SAMPLEISREADY, 0, (LPARAM)pSM);
	}
	else
	{
		HeapFree(m_hHeap,0,(LPVOID)wParam);
		HeapFree(m_hHeap,0,(LPVOID)lParam);
	}
	return 1;
}

//DEL void CWcxPlayerDlg::UpdateHistogram()
//DEL {
//DEL 	CFileLog log;
//DEL 	log.SetAttributes(true,true);
//DEL 	UINT i=GenerateBufferIndex();
//DEL 	static UINT uIndex=0;
//DEL 	LPVOID pFFTData=m_BufferManager.ReadSampleBuffer(uIndex);
//DEL 	UINT j=uIndex;
//DEL 	uIndex = uIndex+SAMPLE_BUFFER;
//DEL 	
//DEL 	m_BufferManager.FreeUsedSampleBuffer();
//DEL 
//DEL 	CString s,h;
//DEL 	s.Format("UpdateHistogram Processed %d " ,*(PBYTE)pFFTData);
//DEL 	h.Format("Virtual Addrs %d ", j);
//DEL 	s=h+s;
//DEL 	log.WriteData(s);
//DEL 	
//DEL 	UpdateHistogramControl(pFFTData);
//DEL 	HeapFree(m_hHeap,0,pFFTData);
//DEL 	//SetTimer(4,100,NULL);	
//DEL 	//SetTimer(4,HISTOGRAM_PROCESS_INTERVAL,NULL);	
//DEL 	SetTimer(4,1,NULL);
//DEL }

DWORD CWcxPlayerDlg::GenerateBufferIndex()
{
	DWORD dwSampleRate = m_DShow.GetCurrentSampleRate();
	UINT uByteCount = m_DShow.GetCurrentSampleSize()/8;
	float iAvgSampleperMiliSecond = (float)dwSampleRate/1000;
	
	UINT uIndex = (UINT)((float)iAvgSampleperMiliSecond*(m_uSampleBufferTimeStamp-HISTOGRAM_PROCESS_INTERVAL));
	UINT r1 = uIndex*uByteCount;
	
	UINT uTmp=m_DShow.GetCurrentAvgByteperSecond();
	UINT uTemp = m_DShow.GetCurrentAvgByteperSecond()/1000*(m_uSampleBufferTimeStamp-HISTOGRAM_PROCESS_INTERVAL);
	UINT uBaseVirtual = uTemp/SAMPLE_BUFFER;
	UINT r2 = uBaseVirtual*SAMPLE_BUFFER;
	
	if(r1>0)
		int i=0;

	return r2;
}

//DEL void CWcxPlayerDlg::UpdateHistogramControl(LPVOID pBuffer)
//DEL {
//DEL 
//DEL 	LPSTR pData = (LPSTR)pBuffer;
//DEL 	int rps=1;
//DEL 	CRect rect;
//DEL 	GetDlgItem(IDC_FAKE_EQU)->GetClientRect(&rect);
//DEL 	pSampleIter->SetSampleWorkSet (pData, SAMPLE_BUFFER,  2, true);
//DEL   
//DEL 		static int channels[3][SPECSCOPEWIDTH] = { {0, 0, 0, 0, 0, },
//DEL                                                    {0, 0, 0, 0, 0, },
//DEL                                                    {0, 0, 0, 0, 0, }
//DEL                                                  };
//DEL 
//DEL         WorkChannel channelWork[2]              = {WRKCHANNEL_LEFT, WRKCHANNEL_RIGHT};
//DEL 
//DEL         int chan=0;
//DEL         int maxFreq;
//DEL         int freqChg;
//DEL         int freqAccum[SPECSCOPEWIDTH];
//DEL 
//DEL         FFTransform* pFFT = pFFTrans;
//DEL         if (chan <= 1) 
//DEL         {
//DEL             pSampleIter->SetPos (0);
//DEL             pSampleIter->SetWorkChannel (WRKCHANNEL_LEFT);
//DEL             pFFTrans->fftTransformMag (*pSampleIter,true);   
//DEL         
//DEL             maxFreq = (pSampleIter->Count() / 2);
//DEL             freqChg = maxFreq / (SPECSCOPEWIDTH+1);
//DEL         }
//DEL                                                                    
//DEL             int    idx;
//DEL             int    freqIdx    = -2;
//DEL             int    freqCnt    = 0;
//DEL             int    accumlator = 0;
//DEL             float* pMags      = pFFTrans->fftGetMagArray();
//DEL 
//DEL             memset (freqAccum, 0, sizeof (freqAccum));
//DEL 
//DEL             for (idx = 0; idx < maxFreq; idx++)
//DEL             {                                    
//DEL                 // get the max power between freq band areas
//DEL                 if (pMags[idx] > accumlator)
//DEL                     accumlator = (int)pMags[idx];
//DEL 
//DEL                 freqCnt++;
//DEL                 if (freqCnt > freqChg)
//DEL                 {
//DEL                     freqCnt = 0;
//DEL                     freqIdx++;
//DEL 
//DEL                     // chop off the lower frequencies that have too much power 
//DEL                     // and over staturate the output
//DEL                     if (freqIdx >= 0) 
//DEL                         freqAccum[freqIdx] = accumlator; 
//DEL 
//DEL                     accumlator = 0;
//DEL                 }                                                                        
//DEL             }
//DEL 
//DEL             if (freqIdx < (SPECSCOPEWIDTH-1))
//DEL             {
//DEL                 freqIdx++;
//DEL                 freqAccum[freqIdx] = accumlator;
//DEL             }   
//DEL 			
//DEL             for (idx = 0; idx < SPECSCOPEWIDTH; idx++)
//DEL             {
//DEL 				int i = rps;
//DEL 				
//DEL 				int iFreaqAccum=freqAccum[idx];
//DEL 
//DEL                 if (freqAccum[idx] >= channels[chan][idx])
//DEL                     channels[chan][idx] = freqAccum[idx];
//DEL                 else
//DEL                 {
//DEL 
//DEL                     channels[chan][idx] -= (128 * rps);
//DEL                     if (channels[chan][idx] < 0)
//DEL                         channels[chan][idx] = 0;
//DEL                 }
//DEL 				int iChannel = channels[chan][idx];
//DEL 				double dVal = ((double)iChannel/8191)*rect.Height();
//DEL 
//DEL 				m_cFake.m_iLastHeight[idx]=rect.Height()-(int)dVal;;
//DEL 			}
//DEL 			m_cFake.DrawEqualizer();
//DEL }

UINT CWcxPlayerDlg::GetDurationOf1SampleUnit()
{
	float ft=SAMPLE_BUFFER;
	float fTmp=ft/m_DShow.GetCurrentAvgByteperSecond();
	return 1000*fTmp;
}

//LRESULT CWcxPlayerDlg::OnUpdateHistogram(WPARAM wParam, LPARAM lParam)
LRESULT CWcxPlayerDlg::OnUpdateHistogram(WPARAM wParam, LPARAM lParam)
{
	if(m_DShow.GetGraphState()==2 && m_DShow.m_bIsSeeking==false)	
	{
		//UINT uNow = timeGetTime();
		//if(uNow-m_uLastHistogramUpdate > 20)
		{
			HistogramValue *pHV= (HistogramValue *)lParam;
			CRect rect;
			GetDlgItem(IDC_FAKE_EQU)->GetClientRect(&rect);
			for(int i=0;i<HISTOGRAM_COUNT;i++)
			{
				int iH = pHV->h[i];
				double dVal = ((double)iH/8191)*rect.Height();
				m_cFake.m_iLastHeight[i]=rect.Height()-(int)dVal;;
			}
			m_cFake.DrawEqualizer();
			//m_uLastHistogramUpdate = timeGetTime();
		}
	}
	HeapFree(m_hHeap,0,(LPVOID)lParam);
	return 1;
}

void CWcxPlayerDlg::StartHistogramThread()
{
	m_pFFTThread = (CModTimer *)AfxBeginThread(RUNTIME_CLASS(CModTimer),
                                    THREAD_PRIORITY_NORMAL,
                                    0, // stack size
                                    CREATE_SUSPENDED);

	m_pFFTThread->m_hMainWnd=m_hWnd;
	m_pFFTThread->m_hHeap=m_hHeap;
	m_pFFTThread->m_bIsPlaying=true;
	m_pFFTThread->m_psc=&m_cs;
	m_pFFTThread->m_iSampleRate = m_DShow.GetCurrentSampleRate();
	m_pFFTThread->m_iChannels = m_DShow.GetChannelCount();
	m_pFFTThread->m_iBit = m_DShow.GetCurrentSampleSize();
	m_pFFTThread->m_iBytePerSecond = m_DShow.GetCurrentAvgByteperSecond();
	m_pFFTThread->ResumeThread();
	m_pFFTThread->PostThreadMessage(WM_MEDIA_PLAY,0,0);
}

void CWcxPlayerDlg::StopHistogramThread()
{
	m_cs.Lock();
	m_pFFTThread->m_bIsPlaying=false;
	m_cs.Unlock();
//	m_pFFTThread->PostThreadMessage(WM_MEDIA_STOP, 0, 0);
	m_pFFTThread->PostThreadMessage(WM_QUIT, 0, 0);
	WaitForSingleObject(m_pFFTThread->m_hThread,INFINITE);
	m_pFFTThread=NULL;
}

LRESULT CWcxPlayerDlg::RunMedia(WPARAM wParam, LPARAM lParam)
{
	StartHistogramThread();
	m_DShow.GraphRun();
	/*
	if(m_pAdvDlg)
	{
		m_pAdvDlg->m_cSeekBar.SetReferenceTime();
	}
	*/
	m_cSeekBar.SetReferenceTime();
	m_cSeekBar.SetTimer(2,1000,NULL);
	return 1;
}

LRESULT CWcxPlayerDlg::OnBeginSeek(WPARAM wParam, LPARAM lParam)
{
	return 1;
}


HRESULT CWcxPlayerDlg::UpdateYahooMSN(LPCTSTR sMsg)
{
	CString sMessage = sMsg;
	sMessage = "WINAMPUN!!! " + sMessage;
	CString sUser;
	HWND hWndYahoo = ::FindWindow("YahooBuddyMain", NULL);
	if(hWndYahoo)
	{
		HKEY hRegHdl;
		if(ERROR_SUCCESS==RegOpenKey(HKEY_CURRENT_USER,"Software\\Yahoo\\Pager",&hRegHdl))
		{
			TCHAR sValName[MAX_VALUE_NAME];
			TCHAR sValVl[MAX_PATH];
			DWORD dwSize1=MAX_VALUE_NAME;
			DWORD dwSize2=MAX_PATH;			
			sValName[0] ='\0';
			DWORD dwType;
			int i =0;
			while(ERROR_SUCCESS==RegEnumValue(hRegHdl,i,sValName, &dwSize1, NULL,&dwType,(BYTE*)sValVl,&dwSize2))
			{
				i++;
				CString sName = sValName;
				if(sName=="Yahoo! User ID")
				{
					sUser=sValVl;
					break;
				}
				dwSize1=MAX_VALUE_NAME;
				dwSize2=MAX_PATH;
			}
			RegCloseKey(hRegHdl);
			if(ERROR_SUCCESS==RegOpenKey(HKEY_CURRENT_USER,"Software\\Yahoo\\Pager\\profiles\\"+ sUser + "\\Custom Msgs",&hRegHdl))
			{			
				RegDeleteValue(hRegHdl,"1");
				RegDeleteValue(hRegHdl,"1_bin");
				RegDeleteValue(hRegHdl,"1_DND");
				RegDeleteValue(hRegHdl,"2");
				RegDeleteValue(hRegHdl,"2_bin");
				RegDeleteValue(hRegHdl,"2_DND");
				RegDeleteValue(hRegHdl,"3");
				RegDeleteValue(hRegHdl,"3_bin");
				RegDeleteValue(hRegHdl,"3_DND");
				RegDeleteValue(hRegHdl,"4");
				RegDeleteValue(hRegHdl,"4_bin");
				RegDeleteValue(hRegHdl,"4_DND");
				RegDeleteValue(hRegHdl,"5");
				RegDeleteValue(hRegHdl,"5_bin");
				RegDeleteValue(hRegHdl,"5_DND");
				RegSetValueEx(hRegHdl,"5",NULL,REG_SZ,(CONST BYTE*)sMessage.GetBuffer(0),sMessage.GetLength()+1);
				RegCloseKey(hRegHdl);
				::PostMessage(hWndYahoo,0x111, 0x188, 0);
				return S_OK;
			}
			//else
			//	MessageBox("Can't open Custome Message [" + sUser+"]");
		}
		//else
		//	MessageBox("Can't find Yahoo USER ID");
		return E_FAIL;
	}
	//else
	//	MessageBox("Can't find Yahoo Window");
	return S_OK;
}
