// EquFilter.h: interface for the CEquFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EQUFILTER_H__25D6CA8C_914A_4324_8B28_1040CD114ED7__INCLUDED_)
#define AFX_EQUFILTER_H__25D6CA8C_914A_4324_8B28_1040CD114ED7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <streams.h>


struct __declspec(uuid("{41216f09-33cf-415d-915c-cd08a35453de}")) CLSID_EquFilter;

//DEFINE_GUID(CLSID_EquFilter,0x41216f09, 0x33cf, 0x415d, 0x91, 0x5c, 0xcd, 0x08, 0xa3, 0x54, 0x53, 0xde);



class CEquFilter  : public CTransInPlaceFilter
{
public:
	void CopySampleData(LPSTR pBuff);
	CEquFilter(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr);
	virtual ~CEquFilter();
	HRESULT CheckInputType(const CMediaType* mtIn);
    HRESULT Transform(IMediaSample *pMediaSample);
	HWND	m_HMainWnd;
	WAVEFORMATEX m_WaveFmt;
	HANDLE m_hSampleHeap;
private:

	virtual HRESULT SetMediaType(PIN_DIRECTION direction,const CMediaType *pmt);
	CCritSec            m_Lock;
	int                 m_SamplesPerSec;       // Current sample format
    int                 m_BytesPerSample;      // Current sample format
    int                 m_Channels;  
	BYTE m_bSampleBuffer[512];
	WORD m_wbSampleBuffer[256];
	int m_iBufferIndex;
	UINT m_iLastDataSent;
};

#endif // !defined(AFX_EQUFILTER_H__25D6CA8C_914A_4324_8B28_1040CD114ED7__INCLUDED_)
