// DShowMP3.cpp: implementation of the CDShowMP3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "WcxPlayer.h"
#include "DShowMP3.h"
#include "EquFilter.h"
#include <atlbase.h>
#include "FileLog.h"
//#include "ITestFilter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
DWORD     g_dwGraphRegister=0;
CComPtr<IBaseFilter>    g_pFilter;
CEquFilter * g_pEquFilter=NULL;

// {A6512C9F-A47B-45ba-A054-0DB0D4BB87F7}
//static const GUID CLSID_YuvGray = 
//{ 0xa6512c9f, 0xa47b, 0x45ba, { 0xa0, 0x54, 0xd, 0xb0, 
//                                0xd4, 0xbb, 0x87, 0xf7 } };
// {8bfc560b-7d3b-432d-8552-427fbbef7347}
const GUID  CLSID_TestFilter = 
{ 0x8bfc560b, 0x7d3b, 0x432d, { 0x85, 0x52, 0x42, 0x7f, 
								0xbb, 0xef, 0x73, 0x47 } };
// {0e1fbc30-d76d-42f3-8d03-29ee6f757d06}
const GUID  IID_ITestFilter = 
{ 0xe1fbc30, 0xd76d, 0x42f3, { 0x8d, 0x3, 0x29, 0xee, 
								0x6f, 0x75, 0x7d, 0x6 } };
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDShowMP3::CDShowMP3()
{
	CoInitialize(NULL);
	m_pGraph=NULL;
	m_pME=NULL;
	m_pMC=NULL;
	m_pDevice=NULL;
	m_pMediaSeeking=NULL;
	m_WndMain = NULL;
	m_pBA = NULL;
	m_lVolume = 0;
	m_pACM = NULL;
	m_pMPEGSpliter=NULL;
	m_bIsSeeked = false;
	m_bIsSeeking = false;
}

CDShowMP3::~CDShowMP3()
{
	CoUninitialize();
}

HRESULT CDShowMP3::CreateInterface()
{	
	HRESULT hr;		
    hr = CoCreateInstance (CLSID_FilterGraph, NULL, CLSCTX_INPROC,
                           IID_IGraphBuilder, (void **) &m_pGraph);
    if (FAILED(hr))
	{
		MessageBox(NULL,"Couldn't create graph","Error",MB_OK);		
        return hr;	
	}

	hr = m_pGraph->QueryInterface(IID_IMediaControl,(LPVOID *) &m_pMC);
    if (FAILED(hr))
	{		
		MessageBox(NULL,"Couldn't create Media Control","Error",MB_OK);	
        return hr;
	}

	hr = m_pGraph->QueryInterface(IID_IMediaEvent, (LPVOID *) &m_pME);
    if (FAILED(hr))
	{
		MessageBox(NULL,"Couldn't create Event Handler","Error",MB_OK);			
        return hr;
	}
	else
	{
		m_pME->SetNotifyWindow((OAHWND)m_WndMain->m_hWnd,WM_GRAPHNOTIFY, 0);
	}

	hr = m_pGraph->QueryInterface(IID_IMediaSeeking,reinterpret_cast<void **>(&m_pMediaSeeking));
    if (FAILED(hr))
	{
		MessageBox(NULL,"Couldn't create Seeker","Error",MB_OK);				
        return hr;
	}

	hr = m_pGraph->QueryInterface(IID_IBasicAudio, (LPVOID *) &m_pBA);
	if(FAILED(hr))
	{
		MessageBox(NULL,"Failed to create Basic Audio","Error",MB_OK);
		return hr;
	}

	return S_OK;
}

HRESULT CDShowMP3::RemoveFilter()
{
	if(m_pGraph)
	{
		IEnumFilters *pEnum = NULL;
		HRESULT hr = m_pGraph->EnumFilters(&pEnum);
		if (SUCCEEDED(hr))
		{
			IBaseFilter *pFilter = NULL;
			while (S_OK == pEnum->Next(1, &pFilter, NULL))
			 {
				 // Remove the filter.
				 m_pGraph->RemoveFilter(pFilter);
				 // Reset the enumerator.
				 pEnum->Reset();
				 pFilter->Release();
			}
			pEnum->Release();
			return hr;
		}
		else
			MessageBox(NULL,GetErrorMessage(),"Error",MB_OK);
	}	
	return E_FAIL;
}

CString CDShowMP3::GetErrorMessage()
{
	LPVOID lpMsgBuf;
	::FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);

	CString sRet = (LPCTSTR)lpMsgBuf;
	LocalFree( lpMsgBuf );
	return sRet;
}

void CDShowMP3::ReleaseInterface()
{	
	SAFE_RELEASE(m_pBA);
	SAFE_RELEASE(m_pMC);		
	SAFE_RELEASE(m_pME);
	SAFE_RELEASE(m_pMediaSeeking);
	SAFE_RELEASE(m_pGraph);
	SAFE_RELEASE(m_pDevice);
}

int CDShowMP3::GetGraphState()
{
	OAFilterState eDate;
	if(m_pMC)
	{
		if(SUCCEEDED(m_pMC->GetState(500,&eDate)))
		{
			if(eDate==State_Stopped)
				return 0;
			if(eDate==State_Paused)
				return 1;
			if(eDate==State_Running)
				return 2;
			return -1;
		}
		else
			return -1;
	}
	else
		return -1;
}

HRESULT CDShowMP3::BuildPlayerGraph()
{
	CString s ;
	if(SUCCEEDED(m_pGraph->RenderFile(m_bMediaPath,NULL)))
	{
#ifdef _DEBUG
		HRESULT hr = AddGraphToRot(m_pGraph, &g_dwGraphRegister);
		if (FAILED(hr))
		{
			MessageBox(NULL,"Failed to register filter graph with ROT!","Error",MB_OK);
			g_dwGraphRegister = 0;
		}
#endif
		DWORD dwCap = 0;
		if(SUCCEEDED(m_pMediaSeeking->GetCapabilities(&dwCap)))
		{
			if (AM_SEEKING_CanSeekAbsolute & dwCap)
			{
				REFERENCE_TIME rtNow  = 0;
				if(m_pMediaSeeking->SetPositions(&rtNow,  AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning)!=S_OK)				
				{       
					s = GetErrorMessage();
					MessageBox(NULL,"Failed to seek the beginning of the media (" + s +")","Error",MB_OK);	
				}

			}
		}
		if(FAILED(m_pGraph->QueryInterface(IID_IBasicAudio, (LPVOID *) &m_pBA)))		
		{
			s = GetErrorMessage();
			MessageBox(NULL,"Failed to create Basic Audio (" + s +")","Error",MB_OK);
		}
		return S_OK;
	}
	else
	{
		s = GetErrorMessage();
		MessageBox(NULL,"Failed to Render audio file (" + s +")","Error",MB_OK);
	}

	return E_FAIL;
}

HRESULT CDShowMP3::GraphRun()
{
	if(SUCCEEDED(m_pMC->Run()))
	{
		m_uStartTimeAfterSeek = timeGetTime();
		if(m_pMediaSeeking)
			m_pMediaSeeking->GetCurrentPosition(&m_llPosAfterSeek);
		if(m_pBA)
		{
			if(m_pBA->put_Volume(m_lVolume)!=S_OK)
			//if(FAILED(m_pBA->put_Volume(-2000)))
				MessageBox(NULL,"Failed to set volume level","Error",MB_OK);
		}
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CDShowMP3::GraphStop()
{	
	if(m_pGraph)
	{
#ifdef _DEBUG
		if (g_dwGraphRegister)
		{
			RemoveGraphFromRot(g_dwGraphRegister);
			g_dwGraphRegister = 0;
		}
#endif
		if(SUCCEEDED(m_pMC->Stop()))
		{
			//SAFE_RELEASE(m_pBA);
			return S_OK;
		}
		MessageBox(NULL,"Failed to stop the graph","Error",MB_OK);
	}
	return E_FAIL;
}

int CDShowMP3::TranslateEvent()
{
	long evCode, param1, param2;
	HRESULT hr;
	int iRet=0;
	while (hr = m_pME->GetEvent(&evCode, &param1, &param2, 0), SUCCEEDED(hr))
	{
		switch(evCode) 
		{ 
		case EC_COMPLETE:
			// Call application-defined functions for each 
			// type of event that you want to handle.
			iRet=1;
			GraphStop();
			RemoveFilter();
		}
    } 
    hr = m_pME->FreeEventParams(evCode, param1, param2);
	return iRet;
}

HRESULT CDShowMP3::GraphPause(bool bPause)
{
//	CFileLog log;
//	log.SetAttributes(true,true);
	OAFilterState eDate;
	if(bPause)
	{
		HRESULT hr;
		hr=m_pMC->Pause();
		if(hr==S_OK)
		{
//			log.WriteData("PAUSE");
			return S_OK;
		}
		else if(hr==S_FALSE)
		{
			hr=m_pMC->GetState(500,&eDate);
			if(hr==S_OK)
			{
//				log.WriteData("PAUSE");
				return S_OK;
			}
			else
			{
				return E_FAIL;
			}
		}
	}
	else
	{
		if(SUCCEEDED(m_pMC->Run()))
		{
//			log.WriteData("RUN");
			return S_OK;
		}
	}
	return E_FAIL;
}

long CDShowMP3::GetCurrentVolume()
{
	CString s;
	if(GetGraphState()==1 || GetGraphState()==2)
	{
		if(m_pBA)
		{
			if(m_pBA->get_Volume(&m_lVolume)!=S_OK)
			{
				s = GetErrorMessage();
				MessageBox(NULL,"Failed to detect current volume level ("+ s + ")","Error",MB_OK);				
			}			
		}
	}
	return m_lVolume;
}

HRESULT CDShowMP3::SetCurrentVolume(long lVol)
{
	CString s;
	m_lVolume = lVol;
	if(GetGraphState()==1 || GetGraphState()==2)
	{
		if(m_pBA)
		{
			if(m_pBA->put_Volume(m_lVolume)!=S_OK)
			{
				s = GetErrorMessage();
				MessageBox(NULL,"Failed to set volume level ("+ s + ")","Error",MB_OK);				
				return E_FAIL;
			}			
		}
	}
	return S_OK;
}

HRESULT CDShowMP3::GetCurrentPosition(REFERENCE_TIME * rtNow, REFERENCE_TIME * rtStop)
{
	if(GetGraphState()==1 || GetGraphState()==2)
	{
		DWORD dwCap = 0;
		if(SUCCEEDED(m_pMediaSeeking->GetCapabilities(&dwCap)))
		{
			if (AM_SEEKING_CanSeekAbsolute & dwCap)
			{
				if(SUCCEEDED(m_pMediaSeeking->GetPositions(rtNow,rtStop)))
				{
					return S_OK;
				}
			}
		}
	}
	return E_FAIL;
}

HRESULT CDShowMP3::SetCurrentPosition(REFERENCE_TIME rtNow)
{
	if(GetGraphState()==1 || GetGraphState()==2)
	{
		DWORD dwCap = 0;
		if(SUCCEEDED(m_pMediaSeeking->GetCapabilities(&dwCap)))
		{
			if (AM_SEEKING_CanSeekAbsolute & dwCap)
			{
				if(SUCCEEDED(m_pMediaSeeking->SetPositions(&rtNow,  AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning)))
				{
//					m_uBufferIndex=0;
					return S_OK;
				}
			}
		}
	}
	return E_FAIL;
}

HRESULT CDShowMP3::AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    if (FAILED(GetRunningObjectTable(0, &pROT))) 
    {
        return E_FAIL;
    }

    WCHAR wsz[128];
    wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, 
              GetCurrentProcessId());

    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) 
    {
        // Use the ROTFLAGS_REGISTRATIONKEEPSALIVE to ensure a strong reference
        // to the object.  Using this flag will cause the object to remain
        // registered until it is explicitly revoked with the Revoke() method.
        //
        // Not using this flag means that if GraphEdit remotely connects
        // to this graph and then GraphEdit exits, this object registration 
        // will be deleted, causing future attempts by GraphEdit to fail until
        // this application is restarted or until the graph is registered again.
        hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph, 
                            pMoniker, pdwRegister);
        pMoniker->Release();
    }

    pROT->Release();
    return hr;
}

void CDShowMP3::RemoveGraphFromRot(DWORD pdwRegister)
{
    IRunningObjectTable *pROT;

    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) 
    {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}

HRESULT CDShowMP3::CreateDefaultMP3Interface()
{
	HRESULT hr;		
    hr = CoCreateInstance (CLSID_ACMWrapper, NULL, CLSCTX_INPROC,
                           IID_IBaseFilter, (void **) &m_pACM);
    if (FAILED(hr))
	{
		MessageBox(NULL,"Couldn't create ACM Wrapper","Error",MB_OK);		
        return hr;	
	}
	hr = CoCreateInstance (CLSID_MPEG1Splitter, NULL, CLSCTX_INPROC,
                           IID_IBaseFilter, (void **) &m_pMPEGSpliter);
    if (FAILED(hr))
	{
		MessageBox(NULL,"Couldn't create MPEG Splitter","Error",MB_OK);		
        return hr;	
	}
	
	return S_OK;
}

void CDShowMP3::ReleaseMP3Interface()
{
	SAFE_RELEASE(m_pACM);
	SAFE_RELEASE(m_pMPEGSpliter);
//	g_pFilter.Release();
}

HRESULT CDShowMP3::BuildMP3Graph()
{
	if(SUCCEEDED(m_pGraph->AddFilter(m_pACM,L"ACM WRAPPER")))
	{
		if(FAILED(m_pGraph->AddFilter(m_pMPEGSpliter,L"MPEG-1 SPLITTER")))
		{
			MessageBox(NULL,"Failed to add MPEG-1 Spliter!","Error",MB_OK);
			return E_FAIL;
		}
	}
	else
	{
		MessageBox(NULL,"Failed to add ACM Wraper","Error",MB_OK);
		return E_FAIL;
	}

	CString s ;
	if(SUCCEEDED(m_pGraph->RenderFile(m_bMediaPath,NULL)))
	{
#ifdef _DEBUG
		HRESULT hr = AddGraphToRot(m_pGraph, &g_dwGraphRegister);
		if (FAILED(hr))
		{
			MessageBox(NULL,"Failed to register filter graph with ROT!","Error",MB_OK);
			g_dwGraphRegister = 0;
		}
#endif
		DWORD dwCap = 0;
		if(SUCCEEDED(m_pMediaSeeking->GetCapabilities(&dwCap)))
		{
			if (AM_SEEKING_CanSeekAbsolute & dwCap)
			{
				REFERENCE_TIME rtNow  = 0;
				if(m_pMediaSeeking->SetPositions(&rtNow,  AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning)!=S_OK)				
				{       
					s = GetErrorMessage();
					MessageBox(NULL,"Failed to seek the beginning of the media (" + s +")","Error",MB_OK);	
				}

			}
		}
		if(FAILED(m_pGraph->QueryInterface(IID_IBasicAudio, (LPVOID *) &m_pBA)))		
		{
			s = GetErrorMessage();
			MessageBox(NULL,"Failed to create Basic Audio (" + s +")","Error",MB_OK);
		}
		return S_OK;
	}
	else
	{
		s = GetErrorMessage();
		MessageBox(NULL,"Failed to Render audio file (" + s +")","Error",MB_OK);
	}

	return E_FAIL;
}

//DEL HRESULT CDShowMP3::AddCustomEffect()
//DEL {
//DEL 	HRESULT hr;
//DEL 	IBaseFilter * pCustom=NULL;
//DEL   /*  hr = CoCreateInstance (CLSID_TestFilter, NULL, CLSCTX_INPROC,
//DEL                            IID_IBaseFilter, (void **) &pCustom);
//DEL 	if(SUCCEEDED(hr))
//DEL 	{
//DEL 		m_pGraph->AddFilter(pCustom ,L"EQU SNATCHER");
//DEL 	}*/
//DEL 	
//DEL 	g_pEquFilter = new CEquFilter("EquFilter",NULL,&hr);
//DEL 	if (!g_pEquFilter)
//DEL     {
//DEL 		return E_FAIL;
//DEL 	}
//DEL //	g_pRenderer = g_pEquFilter;
//DEL 	
//DEL 	if(FAILED(m_pGraph->AddFilter(g_pRenderer ,L"EQU SNATCHER")))
//DEL 	{	
//DEL 		return E_FAIL;
//DEL 	}
//DEL //	pCustom->Release();
//DEL //	g_pEquFilter->Release();
//DEL 	return S_OK;
//DEL }

HRESULT CDShowMP3::ConnectCustomFilter()
{
	BOOL       bFound = FALSE;
    IEnumPins  *pEnum;
    IPin       *pPin;

    HRESULT hr = m_pACM->EnumPins(&pEnum);
    if (FAILED(hr))
    {
        return E_FAIL;
    }
	while(pEnum->Next(1, &pPin, 0) == S_OK)
    {
        PIN_DIRECTION PinDirThis;
		PIN_DIRECTION PinDir = PINDIR_OUTPUT;
        pPin->QueryDirection(&PinDirThis);
        if (bFound = (PinDir == PinDirThis))
            break;
        pPin->Release();
    }
    pEnum->Release();
//    return (bFound ? pPin : NULL);  

	return S_OK;
}

HRESULT CDShowMP3::BuildGraphManually()
{
	CComPtr<IBaseFilter> pSrc=NULL;
	CComPtr<IPin>  pFSrcPinOut=NULL;
	CComPtr<IPin>  pFPinIn=NULL;
	CComPtr<IPin>	pFPinOut=NULL;	
	
	if(SUCCEEDED(m_pGraph->AddSourceFilter(m_bMediaPath,L"SOURCE",&pSrc)))
	{
		if(SUCCEEDED(m_pGraph->AddFilter(m_pACM,L"ACM WRAPPER")))
		{
			if(SUCCEEDED(m_pGraph->AddFilter(m_pMPEGSpliter,L"MPEG-1 SPLITTER")))
			{		
				if(m_pGraph->AddFilter(g_pFilter ,L"HISTOGRAM FILTER")!=S_OK)
				{	
					MessageBox(NULL,"Could not add histogram filter","Error",MB_OK);
					return E_FAIL;
				}
				if (pSrc->FindPin(L"Output", &pFSrcPinOut)!=S_OK)
				{
					MessageBox(NULL,"Could not find output pin","Error",MB_OK);
					return E_FAIL;
				}
				
				if (g_pFilter->FindPin(L"In", &pFPinIn)!=S_OK)
				{
					MessageBox(NULL,"Could not find input pin","Error",MB_OK);
					return E_FAIL;
				}
				
				if (m_pGraph->Connect(pFSrcPinOut, pFPinIn)!=S_OK)
				{					
					MessageBox(NULL,"Could not connect pins!","Error",MB_OK);;
					return E_FAIL;
				}
				
				if (g_pFilter->FindPin(L"Out", &pFPinOut)!=S_OK)
				{
					MessageBox(NULL,"Could not find histogram's output pin","Error",MB_OK);
					return E_FAIL;
				}	
				if(m_pGraph->Render(pFPinOut)!=S_OK)
				{
					MessageBox(NULL,"Could not render histogram's output pin","Error",MB_OK);
					return E_FAIL;
				}
				
#ifdef _DEBUG
				HRESULT hr = AddGraphToRot(m_pGraph, &g_dwGraphRegister);
				if (FAILED(hr))
				{
					MessageBox(NULL,"Failed to register filter graph with ROT!","Error",MB_OK);
					g_dwGraphRegister = 0;
				}
#endif
				DWORD dwCap = 0;CString s;
				if(SUCCEEDED(m_pMediaSeeking->GetCapabilities(&dwCap)))
				{
					if (AM_SEEKING_CanSeekAbsolute & dwCap)
					{
						REFERENCE_TIME rtNow  = 0;
						if(m_pMediaSeeking->SetPositions(&rtNow,  AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning)!=S_OK)				
						{       
							s = GetErrorMessage();
							MessageBox(NULL,"Failed to seek the beginning of the media (" + s +")","Error",MB_OK);	
						}
					}
				}

				m_pWaveFmt = &(g_pEquFilter->m_WaveFmt);
				return S_OK;
			}
			else
			{
				MessageBox(NULL,"Failed to add MPEG-1 Spliter!","Error",MB_OK);
				return E_FAIL;
			}
		}
		else
		{
			MessageBox(NULL,"Failed to add ACM Wraper","Error",MB_OK);
			return E_FAIL;
		}
	}
	else
	{
//		MessageBox(NULL,"Failed to open file","Error",MB_OK);
		return E_FAIL;
	}
	return E_FAIL;
}

HRESULT CDShowMP3::CreateHistogramEffectFilter()
{
	HRESULT hr;
	g_pEquFilter = new CEquFilter("EquFilter",NULL,&hr);
	if(g_pEquFilter)
	{
		g_pEquFilter->m_HMainWnd=m_WndMain->m_hWnd;
		g_pEquFilter->m_hSampleHeap = m_hSampleHeap;
		g_pFilter=g_pEquFilter;
		/*
		if(FAILED(m_pGraph->QueryInterface(IID_IBasicAudio, (LPVOID *) &m_pBA)))		
		{
			CString s;
			s = GetErrorMessage();
			MessageBox(NULL,"Failed to create Basic Audio (" + s +")","Error",MB_OK);
		}
		*/
		return S_OK;
	}
	return E_FAIL;
}

void CDShowMP3::ReleaseHistogramFilter()
{
	if(!g_pFilter)
		g_pFilter.Release();
	g_pFilter=NULL;
}


DWORD CDShowMP3::GetCurrentSampleRate()
{
	if(m_pWaveFmt)
	{
		return m_pWaveFmt->nSamplesPerSec;
	}
	return 0;
}

WORD CDShowMP3::GetCurrentSampleSize()
{
	if(m_pWaveFmt)
	{
		return m_pWaveFmt->wBitsPerSample;
	}
	return 0;
}


WORD CDShowMP3::GetChannelCount()
{
	if(m_pWaveFmt)
	{
		return m_pWaveFmt->nChannels;
	}
	return 0;
}

DWORD CDShowMP3::GetCurrentAvgByteperSecond()
{
	if(m_pWaveFmt)
	{
		return m_pWaveFmt->nAvgBytesPerSec;
	}
	return 0;
}

HRESULT CDShowMP3::SetCurrentPosition2(REFERENCE_TIME rtNow)
{
	DWORD dwCap = 0;
	if(SUCCEEDED(m_pMediaSeeking->GetCapabilities(&dwCap)))
	{
		if (AM_SEEKING_CanSeekAbsolute & dwCap)
		{
			if(SUCCEEDED(m_pMediaSeeking->SetPositions(&rtNow,  AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning)))
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CDShowMP3::GetCurrentPosition2(REFERENCE_TIME * rtNow, REFERENCE_TIME * rtStop)
{
	DWORD dwCap = 0;
	if(SUCCEEDED(m_pMediaSeeking->GetCapabilities(&dwCap)))
	{
		if (AM_SEEKING_CanSeekAbsolute & dwCap)
		{
			if(SUCCEEDED(m_pMediaSeeking->GetPositions(rtNow,rtStop)))
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

//DEL void CDShowMP3::Log(const char *sMsg)
//DEL {
//DEL 
//DEL }
