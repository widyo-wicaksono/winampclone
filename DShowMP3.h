// DShowMP3.h: interface for the CDShowMP3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSHOWMP3_H__3A0AA123_EAA2_4C7E_AECC_EAB0D34EFC33__INCLUDED_)
#define AFX_DSHOWMP3_H__3A0AA123_EAA2_4C7E_AECC_EAB0D34EFC33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dshow.h"
#include "comutil.h"

class CDShowMP3  
{
public:

	DWORD GetCurrentAvgByteperSecond();
	DWORD GetCurrentSampleRate();

	WORD GetChannelCount();
	WORD GetCurrentSampleSize();
	
	UINT m_uStartTimeAfterSeek;
	LONGLONG m_llPosAfterSeek;
//	UINT m_uBufferIndex;
	HANDLE m_hSampleHeap;
	WAVEFORMATEX * m_pWaveFmt;
	//BSTR m_bMediaPath;
	_bstr_t m_bMediaPath;

	void ReleaseHistogramFilter();
	void ReleaseMP3Interface();
	void RemoveGraphFromRot(DWORD pdwRegister);
	void ReleaseInterface();

	HRESULT GetCurrentPosition2(REFERENCE_TIME * rtNow, REFERENCE_TIME * rtStop);
	HRESULT SetCurrentPosition2(REFERENCE_TIME rtNow);
	HRESULT CreateHistogramEffectFilter();
	HRESULT BuildGraphManually();
	HRESULT ConnectCustomFilter();
	HRESULT BuildMP3Graph();
	HRESULT CreateDefaultMP3Interface();
	HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister); 
	HRESULT SetCurrentPosition(REFERENCE_TIME rtNow);
	HRESULT GetCurrentPosition(REFERENCE_TIME * rtNow, REFERENCE_TIME * rtStop);
	HRESULT SetCurrentVolume(long lVol);	
	HRESULT GraphPause(bool bPause);	
	HRESULT GraphStop();
	HRESULT GraphRun();
	HRESULT BuildPlayerGraph();
	HRESULT RemoveFilter();
	HRESULT CreateInterface();

	int GetGraphState();
	int TranslateEvent();	
	long GetCurrentVolume();

	bool m_bIsSeeked;
	bool m_bIsSeeking;

	CWnd * m_WndMain;		
	CString GetErrorMessage();
	CDShowMP3();

	virtual ~CDShowMP3();
private:
	IGraphBuilder * m_pGraph;
	IMediaEventEx * m_pME;
	IMediaControl * m_pMC;
	IBaseFilter	* m_pDevice;
	IMediaSeeking * m_pMediaSeeking;
	IBasicAudio   * m_pBA;
	IBaseFilter	* m_pACM;
	IBaseFilter	* m_pMPEGSpliter;
//	CEquFilter * m_pEquFilter;
	long m_lVolume;
	
};

#endif // !defined(AFX_DSHOWMP3_H__3A0AA123_EAA2_4C7E_AECC_EAB0D34EFC33__INCLUDED_)
