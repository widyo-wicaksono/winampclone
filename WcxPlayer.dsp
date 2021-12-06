# Microsoft Developer Studio Project File - Name="WcxPlayer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WcxPlayer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WcxPlayer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WcxPlayer.mak" CFG="WcxPlayer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WcxPlayer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WcxPlayer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WcxPlayer - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib strmbase.lib comsupp.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "WcxPlayer - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib STRMBASD.lib comsupp.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "WcxPlayer - Win32 Release"
# Name "WcxPlayer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AdvDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BigTime.cpp
# End Source File
# Begin Source File

SOURCE=.\DShowMP3.cpp
# End Source File
# Begin Source File

SOURCE=.\EquFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\FakeEqu.cpp
# End Source File
# Begin Source File

SOURCE=.\FFTransform.cpp
# End Source File
# Begin Source File

SOURCE=.\FileLog.cpp
# End Source File
# Begin Source File

SOURCE=.\HelpDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\RuningText.cpp
# End Source File
# Begin Source File

SOURCE=.\SampleBufferManager.cpp
# End Source File
# Begin Source File

SOURCE=.\SeekBarWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinVerticleScrollbar.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\VolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\WcxPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\WcxPlayer.rc
# End Source File
# Begin Source File

SOURCE=.\WcxPlayerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WinAmpAboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MP3\Wmp3.cpp
# End Source File
# Begin Source File

SOURCE=.\MP3\wmp3decoder.cpp
# End Source File
# Begin Source File

SOURCE=.\MP3\wstream.cpp
# End Source File
# Begin Source File

SOURCE=.\MP3\wwave.cpp
# End Source File
# Begin Source File

SOURCE=.\xSkinButton.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AdvDlg.h
# End Source File
# Begin Source File

SOURCE=.\BigTime.h
# End Source File
# Begin Source File

SOURCE=.\DShowMP3.h
# End Source File
# Begin Source File

SOURCE=.\EquFilter.h
# End Source File
# Begin Source File

SOURCE=.\FakeEqu.h
# End Source File
# Begin Source File

SOURCE=.\FFTransform.h
# End Source File
# Begin Source File

SOURCE=.\FileLog.h
# End Source File
# Begin Source File

SOURCE=.\HelpDlg.h
# End Source File
# Begin Source File

SOURCE=.\InfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\MEMDC.H
# End Source File
# Begin Source File

SOURCE=.\ModTimer.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RuningText.h
# End Source File
# Begin Source File

SOURCE=.\SampleBufferManager.h
# End Source File
# Begin Source File

SOURCE=.\SampleIter.h
# End Source File
# Begin Source File

SOURCE=.\SeekBarWnd.h
# End Source File
# Begin Source File

SOURCE=.\SkinListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SkinVerticleScrollbar.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\VolBar.h
# End Source File
# Begin Source File

SOURCE=.\WcxPlayer.h
# End Source File
# Begin Source File

SOURCE=.\WcxPlayerDlg.h
# End Source File
# Begin Source File

SOURCE=.\WinAmpAboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\MP3\wmp3.h
# End Source File
# Begin Source File

SOURCE=.\MP3\wmp3decoder.h
# End Source File
# Begin Source File

SOURCE=.\MP3\wstream.h
# End Source File
# Begin Source File

SOURCE=.\MP3\wwave.h
# End Source File
# Begin Source File

SOURCE=.\xSkinButton.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\0.bmp
# End Source File
# Begin Source File

SOURCE=.\res\1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\7.bmp
# End Source File
# Begin Source File

SOURCE=.\res\8.bmp
# End Source File
# Begin Source File

SOURCE=.\res\9.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AdvCtrl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AdvCtrl2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ARISA.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BaseSkin.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BaseSkin2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BaseSkin3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BaseSkin4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BaseSkin5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BaseSkin6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap10.bmp
# End Source File
# Begin Source File

SOURCE=.\res\HELP1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\HELP2.bmp
# End Source File
# Begin Source File

SOURCE=.\image.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Mp3INFO.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PauseBtnFcs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PauseBtnNrm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PauseBtnPrs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PlayBtnFcs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PlayBtnMsk.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PlayBtnMsk2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PlayBtnMsk3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PlayBtnNrm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PlayBtnPrs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SEEKBACK.bmp
# End Source File
# Begin Source File

SOURCE=.\res\seekbuton.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SEEKTHUMB.bmp
# End Source File
# Begin Source File

SOURCE=.\res\seekvolume.bmp
# End Source File
# Begin Source File

SOURCE=.\res\slidervolume.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sound.ico
# End Source File
# Begin Source File

SOURCE=.\res\StopBtnFcs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\StopBtnNrm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\StopBtnPrs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\titik2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\VerticleScrollBarSpan.bmp
# End Source File
# Begin Source File

SOURCE=.\res\VerticleScrollBarSpan2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\VerticleScrollBarThumb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\VolSlide1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\VolSlide2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\VolThumb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\WcxPlayer.ico
# End Source File
# Begin Source File

SOURCE=.\res\WcxPlayer.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
