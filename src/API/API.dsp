# Microsoft Developer Studio Project File - Name="API" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=API - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "API.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "API.mak" CFG="API - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "API - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "API - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "API - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BUILD_VDSF_API" /D "BUILD_VDSF" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I ".." /I "..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BUILD_VDSF_API" /D "BUILD_VDSF" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib vdsfCommon.lib vdsfEngine.lib ws2_32.lib libxml2.lib /nologo /dll /machine:I386 /out:"Release/vdsf.dll" /libpath:"..\Release"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=if not exist ..\Release\ mkdir ..\Release	if exist ..\Release\vdsf.lib del ..\Release\vdsf.lib	if exist ..\Release\vdsf.dll del ..\Release\vdsf.dll
PostBuild_Cmds=copy Release\vdsf.lib ..\Release	copy Release\vdsf.dll ..\Release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "API - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BUILD_VDSF_API" /D "BUILD_VDSF" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I ".." /I "..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BUILD_VDSF_API" /D "BUILD_VDSF" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib vdsfCommon.lib vdsfEngine.lib ws2_32.lib libxml2.lib /nologo /dll /debug /machine:I386 /out:"Debug/vdsf.dll" /pdbtype:sept /libpath:"..\Debug"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=if not exist ..\Debug\ mkdir ..\Debug	if exist ..\Debug\vdsf.lib del ..\Debug\vdsf.lib	if exist ..\Debug\vdsf.dll del ..\Debug\vdsf.dll
PostBuild_Cmds=copy Debug\vdsf.lib ..\Debug	copy Debug\vdsf.dll ..\Debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "API - Win32 Release"
# Name "API - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\api.c
# End Source File
# Begin Source File

SOURCE=.\CommonObject.c
# End Source File
# Begin Source File

SOURCE=.\Connector.c
# End Source File
# Begin Source File

SOURCE=.\DataDefinition.c
# End Source File
# Begin Source File

SOURCE=.\Folder.c
# End Source File
# Begin Source File

SOURCE=.\HashMap.c
# End Source File
# Begin Source File

SOURCE=.\Lifo.c
# End Source File
# Begin Source File

SOURCE=.\Map.c
# End Source File
# Begin Source File

SOURCE=.\Process.c
# End Source File
# Begin Source File

SOURCE=.\Queue.c
# End Source File
# Begin Source File

SOURCE=.\Session.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\api.h
# End Source File
# Begin Source File

SOURCE=.\CommonObject.h
# End Source File
# Begin Source File

SOURCE=.\Connector.h
# End Source File
# Begin Source File

SOURCE=.\DataDefinition.h
# End Source File
# Begin Source File

SOURCE=.\Folder.h
# End Source File
# Begin Source File

SOURCE=.\Lifo.h
# End Source File
# Begin Source File

SOURCE=.\ListReaders.h
# End Source File
# Begin Source File

SOURCE=.\Map.h
# End Source File
# Begin Source File

SOURCE=.\Process.h
# End Source File
# Begin Source File

SOURCE=.\Session.h
# End Source File
# Begin Source File

SOURCE=.\WatchdogCommon.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
