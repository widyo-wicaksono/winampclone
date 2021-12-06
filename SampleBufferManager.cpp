// SampleBufferManager.cpp: implementation of the CSampleBufferManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WcxPlayer.h"
#include "SampleBufferManager.h"
#include "FileLog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSampleBufferManager::CSampleBufferManager()
{
	m_iReadSize=SAMPLE_BUFFER;
	m_pBuffer = NULL;
	m_dwCurrentVirtualSampleAddress = 0;
	m_iBufferSize=0;
	m_dwSampleCursor=0;
}

CSampleBufferManager::~CSampleBufferManager()
{

}

void CSampleBufferManager::AddNewBuffer(LPVOID pBuff,LONGLONG rt)
{
	AudioSampleMemory as;
	as.pActualStartAddress = pBuff;
	as.iSize = HeapSize(m_hHeap,0,pBuff);
	as.dwStartAddress = (DWORD)m_iBufferSize;
	m_iBufferSize = m_iBufferSize + as.iSize; 	
	as.dwEndAddress = as.dwStartAddress+(DWORD)as.iSize-1;
	as.st = rt;
	m_BufferList.push_back(as);	

}

void CSampleBufferManager::FreeAllAllocatedBuffer()
{
	list<AudioSampleMemory>::iterator iter = m_BufferList.begin();
	while(iter!=m_BufferList.end())
	{
		if(!HeapFree(m_hHeap,0,iter->pActualStartAddress))
		{
			MessageBox(NULL,"Failed to free heap","Error",MB_OK);
		}		
		iter++;
	}
	while(!m_BufferList.empty())
	{
		m_BufferList.pop_front();
	}
	m_iBufferSize=0;
}

//DEL HRESULT CSampleBufferManager::AllocateNewBuffer()
//DEL {
//DEL 	
//DEL }


//DEL LPVOID CSampleBufferManager::ReadSampleBuffer(int iIndex)
//DEL {
//DEL 	return NULL;
//DEL }

void CSampleBufferManager::FreeBuffer()
{
	if(m_pBuffer)
	{
		if(!HeapFree(m_hHeap,0,m_pBuffer))
		{
			MessageBox(NULL,"Failed to free heap","Error",MB_OK);
		}
		m_pBuffer=NULL;
	}
}

LPVOID CSampleBufferManager::ReadSampleBuffer(DWORD dwVirtualAddr)
{
	bool bIsFound=false;
	LPVOID pSampleBuffer=HeapAlloc(m_hHeap,HEAP_ZERO_MEMORY,m_iReadSize);
	if(pSampleBuffer)
	{
		m_dwCurrentVirtualSampleAddress=dwVirtualAddr;
		list<AudioSampleMemory>::iterator iter = m_BufferList.begin();
		while(iter!=m_BufferList.end())
		{
			if(m_dwCurrentVirtualSampleAddress>=iter->dwStartAddress && m_dwCurrentVirtualSampleAddress <= iter->dwEndAddress)
			{
				if(m_dwCurrentVirtualSampleAddress+(m_iReadSize-1)<=iter->dwEndAddress)
				{
					PBYTE pAddress = (PBYTE)iter->pActualStartAddress + (dwVirtualAddr - iter->dwStartAddress);
					CopyMemory(pSampleBuffer,(LPVOID)pAddress,m_iReadSize);
					bIsFound=true;
					break;
				}				
				else
				{
					
				}
			}
			iter++;
		}
	}
	/*if(pSampleBuffer)
	{
		DWORD dSize=HeapSize(m_hHeap,0,m_pBuffer)+(UINT)m_pBuffer;
		UINT i = (UINT)pBuffer+m_iReadSize;
		if(i<=dSize)
			CopyMemory(pSampleBuffer,pBuffer,m_iReadSize);
		else if((DWORD)pBuffer<=dSize)
			CopyMemory(pSampleBuffer,pBuffer,m_iReadSize-(i-dSize));
	}
	
	*/
	if(!bIsFound)
	{
		CFileLog log;
		log.SetAttributes(true,true);
		log.WriteData("Out of range");
	}
	return pSampleBuffer;
}

void CSampleBufferManager::SetNewSampleBuffer(LPVOID pVoid)
{
	m_pBuffer=pVoid;
}

//DEL void CSampleBufferManager::FreeSampleBuffer()
//DEL {
//DEL 
//DEL }

LPVOID CSampleBufferManager::GetBuffer()
{
	return m_pBuffer;
}

void CSampleBufferManager::FreeFirslBufferInList()
{
	list<AudioSampleMemory>::iterator iter = m_BufferList.begin();
	HeapFree(m_hHeap,0, iter->pActualStartAddress);
	m_BufferList.pop_front();
}

void CSampleBufferManager::FreeUsedSampleBuffer()
{
	CFileLog log;
	log.SetAttributes(true,true);
	list<AudioSampleMemory>::iterator iter = m_BufferList.begin();
	int i=0;
	while(iter!=m_BufferList.end())
	{
		if(m_dwCurrentVirtualSampleAddress>=iter->dwStartAddress && m_dwCurrentVirtualSampleAddress <= iter->dwEndAddress)
		{
			break;
		}
		else if(m_dwCurrentVirtualSampleAddress>iter->dwEndAddress)
		{
			if(!HeapFree(m_hHeap,0,iter->pActualStartAddress))
			{
				MessageBox(NULL,"Failed to free sample buffer","Error",MB_OK);
			}
			else
			{
				CString s,h;
				s.Format("Freed buffer with vaddress %d",iter->dwStartAddress);
				h.Format(" to %d", iter->dwEndAddress);
				s=s+h;
				log.WriteData(s);
			}
		}
		i++;
		iter++;
	}
	for(int j=0;j<i;j++)
	{
		m_BufferList.pop_front();
	}
}
