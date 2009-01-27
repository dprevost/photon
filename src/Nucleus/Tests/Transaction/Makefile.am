# Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

check_PROGRAMS =              \
        AddInvalidSig         \
        AddNullChildOffset    \
        AddNullContext        \
        AddNullParentOffset   \
        AddNullTx             \
        AddPass               \
        CommitInvalidSig      \
        CommitNullContext     \
        CommitNullTx          \
        CommitPass            \
        FiniInvalidSig        \
        FiniNullContext       \
        FiniNullTx            \
        FiniPass              \
        InitNullContext       \
        InitNullTx            \
        InitPass              \
        InitZeroBlocks        \
        RemoveLastInvalidSig  \
        RemoveLastNullContext \
        RemoveLastNullTx      \
        RemoveLastPass        \
        RollbackInvalidSig    \
        RollbackNullContext   \
        RollbackNullTx        \
        RollbackPass          \
        TestFolder            \
        TestHashMap           \
        TestQueue             

COMMON_HEADER = txTest.h

AddInvalidSig_SOURCES         = $(COMMON_HEADER) AddInvalidSig.c
AddNullChildOffset_SOURCES    = $(COMMON_HEADER) AddNullChildOffset.c
AddNullContext_SOURCES        = $(COMMON_HEADER) AddNullContext.c
AddNullParentOffset_SOURCES   = $(COMMON_HEADER) AddNullParentOffset.c
AddNullTx_SOURCES             = $(COMMON_HEADER) AddNullTx.c
AddPass_SOURCES               = $(COMMON_HEADER) AddPass.c
CommitInvalidSig_SOURCES      = $(COMMON_HEADER) CommitInvalidSig.c
CommitNullContext_SOURCES     = $(COMMON_HEADER) CommitNullContext.c
CommitNullTx_SOURCES          = $(COMMON_HEADER) CommitNullTx.c
CommitPass_SOURCES            = $(COMMON_HEADER) CommitPass.c
FiniInvalidSig_SOURCES        = $(COMMON_HEADER) FiniInvalidSig.c
FiniNullContext_SOURCES       = $(COMMON_HEADER) FiniNullContext.c
FiniNullTx_SOURCES            = $(COMMON_HEADER) FiniNullTx.c
FiniPass_SOURCES              = $(COMMON_HEADER) FiniPass.c
InitNullContext_SOURCES       = $(COMMON_HEADER) InitNullContext.c
InitNullTx_SOURCES            = $(COMMON_HEADER) InitNullTx.c
InitPass_SOURCES              = $(COMMON_HEADER) InitPass.c
InitZeroBlocks_SOURCES        = $(COMMON_HEADER) InitZeroBlocks.c
RemoveLastInvalidSig_SOURCES  = $(COMMON_HEADER) RemoveLastInvalidSig.c
RemoveLastNullContext_SOURCES = $(COMMON_HEADER) RemoveLastNullContext.c
RemoveLastNullTx_SOURCES      = $(COMMON_HEADER) RemoveLastNullTx.c
RemoveLastPass_SOURCES        = $(COMMON_HEADER) RemoveLastPass.c
RollbackInvalidSig_SOURCES    = $(COMMON_HEADER) RollbackInvalidSig.c
RollbackNullContext_SOURCES   = $(COMMON_HEADER) RollbackNullContext.c
RollbackNullTx_SOURCES        = $(COMMON_HEADER) RollbackNullTx.c
RollbackPass_SOURCES          = $(COMMON_HEADER) RollbackPass.c
TestFolder_SOURCES            = $(COMMON_HEADER) TestFolder.c
TestHashMap_SOURCES           = $(COMMON_HEADER) TestHashMap.c
TestQueue_SOURCES             = $(COMMON_HEADER) TestQueue.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                       \
        AddInvalidSig         \
        AddNullChildOffset    \
        AddNullContext        \
        AddNullParentOffset   \
        AddNullTx             \
        AddPass               \
        CommitInvalidSig      \
        CommitNullContext     \
        CommitNullTx          \
        CommitPass            \
        FiniInvalidSig        \
        FiniNullContext       \
        FiniNullTx            \
        FiniPass              \
        InitNullContext       \
        InitNullTx            \
        InitPass              \
        InitZeroBlocks        \
        RemoveLastInvalidSig  \
        RemoveLastNullContext \
        RemoveLastNullTx      \
        RemoveLastPass        \
        RollbackInvalidSig    \
        RollbackNullContext   \
        RollbackNullTx        \
        RollbackPass          \
        TestFolder            \
        TestHashMap           \
        TestQueue             

XFAIL_TESTS =                 \
        AddInvalidSig         \
        AddNullChildOffset    \
        AddNullContext        \
        AddNullParentOffset   \
        AddNullTx             \
        CommitInvalidSig      \
        CommitNullContext     \
        CommitNullTx          \
        FiniInvalidSig        \
        FiniNullContext       \
        FiniNullTx            \
        InitNullContext       \
        InitNullTx            \
        InitZeroBlocks        \
        RemoveLastInvalidSig  \
        RemoveLastNullContext \
        RemoveLastNullTx      \
        RollbackInvalidSig    \
        RollbackNullContext   \
        RollbackNullTx        

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

