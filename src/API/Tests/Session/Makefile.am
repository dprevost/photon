# Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

check_PROGRAMS =                     \
                 CloseNullSession    \
                 ClosePass           \
                 CloseObjNullObject  \
                 CloseObjNullSession \
                 CloseObjPass        \
                 CommitNullHandle    \
                 CommitPass          \
                 CreateNullHandle    \
                 CreateNullName      \
                 CreatePass          \
                 CreateWrongType     \
                 CreateZeroLength    \
                 DestroyNotCreated   \
                 DestroyNullHandle   \
                 DestroyNullName     \
                 DestroyPass         \
                 DestroyZeroLength   \
                 ExitNullHandle      \
                 ExitPass            \
                 InitNoProcess       \
                 InitNullHandle      \
                 InitPass            \
                 OpenObjNotCreated   \
                 OpenObjNullName     \
                 OpenObjNullObject   \
                 OpenObjNullSession  \
                 OpenObjPass         \
                 OpenObjWrongType    \
                 OpenObjZeroLength   \
                 RollbackNullHandle  \
                 RollbackPass        \
                 StatusPass          

CloseNullSession_SOURCES    = CloseNullSession.c
ClosePass_SOURCES           = ClosePass.c
CloseObjNullObject_SOURCES  = CloseObjNullObject.c
CloseObjNullSession_SOURCES = CloseObjNullSession.c
CloseObjPass_SOURCES        = CloseObjPass.c
CommitNullHandle_SOURCES    = CommitNullHandle.c
CommitPass_SOURCES          = CommitPass.c
CreateNullHandle_SOURCES    = CreateNullHandle.c
CreateNullName_SOURCES      = CreateNullName.c
CreatePass_SOURCES          = CreatePass.c
CreateWrongType_SOURCES     = CreateWrongType.c
CreateZeroLength_SOURCES    = CreateZeroLength.c
DestroyNotCreated_SOURCES   = DestroyNotCreated.c
DestroyNullHandle_SOURCES   = DestroyNullHandle.c
DestroyNullName_SOURCES     = DestroyNullName.c
DestroyPass_SOURCES         = DestroyPass.c
DestroyZeroLength_SOURCES   = DestroyZeroLength.c
ExitNullHandle_SOURCES      = ExitNullHandle.c
ExitPass_SOURCES            = ExitPass.c
InitNoProcess_SOURCES       = InitNoProcess.c
InitNullHandle_SOURCES      = InitNullHandle.c
InitPass_SOURCES            = InitPass.c
OpenObjNotCreated_SOURCES   = OpenObjNotCreated.c
OpenObjNullName_SOURCES     = OpenObjNullName.c
OpenObjNullObject_SOURCES   = OpenObjNullObject.c
OpenObjNullSession_SOURCES  = OpenObjNullSession.c
OpenObjPass_SOURCES         = OpenObjPass.c
OpenObjWrongType_SOURCES    = OpenObjWrongType.c
OpenObjZeroLength_SOURCES   = OpenObjZeroLength.c
RollbackNullHandle_SOURCES  = RollbackNullHandle.c
RollbackPass_SOURCES        = RollbackPass.c

EXTRA_DIST =                        \
             CloseNullSession.sh    \
             ClosePass.sh           \
             CloseObjNullObject.sh  \
             CloseObjNullSession.sh \
             CloseObjPass.sh        \
             CommitNullHandle.sh    \
             CommitPass.sh          \
             CreateNullHandle.sh    \
             CreateNullName.sh      \
             CreatePass.sh          \
             CreateWrongType.sh     \
             CreateZeroLength.sh    \
             DestroyNotCreated.sh   \
             DestroyNullHandle.sh   \
             DestroyNullName.sh     \
             DestroyPass.sh         \
             DestroyZeroLength.sh   \
             ExitNullHandle.sh      \
             ExitPass.sh            \
             InitNullHandle.sh      \
             InitPass.sh            \
             OpenObjNotCreated.sh   \
             OpenObjNullName.sh     \
             OpenObjNullObject.sh   \
             OpenObjNullSession.sh  \
             OpenObjPass.sh         \
             OpenObjWrongType.sh    \
             OpenObjZeroLength.sh   \
             RollbackNullHandle.sh  \
             RollbackPass.sh        \
             StatusPass.sh          

TESTS_ENVIRONMENT =                              \
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
        CommitNullHandle.sh    \
        CommitPass.sh          \
        CreateNullHandle.sh    \
        CreateNullName.sh      \
        CreatePass.sh          \
        CreateWrongType.sh     \
        CreateZeroLength.sh    \
        DestroyNotCreated.sh   \
        DestroyNullHandle.sh   \
        DestroyNullName.sh     \
        DestroyPass.sh         \
        DestroyZeroLength.sh   \
        ExitNullHandle.sh      \
        ExitPass.sh            \
        InitNoProcess          \
        InitNullHandle.sh      \
        InitPass.sh            \
        OpenObjNotCreated.sh   \
        OpenObjNullName.sh     \
        OpenObjNullObject.sh   \
        OpenObjNullSession.sh  \
        OpenObjPass.sh         \
        OpenObjWrongType.sh    \
        OpenObjZeroLength.sh   \
        RollbackNullHandle.sh  \
        RollbackPass.sh        \
        StatusPass.sh          

XFAIL_TESTS =                  

LDADD = $(top_builddir)/src/API/libvdsf.la      \
        $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

