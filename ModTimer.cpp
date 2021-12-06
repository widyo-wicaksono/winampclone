// ModTimer.cpp : implementation file
//

#include "stdafx.h"
#include "WcxPlayer.h"
#include "ModTimer.h"
#include "mmsystem.h"
#include "FileLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CModTimer

LONGLONG g_llDurationAllowed=0;
bool g_bIsLate=false;

IMPLEMENT_DYNCREATE(CModTimer, CWinThread)

CModTimer::CModTimer()
{
	m_pFFTrans=NULL; 
	m_pSampleIter=NULL;
	m_bIsPlaying = false;
}

CModTimer::~CModTimer()
{
}

BOOL CModTimer::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	m_uLastSampleSent = timeGetTime();
	return TRUE;
}

int CModTimer::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
//	delete pFFTrans;
 //   delete pSampleIter ;
	CleanUpThread();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CModTimer, CWinThread)
	//{{AFX_MSG_MAP(CModTimer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		ON_THREAD_MESSAGE(WM_MEDIA_PLAY,PrepareThreadForBufferData)
		ON_THREAD_MESSAGE(WM_MEDIA_STOP,CleanUpThreadfromBufferData)
		ON_THREAD_MESSAGE(WM_SAMPLEISREADY,OnSampleIsReady)
		ON_THREAD_MESSAGE(WM_SAMPLESEEKEDISREADY_2,OnSampleSeekIsReady)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModTimer message handlers

DWORD CModTimer::FFT(LPVOID pBuffer)
{
	LPSTR pData = (LPSTR)pBuffer;
	int rps=1;
	m_pSampleIter->SetSampleWorkSet(pData, SAMPLE_BUFFER,  2, true);

	static int channels[SPECSCOPEWIDTH] = {0, 0, 0, 0, 0};

    int chan=0;
    int maxFreq;
    int freqChg;
    int freqAccum[SPECSCOPEWIDTH];
		
    FFTransform* pFFT = m_pFFTrans;

    m_pSampleIter->SetPos (0);
    m_pSampleIter->SetWorkChannel (WRKCHANNEL_LEFT);
    m_pFFTrans->fftTransformMag (*m_pSampleIter,true);   

    maxFreq = (m_pSampleIter->Count() / 2);
    freqChg = maxFreq / (SPECSCOPEWIDTH+1);
                                               
    int    idx;
    int    freqIdx    = -2;
    int    freqCnt    = 0;
    int    accumlator = 0;
    float* pMags      = m_pFFTrans->fftGetMagArray();

    memset (freqAccum, 0, sizeof (freqAccum));

    for (idx = 0; idx < maxFreq; idx++)
    {                                    
        // get the max power between freq band areas
        if (pMags[idx] > accumlator)
            accumlator = (int)pMags[idx];

        freqCnt++;
        if (freqCnt > freqChg)
        {
            freqCnt = 0;
            freqIdx++;

            // chop off the lower frequencies that have too much power 
            // and over staturate the output
            if (freqIdx >= 0) 
                freqAccum[freqIdx] = accumlator; 

            accumlator = 0;
        }                                                                        
    }

    if (freqIdx < (SPECSCOPEWIDTH-1))
    {
        freqIdx++;
        freqAccum[freqIdx] = accumlator;
    }   
			
	LPVOID pVoid = HeapAlloc(m_hHeap,HEAP_ZERO_MEMORY,sizeof(HistogramValue));
	HistogramValue *pHv = (HistogramValue *)pVoid;
    for (idx = 0; idx < SPECSCOPEWIDTH; idx++)
    {
		int i = rps;
		
		int iFreaqAccum=freqAccum[idx];

        if (freqAccum[idx] >= channels[idx])
            channels[idx] = freqAccum[idx];
        else
        {
            channels[idx] -= (128 * rps);
            if (channels[idx] < 0)
                channels[idx] = 0;
        }
		pHv->h[idx]= channels[idx];
	}
	return (DWORD)pVoid;
}

//LRESULT CModTimer::OnSampleIsReady(WPARAM wParam, LPARAM lParam)
void CModTimer::OnSampleIsReady(WPARAM wParam, LPARAM lParam)
{
	//CFileLog log;
	//log.SetAttributes(true,true);
	DWORD uSleep=0;
	AudioSampleMemory *pSampleInfo = (AudioSampleMemory *)lParam;
	m_psc->Lock();
	bool bIsPlaying=m_bIsPlaying;
	m_psc->Unlock();
	if(!bIsPlaying)
	{	
		HeapFree(m_hHeap,0,pSampleInfo->pActualStartAddress);
		HeapFree(m_hHeap,0,(LPVOID)pSampleInfo);
		return;
	}
	
	if(pSampleInfo->st>=0)
	{	
		UINT uNow = timeGetTime();		
		LONGLONG llIndex = (LONGLONG)((uNow-pSampleInfo->uCurrentSystemTime)*10000);
		LONGLONG llActualMediaCurrentPosition = (pSampleInfo->llCurrentMediaPos) + llIndex ;

		if(llActualMediaCurrentPosition>pSampleInfo->ft)
		{
			HeapFree(m_hHeap,0,pSampleInfo->pActualStartAddress);
			HeapFree(m_hHeap,0,(LPVOID)pSampleInfo);
			return;
		}
		if(pSampleInfo->st>llActualMediaCurrentPosition)
		{
			//int iSize=HeapSize(m_hHeap,0,(LPVOID)pSampleInfo->pActualStartAddress);
			uSleep = (UINT)(((pSampleInfo->st) - (llActualMediaCurrentPosition))/10000);
			Sleep(uSleep);
		}
		else
		{
			g_bIsLate=true;
			g_llDurationAllowed= pSampleInfo->ft-llActualMediaCurrentPosition;
		}
		UpdateHistogram(pSampleInfo);
	}
	else
	{
		HeapFree(m_hHeap,0,pSampleInfo->pActualStartAddress);
		HeapFree(m_hHeap,0,(LPVOID)pSampleInfo);
	}
//	return 1;
}

void CModTimer::UpdateHistogram(AudioSampleMemory *pSampleInfo)
{
	int iAddedSleep=0;
	int iDropped=0;
	bool IsCut=false;
	UINT uStartProcess = timeGetTime();

	if(uStartProcess-m_uLastSampleSent>10){
		LONGLONG llStartProcess = uStartProcess*10000;
		LPVOID pBuff=HeapAlloc(m_hHeap,HEAP_ZERO_MEMORY,SAMPLE_BUFFER);
		int iSize = 0;
		
		iSize = HeapSize(m_hHeap, 0, (LPVOID)(pSampleInfo->pActualStartAddress));
					
		PBYTE pTemp = (PBYTE)pSampleInfo->pActualStartAddress;
		int x=0;
		LONGLONG llCorrectDuration = pSampleInfo->ft - pSampleInfo->st;
		int iUnit = iSize/SAMPLE_BUFFER;
		float fDurationPerUnit = (float)(llCorrectDuration/10000)/iUnit;
		float fTotalDurationPerUnit = 0;
		while(true)
		{		
			CopyMemory(pBuff,(LPVOID)(pTemp+(x)),SAMPLE_BUFFER);
			HistogramValue * pHV = (HistogramValue*)FFT(pBuff);
			PostMessage(m_hMainWnd, WM_UPDATE_HISTOGRAM,NULL, (LPARAM)pHV);		
			
			m_psc->Lock();
			bool bIsPlaying=m_bIsPlaying;
			m_psc->Unlock();
			if(!bIsPlaying)
			{
				break;
			}
		
			x=x+SAMPLE_BUFFER;
			if(x+SAMPLE_BUFFER>iSize)
			{
				break;
			}

			UINT uStart = timeGetTime();
			LONGLONG llNow = uStart*10000;
			if(llNow-llStartProcess>=llCorrectDuration){	
				iDropped++;
				break;
			}

			fTotalDurationPerUnit=fTotalDurationPerUnit+fDurationPerUnit;
			UINT uActualDuration = uStart-uStartProcess;

			if(g_bIsLate){
				if(g_llDurationAllowed/10000<uActualDuration)
				{
					IsCut=true;
					break;
				}
			}

			if((float)uActualDuration < fTotalDurationPerUnit)
			{
				if(fTotalDurationPerUnit-(float)uActualDuration>=2)
				{
					UINT uSleepAdd=(UINT)fTotalDurationPerUnit-uActualDuration-1;
					Sleep(uSleepAdd);
					iAddedSleep++;
				}
			}
		
		}
		
		g_bIsLate=false;
		HeapFree(m_hHeap,0,pBuff);
		m_uLastSampleSent = timeGetTime();
	}
	HeapFree(m_hHeap, 0, pSampleInfo->pActualStartAddress);
	HeapFree(m_hHeap,0,(LPVOID)pSampleInfo);
}

void CModTimer::PrepareThread()
{
	CleanUpThread();
	m_psc->Lock();
	m_pFFTrans          = new FFTransform (m_iSampleRate, SAMPLE_BUFFER/(m_iChannels * (m_iBit/8)));
	m_psc->Unlock();
    m_pSampleIter       = new SampleIter();
}


void CModTimer::CleanUpThread()
{
	if(m_pFFTrans)
	{
		delete m_pFFTrans;
		m_pFFTrans=NULL;
	}
	if(m_pSampleIter)
	{
		delete m_pSampleIter;
		m_pSampleIter=NULL;
	}
}

//LRESULT CModTimer::PrepareThreadForBufferData(WPARAM wParam, LPARAM lParam)
void CModTimer::PrepareThreadForBufferData(WPARAM wParam, LPARAM lParam)
{
	//m_bIsPlaying=true;
	PrepareThread();
	m_bIsPlaying=true;
	//return 1;
}

//LRESULT CModTimer::CleanUpThreadfromBufferData(WPARAM wParam, LPARAM lParam)
void CModTimer::CleanUpThreadfromBufferData(WPARAM wParam, LPARAM lParam)
{
	//m_bIsPlaying=false;
	CleanUpThread();
	//return 1;
}

//LRESULT CModTimer::OnSampleSeekIsReady(WPARAM wParam, LPARAM lParam)
void CModTimer::OnSampleSeekIsReady(WPARAM wParam, LPARAM lParam)
{
	/*
	UINT uNow1=timeGetTime();
	static x=0;
	x++;
	int b=x;
	static bool IsDropped=false;
	bool bIsDropped = IsDropped;
	*/
	DWORD uSleep=0;
	AudioSampleMemory *pSampleInfo = (AudioSampleMemory *)lParam;
	m_psc->Lock();
	bool bIsPlaying=m_bIsPlaying;
	m_psc->Unlock();
	if(!bIsPlaying)
	{	
		HeapFree(m_hHeap,0,pSampleInfo->pActualStartAddress);
		HeapFree(m_hHeap,0,(LPVOID)pSampleInfo);
		return;
	}
	
	if(pSampleInfo->st>=0)
	{	
		LONGLONG llIndex = (LONGLONG)((timeGetTime()-pSampleInfo->uCurrentSystemTime)*10000);
		LONGLONG llVirtualMediaCurrentPositionFromLastRan = (pSampleInfo->llCurrentMediaPos) + llIndex - pSampleInfo->llMediaPosWhenGraphRun; 
		UINT uNow = timeGetTime();
	//	LONGLONG llActualMediaCurrentPositionFromLastRan = (LONGLONG)((uNow-pSampleInfo->uLastMediaRunTime)*10000);
	//	if(x==1)
	//		int h=0;
		if(llVirtualMediaCurrentPositionFromLastRan>pSampleInfo->ft)
		{
//			IsDropped=true;
			HeapFree(m_hHeap,0,pSampleInfo->pActualStartAddress);
			HeapFree(m_hHeap,0,(LPVOID)pSampleInfo);
			return;
		}
		
		if(pSampleInfo->st>llVirtualMediaCurrentPositionFromLastRan)
		{
			int iSize=HeapSize(m_hHeap,0,(LPVOID)pSampleInfo->pActualStartAddress);
			uSleep = (UINT)(((pSampleInfo->st) - (llVirtualMediaCurrentPositionFromLastRan))/10000);
			Sleep(uSleep);
		}
		else 
		{
			g_bIsLate=true;
			g_llDurationAllowed= pSampleInfo->ft-llVirtualMediaCurrentPositionFromLastRan;
		}
/*
		CFileLog log;
		CString s,h,u,b,c,d;
		log.SetAttributes(true,true);
		s.Format("sleep %d ",uSleep);	
		h.Format("pSampleInfo->st %d ",pSampleInfo->st);	
		u.Format("llVirtualMediaCurrentPositionFromLastRan %d ",llVirtualMediaCurrentPositionFromLastRan);		
		b.Format("llIndex %d ",llIndex);	
		c.Format("pSampleInfo->llCurrentMediaPos %d ",pSampleInfo->llCurrentMediaPos);	
		d.Format("pSampleInfo->llMediaPosWhenGraphRun %d ",pSampleInfo->llMediaPosWhenGraphRun);	

		log.WriteData(s + " " + h + " " + u + " " + b + " " + c + " " + d);
*/
		UpdateHistogram(pSampleInfo);
	}
	else
	{
		HeapFree(m_hHeap,0,pSampleInfo->pActualStartAddress);
		HeapFree(m_hHeap,0,(LPVOID)pSampleInfo);
	}

	//return 1;
}
/*
LRESULT CModTimer::MatamuAsu(WPARAM wParam, LPARAM lParam)
{
	UINT uNow=timeGetTime();
	AudioSampleMemory *pSampleInfo = (AudioSampleMemory *)lParam;
		static x=0;
	x++;
	int b=x;
	if(x==1)
		int j=0;
	return 1;
}
*/