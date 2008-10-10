# Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

AM_LDFLAGS = @LIBXML2_LIBS@

check_PROGRAMS =            \
        CloseNullSession    \
        ClosePass           \
        CloseObjNullObject  \
        CloseObjNullSession \
        CloseObjPass        \
        CommitPass          \
        CreatePass          \
        CreateQueue         \
        DestroyPass         \
        ErrorMsg            \
        ExitPass            \
        ExitWithOpenObjects \
        GetInfo             \
        InitPass            \
        LastError           \
        OpenObjNotCreated   \
        OpenObjNullName     \
        OpenObjNullObject   \
        OpenObjNullSession  \
        OpenObjPass         \
        OpenObjWrongType    \
        OpenObjZeroLength   \
        RollbackPass        \
        StatusPass          

CloseNullSession_SOURCES    = CloseNullSession.c
ClosePass_SOURCES           = ClosePass.c
CloseObjNullObject_SOURCES  = CloseObjNullObject.c
CloseObjNullSession_SOURCES = CloseObjNullSession.c
CloseObjPass_SOURCES        = CloseObjPass.c
CommitPass_SOURCES          = CommitPass.c
CreatePass_SOURCES          = CreatePass.c
CreateQueue_SOURCES         = CreateQueue.c
DestroyPass_SOURCES         = DestroyPass.c
ErrorMsg_SOURCES            = ErrorMsg.c
ExitPass_SOURCES            = ExitPass.c
ExitWithOpenObjects_SOURCES = ExitWithOpenObjects.c
GetInfo_SOURCES             = GetInfo.c
InitPass_SOURCES            = InitPass.c
LastError_SOURCES           = LastError.c
OpenObjNotCreated_SOURCES   = OpenObjNotCreated.c
OpenObjNullName_SOURCES     = OpenObjNullName.c
OpenObjNullObject_SOURCES   = OpenObjNullObject.c
OpenObjNullSession_SOURCES  = OpenObjNullSession.c
OpenObjPass_SOURCES         = OpenObjPass.c
OpenObjWrongType_SOURCES    = OpenObjWrongType.c
OpenObjZeroLength_SOURCES   = OpenObjZeroLength.c
RollbackPass_SOURCES        = RollbackPass.c

EXTRA_DIST =                   \
        CloseNullSession.sh    \
        ClosePass.sh           \
        CloseObjNullObject.sh  \
        CloseObjNullSession.sh \
        CloseObjPass.sh        \
        CommitPass.sh          \
        CreatePass.sh          \
        CreateQueue.sh         \
        DestroyPass.sh         \
        ErrorMsg.sh            \
        ExitPass.sh            \
        ExitWithOpenObjects.sh \
        GetInfo.sh             \
        InitPass.sh            \
        LastError.sh           \
        Makefile.win32         \
        OpenObjNotCreated.sh   \
        OpenObjNullName.sh     \
        OpenObjNullObject.sh   \
        OpenObjNullSession.sh  \
        OpenObjPass.sh         \
        OpenObjWrongType.sh    \
        OpenObjZeroLength.sh   \
        RollbackPass.sh        \
        RunTests.vbs           \
        StatusPass.sh          

TESTS_ENVIRONMENT =                  \
        tcp_port=10701               \
        top_srcdir=$(top_srcdir)     \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                        \
        CloseNullSession.sh    \
        ClosePass.sh           \
        CloseObjNullObject.sh  \
        CloseObjNullSession.sh \
        CloseObjPass.sh        \
        CommitPass.sh          \
        CreatePass.sh          \
        CreateQueue.sh         \
        DestroyPass.sh         \
        ErrorMsg.sh            \
        ExitPass.sh            \
        ExitWithOpenObjects.sh \
        GetInfo.sh             \
        InitPass.sh            \
        LastError.sh           \
        OpenObjNotCreated.sh   \
        OpenObjNullName.sh     \
        OpenObjNullObject.sh   \
        OpenObjNullSession.sh  \
        OpenObjPass.sh         \
        OpenObjWrongType.sh    \
        OpenObjZeroLength.sh   \
        RollbackPass.sh        \
        StatusPass.sh          

XFAIL_TESTS =                  

LDADD = $(top_builddir)/src/API/libphoton.la      \
        $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la   

