// FileLog.cpp: implementation of the CFileLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileLog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileLog::CFileLog()
{
	m_bIsLogged=false;
	m_bIsDateLogged=false;
	char StrFP[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, StrFP);
//	SHGetSpecialFolderPath(NULL,StrFP,35,0);
	//SHGetFolderPath();
	m_cPath=StrFP;
}

CFileLog::~CFileLog()
{
	/*
	if(m_bIsLogged)
	{
		m_cFile.Close();
	}
	*/
}

HRESULT CFileLog::WriteData(CString psStr)
{
	CFileException e;
	if(m_bIsLogged)
	{
		CTime cWaktu = CTime::GetCurrentTime();
		CString day;
		CString month;
		CString year;

		day.Format("%d",cWaktu.GetDay());
		month.Format("%d",cWaktu.GetMonth());
		year.Format("%d",cWaktu.GetYear());

		CString sFName = "WinAmpUn" + year + month + day +".LOG";		
		if(m_cFile.Open(sFName,CFile::shareDenyNone|CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite,&e))
		{
		}
		else
		{
			e.ReportError();
			if(!m_cFile.Open(sFName,CFile::shareDenyNone|CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite,&e))
			{
				e.ReportError();
				return E_FAIL;
			}
		}

		if(m_bIsDateLogged)
		{
			CTime cWaktu = CTime::GetCurrentTime();			

			CString hour;
			CString min;
			CString sec;

			hour.Format("%d",cWaktu.GetHour());
			min.Format("%d",cWaktu.GetMinute());
			sec.Format("%d",cWaktu.GetSecond());

			psStr=hour  + ":" + min + ":" + sec + " " + psStr;
		}
		psStr=psStr + "\r\n";
		try
		{
			m_cFile.SeekToEnd();
			m_cFile.Write(psStr, psStr.GetLength());
		}
		//catch (CFileException e)
		catch(...)
		{
			//e.ReportError();
		}
		m_cFile.Close();
	}
	return S_OK;
}

void CFileLog::SetAttributes(bool pLog, bool pDate)
{
	m_bIsLogged = pLog;
	m_bIsDateLogged = pDate;
}
