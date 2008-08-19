# Microsoft Developer Studio Project File - Name="Engine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Engine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Engine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Engine.mak" CFG="Engine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Engine - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Engine - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Engine - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BUILD_VDSF_ENGINE" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "..\Common" /I ".." /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BUILD_VDSF_ENGINE" /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib vdsfCommon.lib /nologo /dll /machine:I386 /out:"Release/vdsfEngine.dll" /libpath:"..\Release"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=if not exist ..\Release\ mkdir ..\Release	if exist ..\Release\vdsfEngine.lib del ..\Release\vdsfEngine.lib	if exist ..\Release\vdsfEngine.dll del ..\Release\vdsfEngine.dll
PostBuild_Cmds=copy Release\vdsfEngine.lib ..\Release	copy Release\vdsfEngine.dll ..\Release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BUILD_VDSF_ENGINE" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\Common" /I ".." /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BUILD_VDSF_ENGINE" /FD /GZ /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib vdsfCommon.lib /nologo /dll /debug /machine:I386 /out:"Debug/vdsfEngine.dll" /pdbtype:sept /libpath:"..\Debug"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=if not exist ..\Debug\ mkdir ..\Debug	if exist ..\Debug\vdsfEngine.lib del ..\Debug\vdsfEngine.lib	if exist ..\Debug\vdsfEngine.dll del ..\Debug\vdsfEngine.dll
PostBuild_Cmds=copy Debug\vdsfEngine.lib ..\Debug	copy Debug\vdsfEngine.dll ..\Debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Engine - Win32 Release"
# Name "Engine - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BlockGroup.c
# End Source File
# Begin Source File

SOURCE=.\Folder.c
# End Source File
# Begin Source File

SOURCE=.\Hash.c
# End Source File
# Begin Source File

SOURCE=.\hash_fnv.c
# End Source File
# Begin Source File

SOURCE=.\HashMap.c
# End Source File
# Begin Source File

SOURCE=.\InitEngine.c
# End Source File
# Begin Source File

SOURCE=.\LinkedList.c
# End Source File
# Begin Source File

SOURCE=.\LogFile.c
# End Source File
# Begin Source File

SOURCE=.\Map.c
# End Source File
# Begin Source File

SOURCE=.\MemBitmap.c
# End Source File
# Begin Source File

SOURCE=.\MemoryAllocator.c
# End Source File
# Begin Source File

SOURCE=.\MemoryObject.c
# End Source File
# Begin Source File

SOURCE=.\Process.c
# End Source File
# Begin Source File

SOURCE=.\ProcessManager.c
# End Source File
# Begin Source File

SOURCE=.\Queue.c
# End Source File
# Begin Source File

SOURCE=.\Session.c
# End Source File
# Begin Source File

SOURCE=.\Transaction.c
# End Source File
# Begin Source File

SOURCE=.\VdsErrorHandler.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BlockGroup.h
# End Source File
# Begin Source File

SOURCE=.\Engine.h
# End Source File
# Begin Source File

SOURCE=.\Folder.h
# End Source File
# Begin Source File

SOURCE=.\Hash.h
# End Source File
# Begin Source File

SOURCE=.\hash_fnv.h
# End Source File
# Begin Source File

SOURCE=.\HashMap.h
# End Source File
# Begin Source File

SOURCE=.\InitEngine.h
# End Source File
# Begin Source File

SOURCE=.\LinkedList.h
# End Source File
# Begin Source File

SOURCE=.\LinkedList.inl
# End Source File
# Begin Source File

SOURCE=.\LinkNode.h
# End Source File
# Begin Source File

SOURCE=.\LogFile.h
# End Source File
# Begin Source File

SOURCE=.\Map.h
# End Source File
# Begin Source File

SOURCE=.\MemBitmap.h
# End Source File
# Begin Source File

SOURCE=.\MemBitmap.inl
# End Source File
# Begin Source File

SOURCE=.\MemoryAllocator.h
# End Source File
# Begin Source File

SOURCE=.\MemoryHeader.h
# End Source File
# Begin Source File

SOURCE=.\MemoryObject.h
# End Source File
# Begin Source File

SOURCE=.\Process.h
# End Source File
# Begin Source File

SOURCE=.\ProcessManager.h
# End Source File
# Begin Source File

SOURCE=.\Queue.h
# End Source File
# Begin Source File

SOURCE=.\Session.h
# End Source File
# Begin Source File

SOURCE=.\SessionContext.h
# End Source File
# Begin Source File

SOURCE=.\Transaction.h
# End Source File
# Begin Source File

SOURCE=.\TreeNode.h
# End Source File
# Begin Source File

SOURCE=.\TxStatus.h
# End Source File
# Begin Source File

SOURCE=.\VdsErrorHandler.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
