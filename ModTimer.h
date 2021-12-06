#if !defined(AFX_MODTIMER_H__C4B28CBB_BF51_4DDE_BACF_F644947F8DE9__INCLUDED_)
#define AFX_MODTIMER_H__C4B28CBB_BF51_4DDE_BACF_F644947F8DE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModTimer.h : header file
//

#include "FFTransform.h"

/////////////////////////////////////////////////////////////////////////////
// CModTimer thread


class CModTimer : public CWinThread
{
	DECLARE_DYNCREATE(CModTimer)
protected:
	CModTimer();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	//LRESULT MatamuAsu(WPARAM wParam, LPARAM lParam);
	/*
	LRESULT OnSampleSeekIsReady(WPARAM wParam, LPARAM lParam);
	LRESULT PrepareThreadForBufferData(WPARAM wParam, LPARAM lParam);
	LRESULT CleanUpThreadfromBufferData(WPARAM wParam, LPARAM lParam);
	*/

	void OnSampleSeekIsReady(WPARAM wParam, LPARAM lParam);
	void PrepareThreadForBufferData(WPARAM wParam, LPARAM lParam);
	void CleanUpThreadfromBufferData(WPARAM wParam, LPARAM lParam);

	void CleanUpThread();
	void PrepareThread();
	void UpdateHistogram(AudioSampleMemory *pSampleInfo);
	//LRESULT OnSampleIsReady(WPARAM wParam, LPARAM lParam);
	void OnSampleIsReady(WPARAM wParam, LPARAM lParam);
	HANDLE m_hHeap;
	HWND m_hMainWnd;
	DWORD FFT(LPVOID pBuffer);
	int m_iSampleRate;
	int m_iChannels;
	int m_iBit;
	int m_iBytePerSecond;
	bool m_bIsPlaying;
	CCriticalSection *m_psc;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModTimer)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
private:
	HistogramValue m_HI;
	FFTransform*  m_pFFTrans;  
	SampleIter*   m_pSampleIter;
	UINT m_uLastSampleSent;
protected:
	virtual ~CModTimer();

	// Generated message map functions
	//{{AFX_MSG(CModTimer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODTIMER_H__C4B28CBB_BF51_4DDE_BACF_F644947F8DE9__INCLUDED_)
