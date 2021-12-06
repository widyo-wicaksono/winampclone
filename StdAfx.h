// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__0A4B2AF3_D03D_4F42_8494_601AB267FA37__INCLUDED_)
#define AFX_STDAFX_H__0A4B2AF3_D03D_4F42_8494_601AB267FA37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxmt.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define INVALID_SET_FILE_POINTER ((DWORD)-1)


struct MP3ID3V1TAG
{
	char ident[3]; // TAG
	char title[30];
	char artist[30];
	char album[30];
	char year[4];
	char comment[28];
	BYTE reserved;
	BYTE tracknum;
	BYTE genre;
};

struct MP3Data
{
	char sPath[MAX_PATH];
	char sTitle[256];
	char sFileName[256];
	UINT iDuration;
};

struct AudioSampleMemory
{
	DWORD dwStartAddress;
	DWORD dwEndAddress;
	LPVOID pActualStartAddress;
	LONGLONG st;
	LONGLONG ft;
	LONGLONG llCurrentMediaPos;
	LONGLONG llMediaPosWhenGraphRun;
	UINT uCurrentSystemTime;
	UINT uLastMediaRunTime;
	int iSize;
};

struct SampleInfo
{
	LONGLONG lStartTime;
	LONGLONG lFinishTime;
};



#define WM_GRAPHNOTIFY  WM_USER+13
#define WM_STOP_ANIM_FINISHED  WM_USER+1
#define WM_CHILD_EXIT  WM_USER+2
#define WM_ABOUT_EXIT  WM_USER+3
#define WM_HELP_EXIT  WM_USER+4
#define WM_ADV_EXIT  WM_USER+5
#define WM_MY_TRAYMESSAGE WM_USER+6
#define WM_IDLE WM_USER + 7
#define WM_SAMPLEISREADY WM_USER + 8
#define WM_UPDATE_HISTOGRAM WM_USER + 9
#define WM_MEDIA_PLAY WM_USER + 10
#define WM_MEDIA_STOP WM_USER + 11
#define WM_SAMPLESEEKEDISREADY WM_USER + 12
#define WM_BEGIN_SEEK WM_USER + 14
#define WM_END_SEEK WM_USER + 15
#define WM_SAMPLESEEKEDISREADY_2 WM_USER + 16

#ifndef LWA_COLORKEY
# define LWA_COLORKEY 1
#endif
#ifndef LWA_ALPHA
# define LWA_ALPHA 2
#endif
#ifndef WS_EX_LAYERED 
# define WS_EX_LAYERED 0x80000
#endif
#define SAMPLE_BUFFER 512
#define HISTOGRAM_PROCESS_INTERVAL 1
#define SPECSCOPEWIDTH 10
#define HISTOGRAM_COUNT 10
#define MAX_VALUE_NAME 16383
#define MAX_KEY_LENGTH 255


struct HistogramValue
{
	int h[HISTOGRAM_COUNT];
};

typedef BOOL (WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, 
                                  COLORREF crKey, BYTE bAlpha, DWORD dwFlags);


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__0A4B2AF3_D03D_4F42_8494_601AB267FA37__INCLUDED_)

