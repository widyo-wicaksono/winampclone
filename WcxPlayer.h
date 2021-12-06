// WcxPlayer.h : main header file for the WCXPLAYER application
//

#if !defined(AFX_WCXPLAYER_H__B32D21E8_478A_4399_B2D0_4ED0B44D27CB__INCLUDED_)
#define AFX_WCXPLAYER_H__B32D21E8_478A_4399_B2D0_4ED0B44D27CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CWcxPlayerApp:
// See WcxPlayer.cpp for the implementation of this class
//


class CWcxPlayerApp : public CWinApp
{
public:
	CWcxPlayerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWcxPlayerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWcxPlayerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WCXPLAYER_H__B32D21E8_478A_4399_B2D0_4ED0B44D27CB__INCLUDED_)
