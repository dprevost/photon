# Microsoft Developer Studio Project File - Name="GenerateError" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Generic Project" 0x010a

CFG=GenerateError - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GenerateError.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GenerateError.mak" CFG="GenerateError - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GenerateError - Win32 Release" (based on "Win32 (x86) Generic Project")
!MESSAGE "GenerateError - Win32 Debug" (based on "Win32 (x86) Generic Project")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
MTL=midl.exe

!IF  "$(CFG)" == "GenerateError - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GenerateError___Win32_Release"
# PROP BASE Intermediate_Dir "GenerateError___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "GenerateError - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GenerateError___Win32_Debug"
# PROP BASE Intermediate_Dir "GenerateError___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "GenerateError - Win32 Release"
# Name "GenerateError - Win32 Debug"
# Begin Source File

SOURCE=..\include\vdsf\vdsErrors.h

!IF  "$(CFG)" == "GenerateError - Win32 Release"

# Begin Custom Build
InputPath=..\include\vdsf\vdsErrors.h

"VdsErrorHandler.c VdsErrorHandler.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\Release\errorParser -i $(InputPath) -o VdsErrorHandler -p vdseErr

# End Custom Build

!ELSEIF  "$(CFG)" == "GenerateError - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\include\vdsf\vdsErrors.h

BuildCmds= \
	..\Debug\errorParser -i $(InputPath) -o VdsErrorHandler -p vdseErr

"VdsErrorHandler.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"VdsErrorHandler.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
