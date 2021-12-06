; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CWinAmpAboutDlg
LastTemplate=CWinThread
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "WcxPlayer.h"

ClassCount=10
Class1=CWcxPlayerApp
Class2=CWcxPlayerDlg
Class3=CAboutDlg

ResourceCount=19
Resource1=IDR_MINMENU
Resource2=IDR_MAINFRAME
Resource3=IDD_ABOUTBOX2
Resource4=IDR_LISTMENU
Class4=CBigTime
Resource5=IDD_ADV
Class5=CInfoDlg
Resource6=IDD_ABOUTBOX
Class6=CWinAmpAboutDlg
Resource7=IDD_HELPDLG
Class7=CHelpDlg
Resource8=IDD_WCXPLAYER_DIALOG
Class8=CAdvDlg
Resource9=IDD_INFO
Class9=CSeekBarWnd
Class10=CModTimer
Resource10=IDR_ACCMAIN
Resource11=IDD_ABOUTBOX2 (English (U.S.))
Resource12=IDR_LISTMENU (English (U.S.))
Resource13=IDD_ADV (English (U.S.))
Resource14=IDD_ABOUTBOX (English (U.S.))
Resource15=IDR_MINMENU (English (U.S.))
Resource16=IDD_INFO (English (U.S.))
Resource17=IDD_WCXPLAYER_DIALOG (English (U.S.))
Resource18=IDD_HELPDLG (English (U.S.))
Resource19=IDR_ACCMAIN (English (U.S.))

[CLS:CWcxPlayerApp]
Type=0
HeaderFile=WcxPlayer.h
ImplementationFile=WcxPlayer.cpp
Filter=N

[CLS:CWcxPlayerDlg]
Type=0
HeaderFile=WcxPlayerDlg.h
ImplementationFile=WcxPlayerDlg.cpp
Filter=W
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CWcxPlayerDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=WcxPlayerDlg.h
ImplementationFile=WcxPlayerDlg.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=11
Control1=IDOK,button,1342373889
Control2=IDC_STATIC,static,1350565902
Control3=IDC_STATIC,static,1342308353
Control4=IDC_STATIC,static,1342308353
Control5=IDC_STATIC,static,1342308353
Control6=IDC_STATIC,static,1342308353
Control7=IDC_STATIC,static,1342308353
Control8=IDC_STATIC,static,1342308353
Control9=IDC_STATIC,static,1342308353
Control10=IDC_STATIC,static,1342308353
Control11=IDC_STATIC,static,1342308353

[DLG:IDD_WCXPLAYER_DIALOG]
Type=1
Class=CWcxPlayerDlg
ControlCount=7
Control1=IDC_BUTTON_PLAY,button,1342242816
Control2=IDC_BUTTON_STOP,button,1342242816
Control3=IDC_BUTTON_PAUSE,button,1342242816
Control4=IDC_PLAYLIST,SysListView32,1342259205
Control5=IDC_FAKE_EQU,button,1342210055
Control6=IDC_RUNTEXT,static,1342308353
Control7=IDC_TIME,static,1073872897

[MNU:IDR_LISTMENU]
Type=1
Class=?
Command1=IDC_DELETE
Command2=IDC_REMALL
Command3=IDC_GETINFO
CommandCount=3

[CLS:CBigTime]
Type=0
HeaderFile=BigTime.h
ImplementationFile=BigTime.cpp
BaseClass=CStatic
Filter=W
VirtualFilter=WC

[DLG:IDD_INFO]
Type=1
Class=CInfoDlg
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDC_PATH,static,1342308352
Control3=IDC_TITLE,static,1342308352
Control4=IDC_ARTIST,static,1342308352
Control5=IDC_SAMPLE,static,1342308352
Control6=IDC_BITRATE,static,1342308352
Control7=IDC_CHANNEL,static,1342308352

[CLS:CInfoDlg]
Type=0
HeaderFile=InfoDlg.h
ImplementationFile=InfoDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CInfoDlg
VirtualFilter=dWC

[ACL:IDR_ACCMAIN]
Type=1
Class=?
Command1=IDC_ABOUT
Command2=IDC_ADV
Command3=IDC_HELPD
Command4=IDC_MINIMIZETRAY
Command5=IDC_EXIT
CommandCount=5

[DLG:IDD_ABOUTBOX2]
Type=1
Class=CWinAmpAboutDlg
ControlCount=16
Control1=IDOK,button,1342373889
Control2=IDC_STATIC,static,1350565902
Control3=IDC_STATIC,static,1342308353
Control4=IDC_STATIC,static,1342308353
Control5=IDC_STATIC,static,1342308353
Control6=IDC_STATIC,static,1342308353
Control7=IDC_STATIC,static,1342308353
Control8=IDC_STATIC,static,1342308353
Control9=IDC_STATIC,static,1342308353
Control10=IDC_STATIC,static,1342308353
Control11=IDC_STATIC,static,1342308353
Control12=IDC_STATIC,static,1342308353
Control13=IDC_STATIC,static,1342308353
Control14=IDC_STATIC,static,1342308353
Control15=IDC_STATIC,static,1342308353
Control16=IDC_STATIC,static,1342308353

[CLS:CWinAmpAboutDlg]
Type=0
HeaderFile=WinAmpAboutDlg.h
ImplementationFile=WinAmpAboutDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CWinAmpAboutDlg
VirtualFilter=dWC

[CLS:CHelpDlg]
Type=0
HeaderFile=HelpDlg.h
ImplementationFile=HelpDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CHelpDlg
VirtualFilter=dWC

[DLG:IDD_HELPDLG]
Type=1
Class=CHelpDlg
ControlCount=1
Control1=IDOK,button,1342242817

[DLG:IDD_ADV]
Type=1
Class=CAdvDlg
ControlCount=0

[CLS:CAdvDlg]
Type=0
HeaderFile=AdvDlg.h
ImplementationFile=AdvDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CAdvDlg
VirtualFilter=dWC

[MNU:IDR_MINMENU]
Type=1
Class=?
Command1=IDC_RESTOREAPP
Command2=IDC_EXITMINIMIZE
CommandCount=2

[CLS:CSeekBarWnd]
Type=0
HeaderFile=SeekBarWnd.h
ImplementationFile=SeekBarWnd.cpp
BaseClass=CStatic
Filter=W
VirtualFilter=WC

[CLS:CModTimer]
Type=0
HeaderFile=ModTimer.h
ImplementationFile=ModTimer.cpp
BaseClass=CWinThread
Filter=N

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=11
Control1=IDOK,button,1342373889
Control2=IDC_STATIC,static,1350565902
Control3=IDC_STATIC,static,1342308353
Control4=IDC_STATIC,static,1342308353
Control5=IDC_STATIC,static,1342308353
Control6=IDC_STATIC,static,1342308353
Control7=IDC_STATIC,static,1342308353
Control8=IDC_STATIC,static,1342308353
Control9=IDC_STATIC,static,1342308353
Control10=IDC_STATIC,static,1342308353
Control11=IDC_STATIC,static,1342308353

[DLG:IDD_WCXPLAYER_DIALOG (English (U.S.))]
Type=1
Class=?
ControlCount=7
Control1=IDC_BUTTON_PLAY,button,1342242816
Control2=IDC_BUTTON_STOP,button,1342242816
Control3=IDC_BUTTON_PAUSE,button,1342242816
Control4=IDC_PLAYLIST,SysListView32,1342259205
Control5=IDC_FAKE_EQU,button,1342210055
Control6=IDC_RUNTEXT,static,1342308353
Control7=IDC_TIME,static,1073872897

[DLG:IDD_INFO (English (U.S.))]
Type=1
Class=?
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDC_PATH,static,1342308352
Control3=IDC_TITLE,static,1342308352
Control4=IDC_ARTIST,static,1342308352
Control5=IDC_SAMPLE,static,1342308352
Control6=IDC_BITRATE,static,1342308352
Control7=IDC_CHANNEL,static,1342308352

[DLG:IDD_ABOUTBOX2 (English (U.S.))]
Type=1
Class=CWinAmpAboutDlg
ControlCount=20
Control1=IDOK,button,1342373889
Control2=IDC_STATIC,static,1350565902
Control3=IDC_STATIC,static,1342308353
Control4=IDC_STATIC,static,1342308353
Control5=IDC_STATIC,static,1342308353
Control6=IDC_STATIC,static,1342308353
Control7=IDC_STATIC,static,1342308353
Control8=IDC_STATIC,static,1342308353
Control9=IDC_STATIC,static,1342308353
Control10=IDC_STATIC,static,1342308353
Control11=IDC_STATIC,static,1342308353
Control12=IDC_STATIC,static,1342308353
Control13=IDC_STATIC,static,1342308353
Control14=IDC_STATIC,static,1342308353
Control15=IDC_STATIC,static,1342308353
Control16=IDC_STATIC,static,1342308353
Control17=IDC_STATIC,static,1342308353
Control18=IDC_STATIC,static,1342308353
Control19=IDC_STATIC,static,1342308353
Control20=IDC_STATIC,static,1342308353

[DLG:IDD_HELPDLG (English (U.S.))]
Type=1
Class=?
ControlCount=1
Control1=IDOK,button,1342242817

[DLG:IDD_ADV (English (U.S.))]
Type=1
Class=?
ControlCount=0

[MNU:IDR_LISTMENU (English (U.S.))]
Type=1
Class=?
Command1=IDC_DELETE
Command2=IDC_REMALL
Command3=IDC_GETINFO
CommandCount=3

[MNU:IDR_MINMENU (English (U.S.))]
Type=1
Class=?
Command1=IDC_RESTOREAPP
Command2=IDC_EXITMINIMIZE
CommandCount=2

[ACL:IDR_ACCMAIN (English (U.S.))]
Type=1
Class=?
Command1=IDC_ABOUT
Command2=IDC_ADV
Command3=IDC_HELPD
Command4=IDC_MINIMIZETRAY
Command5=IDC_EXIT
CommandCount=5

