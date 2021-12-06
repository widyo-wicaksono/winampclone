// SampleBufferManager.h: interface for the CSampleBufferManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLEBUFFERMANAGER_H__8A90C66D_843A_4DB4_B1EF_C593BF520480__INCLUDED_)
#define AFX_SAMPLEBUFFERMANAGER_H__8A90C66D_843A_4DB4_B1EF_C593BF520480__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
using namespace std;

class CSampleBufferManager  
{
public:
	void FreeUsedSampleBuffer();
	void FreeFirslBufferInList();
	LPVOID GetBuffer();
	void SetNewSampleBuffer(LPVOID pVoid);
	LPVOID ReadSampleBuffer(DWORD dwVirtualAddr);
	void FreeBuffer();
	void AddNewBuffer(LPVOID pBuff,LONGLONG rt);
	HANDLE m_hHeap;
	void FreeAllAllocatedBuffer();
	CSampleBufferManager();
	virtual ~CSampleBufferManager();
private:
	list<AudioSampleMemory> m_BufferList;
	int m_iReadSize;
	int m_iBufferSize;
	LPVOID m_pBuffer;
	DWORD m_dwCurrentVirtualSampleAddress;
	DWORD m_dwSampleCursor;
};

#endif // !defined(AFX_SAMPLEBUFFERMANAGER_H__8A90C66D_843A_4DB4_B1EF_C593BF520480__INCLUDED_)
