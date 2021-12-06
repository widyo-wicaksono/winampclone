// WcxPlayerDlg.h : header file
//

#if !defined(AFX_WCXPLAYERDLG_H__6F18DD78_6D8B_47F1_ABFA_4E47CCA0D535__INCLUDED_)
#define AFX_WCXPLAYERDLG_H__6F18DD78_6D8B_47F1_ABFA_4E47CCA0D535__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "BitmapDialog.h"
#include "xSkinButton.h"
#include "SkinlistCtrl.h"
#include "MP3\WMP3.h"
#include "FakeEqu.h"
#include "RuningText.h"
#include "BigTime.h"
#include "DShowMP3.h"
#include "InfoDlg.h"
#include "WinAmpAboutDlg.h"
#include "HelpDlg.h"
#include "AdvDlg.h"
#include "SampleBufferManager.h"
#include "FFTransform.h"
#include "ModTimer.h"

#include <afxtempl.h>
/////////////////////////////////////////////////////////////////////////////
// CWcxPlayerDlg dialog



class CWcxPlayerDlg : public CDialog
{
// Construction
public:
	HRESULT UpdateYahooMSN(LPCTSTR sMsg);
	void StopHistogramThread();
	void StartHistogramThread();
	UINT GetDurationOf1SampleUnit();
	
	DWORD GenerateBufferIndex();

	LRESULT OnBeginSeek(WPARAM wParam, LPARAM lParam);
	LRESULT RunMedia(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateHistogram(WPARAM wParam, LPARAM lParam);
	LRESULT OnSampleIsReady(WPARAM wParam, LPARAM lParam);
	LRESULT HandleTrayMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnAdvExit(WPARAM wParam, LPARAM lParam);
	LRESULT OnHelpExit(WPARAM wParam, LPARAM lParam);
	LRESULT OnAboutExit(WPARAM wParam, LPARAM lParam);
	LRESULT OnChildExit(WPARAM wParam, LPARAM lParam);
	LRESULT OnStopAnimFinished(WPARAM wParam, LPARAM lParam);
	LRESULT OnGraphMsg(WPARAM wParam, LPARAM lParam);

	void OnRestore();
	void MinimizeToTray();
	void RemoveAllPlayListItems();
	void OnAdvShow();
	void OnShowHelp();
	void OnExit();
	void OnShowAbout();
	void OnMP3Info();
	void OnRemoveFromList();
	int GetCurrentSelectionIndex();

	void PopulateList(LPCTSTR sPass);

	HRESULT GetMP3Tag(MP3ID3V1TAG *sTag, LPCTSTR sPath);
	HRESULT SetBitmap();
	CWcxPlayerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CWcxPlayerDlg)
	enum { IDD = IDD_WCXPLAYER_DIALOG };
	CBigTime	m_cBigTime;
	CRuningText	m_cRunText;
	CFakeEqu	m_cFake;
	CSkinListCtrl	m_cList;
	CxSkinButton	m_cBtnPause;
	CxSkinButton	m_cBtnStop;
	CxSkinButton	m_cBtnPlay;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWcxPlayerDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CWcxPlayerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnButtonPlay();
	afx_msg void OnButtonStop();
	afx_msg void OnRclickPlaylist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnButtonPause();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	lpfnSetLayeredWindowAttributes m_pSetLayeredWindowAttributes;				
	CArray<MP3Data,MP3Data> m_arrMP3;
	
	int m_iGrState;
	int m_iCurSelectionIndex;	
	bool m_bForcedStop;
	bool m_IsHide;		

	CDShowMP3 m_DShow;
	CSampleBufferManager m_BufferManager;
	CFont font;
	CCriticalSection m_cs;
	CVolBar m_cVolBar;
	CSeekBarWnd m_cSeekBar;

	HBITMAP m_hBitmap;
	HACCEL m_hAccel;
	HANDLE m_hHeap;
	UINT m_uSampleBufferTimeStamp;
	NOTIFYICONDATA m_tnd;
	UINT m_uDurationFor1SampleUnit;
	UINT m_uLastHistogramUpdate;

//	CVolBar * m_pVolBar;
	CModTimer * m_pFFTThread;
	CInfoDlg * m_pInfoDlg;
	CWinAmpAboutDlg * m_pAboutDlg;	
	CHelpDlg * m_pHelDlg;
//	CAdvDlg * m_pAdvDlg;
	WMp3 * m_pWmp3;
	SampleIter*   pSampleIter; 

	FFTransform*  pFFTrans;;
    FFTransform*  pFFTransStereo;;    
	LPVOID m_pLeftBuff;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WCXPLAYERDLG_H__6F18DD78_6D8B_47F1_ABFA_4E47CCA0D535__INCLUDED_)
