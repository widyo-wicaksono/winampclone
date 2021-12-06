// EquFilter.cpp: implementation of the CEquFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WcxPlayer.h"
#include "EquFilter.h"
#include "FileLog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEquFilter::CEquFilter(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr)
	: CTransInPlaceFilter (tszName, punk, __uuidof(CLSID_EquFilter), phr)
{
	m_iBufferIndex=0;
	m_iLastDataSent = timeGetTime();
}

CEquFilter::~CEquFilter()
{
	int i=0;
}

HRESULT CEquFilter::CheckInputType(const CMediaType* mtIn)
{
	if (IsEqualGUID(*mtIn->Type(), MEDIATYPE_Audio)) 
	{
        if (IsEqualGUID(*mtIn->Subtype(), MEDIASUBTYPE_PCM)) 
		{
			return S_OK;
        }
	}
	return VFW_E_TYPE_NOT_ACCEPTED;
}
HRESULT CEquFilter::Transform(IMediaSample *pMediaSample)
{
	{
		CAutoLock foo(&m_Lock);

		BYTE *pSampleBuffer;
		size_t iSize = (size_t)pMediaSample->GetActualDataLength();
		pMediaSample->GetPointer(&pSampleBuffer);
		LPVOID pHeap = HeapAlloc(m_hSampleHeap, HEAP_ZERO_MEMORY, iSize);
		CopyMemory(pHeap, pSampleBuffer, iSize);
		REFERENCE_TIME rt1;
		REFERENCE_TIME rt2;
		HRESULT hr =pMediaSample->GetTime(&rt1,&rt2);
				
		LPVOID pSampleInfo = HeapAlloc(m_hSampleHeap, HEAP_ZERO_MEMORY, sizeof(SampleInfo));
		SampleInfo * pSI = (SampleInfo *)pSampleInfo;
		pSI->lStartTime = rt1;
		pSI->lFinishTime= rt2;		

		PostMessage(m_HMainWnd, WM_SAMPLEISREADY, (WPARAM)pSampleInfo, (LPARAM)pHeap);		
	}
	return S_OK;
}

HRESULT CEquFilter::SetMediaType(PIN_DIRECTION direction,const CMediaType *pmt)
{
	CheckPointer(pmt,E_POINTER);
	m_WaveFmt = *(WAVEFORMATEX *) pmt->Format();
    CTransInPlaceFilter::SetMediaType(direction, pmt);

    // Reconnect where necessary.
    if( m_pInput->IsConnected() && m_pOutput->IsConnected() )
    {
        FILTER_INFO fInfo;

        QueryFilterInfo( &fInfo );

        if (direction == PINDIR_OUTPUT && *pmt != m_pInput->CurrentMediaType() )
            fInfo.pGraph->Reconnect( m_pInput );

        QueryFilterInfoReleaseGraph( fInfo );

        ASSERT(!(direction == PINDIR_INPUT && *pmt != m_pOutput->CurrentMediaType()));
    }
    return NOERROR;
}

void CEquFilter::CopySampleData(LPSTR pBuff)
{
	CopyMemory(pBuff,m_wbSampleBuffer,512);
}
