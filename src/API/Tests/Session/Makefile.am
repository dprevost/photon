# Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# Include this at the beginning to allow the current Makefile.am to
# overwrite any macros set in the common file
include $(top_srcdir)/src/Makefile.Common.am

CLEANFILES = *~ core

check_PROGRAMS =            \
        CloseNullSession    \
        Close               \
        CloseObjNullObject  \
        CloseObjNullSession \
        CloseObjPass        \
        Commit              \
        CreateFolder        \
        CreateMap           \
        CreateQueue         \
        DataDefinition      \
        Definition          \
        Destroy             \
        ErrorMsg            \
        ExitPass            \
        ExitWithOpenObjects \
        GetInfo             \
        Init                \
        KeyDefinition       \
        LastError           \
        OpenObjNotCreated   \
        OpenObjNullName     \
        OpenObjNullObject   \
        OpenObjNullSession  \
        OpenObjPass         \
        OpenObjWrongType    \
        OpenObjZeroLength   \
        Rollback            \
        Status              

CloseNullSession_SOURCES    = CloseNullSession.c
Close_SOURCES               = Close.c
CloseObjNullObject_SOURCES  = CloseObjNullObject.c
CloseObjNullSession_SOURCES = CloseObjNullSession.c
CloseObjPass_SOURCES        = CloseObjPass.c
Commit_SOURCES              = Commit.c
CreateFolder_SOURCES        = CreateFolder.c
CreateMap_SOURCES           = CreateMap.c
CreateQueue_SOURCES         = CreateQueue.c
DataDefinition_SOURCES      = DataDefinition.c
Definition_SOURCES          = Definition.c
Destroy_SOURCES             = Destroy.c
ErrorMsg_SOURCES            = ErrorMsg.c
ExitPass_SOURCES            = ExitPass.c
ExitWithOpenObjects_SOURCES = ExitWithOpenObjects.c
GetInfo_SOURCES             = GetInfo.c
Init_SOURCES                = Init.c
KeyDefinition_SOURCES       = KeyDefinition.c
LastError_SOURCES           = LastError.c
OpenObjNotCreated_SOURCES   = OpenObjNotCreated.c
OpenObjNullName_SOURCES     = OpenObjNullName.c
OpenObjNullObject_SOURCES   = OpenObjNullObject.c
OpenObjNullSession_SOURCES  = OpenObjNullSession.c
OpenObjPass_SOURCES         = OpenObjPass.c
OpenObjWrongType_SOURCES    = OpenObjWrongType.c
OpenObjZeroLength_SOURCES   = OpenObjZeroLength.c
Rollback_SOURCES            = Rollback.c
Status_SOURCES              = Status.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.sh    \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        tcp_port=10701               \
        top_srcdir=$(top_srcdir)     \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS = RunTests.sh

XFAIL_TESTS =                  

LDADD = $(top_builddir)/src/API/libphoton.la      \
        $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la   

