// FileLog.h: interface for the CFileLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILELOG_H__6091173A_3F56_479C_8A72_51959D380F8E__INCLUDED_)
#define AFX_FILELOG_H__6091173A_3F56_479C_8A72_51959D380F8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileLog  
{
public:
	void SetAttributes(bool pLog, bool pDate);
	HRESULT WriteData(CString psStr);
	CFileLog();
	virtual ~CFileLog();

private:
	CFile m_cFile;
	bool m_bIsLogged;
	bool m_bIsDateLogged;
	CString m_cPath;
};

#endif // !defined(AFX_FILELOG_H__6091173A_3F56_479C_8A72_51959D380F8E__INCLUDED_)
