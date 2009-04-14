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

check_PROGRAMS =                 \
        CommitAddNullOffset      \
        CommitAddNullQueue       \
        CommitAddPass            \
        CommitRemoveNullContext  \
        CommitRemoveNullOffset   \
        CommitRemoveNullQueue    \
        CommitRemovePass         \
        FiniNullContext          \
        FiniNullQueue            \
        FiniPass                 \
        GetNullContext           \
        GetNullQueue             \
        GetNullIterator          \
        GetPass                  \
        GetWrongLength           \
        InitNullContext          \
        InitNullDataDef          \
        InitNullDefinition       \
        InitNullHashItem         \
        InitNullQueue            \
        InitNullName             \
        InitNullParent           \
        InitNullStatus           \
        InitPass                 \
        InitZeroBlocks           \
        InitZeroLength           \
        InsertNowNullContext     \
        InsertNowNullItem        \
        InsertNowNullQueue       \
        InsertNowPass            \
        InsertNowWrongFlag       \
        InsertNowZeroLengthItem  \
        InsertNullContext        \
        InsertNullQueue          \
        InsertNullItem           \
        InsertPass               \
        InsertWrongFlag          \
        InsertZeroLengthItem     \
        ReleaseNullContext       \
        ReleaseNullItem          \
        ReleaseNullQueue         \
        ReleasePass              \
        ReleaseNoLockNullContext \
        ReleaseNoLockNullItem    \
        ReleaseNoLockNullQueue   \
        ReleaseNoLockPass        \
        RemoveNullContext        \
        RemoveNullItem           \
        RemoveNullQueue          \
        RemovePass               \
        RemoveWrongFlag          \
        RemoveWrongLength        \
        RollbackAddNullContext   \
        RollbackAddNullOffset    \
        RollbackAddNullQueue     \
        RollbackAddPass          \
        RollbackRemoveNullOffset \
        RollbackRemoveNullQueue  \
        RollbackRemovePass       \
        StatusNullQueue          \
        StatusNullStatus         \
        StatusPass               

COMMON_HEADER = queueTest.h

CommitAddNullOffset_SOURCES      = $(COMMON_HEADER) CommitAddNullOffset.c
CommitAddNullQueue_SOURCES       = $(COMMON_HEADER) CommitAddNullQueue.c
CommitAddPass_SOURCES            = $(COMMON_HEADER) CommitAddPass.c
CommitRemoveNullContext_SOURCES  = $(COMMON_HEADER) CommitRemoveNullContext.c
CommitRemoveNullOffset_SOURCES   = $(COMMON_HEADER) CommitRemoveNullOffset.c
CommitRemoveNullQueue_SOURCES    = $(COMMON_HEADER) CommitRemoveNullQueue.c
CommitRemovePass_SOURCES         = $(COMMON_HEADER) CommitRemovePass.c
FiniNullContext_SOURCES          = $(COMMON_HEADER) FiniNullContext.c
FiniNullQueue_SOURCES            = $(COMMON_HEADER) FiniNullQueue.c
FiniPass_SOURCES                 = $(COMMON_HEADER) FiniPass.c
GetNullContext_SOURCES           = $(COMMON_HEADER) GetNullContext.c
GetNullQueue_SOURCES             = $(COMMON_HEADER) GetNullQueue.c
GetNullIterator_SOURCES          = $(COMMON_HEADER) GetNullIterator.c
GetPass_SOURCES                  = $(COMMON_HEADER) GetPass.c
GetWrongLength_SOURCES           = $(COMMON_HEADER) GetWrongLength.c
InitNullContext_SOURCES          = $(COMMON_HEADER) InitNullContext.c
InitNullDataDef_SOURCES          = $(COMMON_HEADER) InitNullDataDef.c
InitNullDefinition_SOURCES       = $(COMMON_HEADER) InitNullDefinition.c
InitNullHashItem_SOURCES         = $(COMMON_HEADER) InitNullHashItem.c
InitNullQueue_SOURCES            = $(COMMON_HEADER) InitNullQueue.c
InitNullName_SOURCES             = $(COMMON_HEADER) InitNullName.c
InitNullParent_SOURCES           = $(COMMON_HEADER) InitNullParent.c
InitNullStatus_SOURCES           = $(COMMON_HEADER) InitNullStatus.c
InitPass_SOURCES                 = $(COMMON_HEADER) InitPass.c
InitZeroBlocks_SOURCES           = $(COMMON_HEADER) InitZeroBlocks.c
InitZeroLength_SOURCES           = $(COMMON_HEADER) InitZeroLength.c
InsertNowNullContext_SOURCES     = $(COMMON_HEADER) InsertNowNullContext.c
InsertNowNullItem_SOURCES        = $(COMMON_HEADER) InsertNowNullItem.c
InsertNowNullQueue_SOURCES       = $(COMMON_HEADER) InsertNowNullQueue.c
InsertNowPass_SOURCES            = $(COMMON_HEADER) InsertNowPass.c
InsertNowWrongFlag_SOURCES       = $(COMMON_HEADER) InsertNowWrongFlag.c
InsertNowZeroLengthItem_SOURCES  = $(COMMON_HEADER) InsertNowZeroLengthItem.c
InsertNullContext_SOURCES        = $(COMMON_HEADER) InsertNullContext.c
InsertNullQueue_SOURCES          = $(COMMON_HEADER) InsertNullQueue.c
InsertNullItem_SOURCES           = $(COMMON_HEADER) InsertNullItem.c
InsertPass_SOURCES               = $(COMMON_HEADER) InsertPass.c
InsertWrongFlag_SOURCES          = $(COMMON_HEADER) InsertWrongFlag.c
InsertZeroLengthItem_SOURCES     = $(COMMON_HEADER) InsertZeroLengthItem.c
ReleaseNullContext_SOURCES       = $(COMMON_HEADER) ReleaseNullContext.c
ReleaseNullItem_SOURCES          = $(COMMON_HEADER) ReleaseNullItem.c
ReleaseNullQueue_SOURCES         = $(COMMON_HEADER) ReleaseNullQueue.c
ReleasePass_SOURCES              = $(COMMON_HEADER) ReleasePass.c
ReleaseNoLockNullContext_SOURCES = $(COMMON_HEADER) ReleaseNoLockNullContext.c
ReleaseNoLockNullItem_SOURCES    = $(COMMON_HEADER) ReleaseNoLockNullItem.c
ReleaseNoLockNullQueue_SOURCES   = $(COMMON_HEADER) ReleaseNoLockNullQueue.c
ReleaseNoLockPass_SOURCES        = $(COMMON_HEADER) ReleaseNoLockPass.c
RemoveNullContext_SOURCES        = $(COMMON_HEADER) RemoveNullContext.c
RemoveNullItem_SOURCES           = $(COMMON_HEADER) RemoveNullItem.c
RemoveNullQueue_SOURCES          = $(COMMON_HEADER) RemoveNullQueue.c
RemovePass_SOURCES               = $(COMMON_HEADER) RemovePass.c
RemoveWrongFlag_SOURCES          = $(COMMON_HEADER) RemoveWrongFlag.c
RemoveWrongLength_SOURCES        = $(COMMON_HEADER) RemoveWrongLength.c
RollbackAddNullContext_SOURCES   = $(COMMON_HEADER) RollbackAddNullContext.c
RollbackAddNullOffset_SOURCES    = $(COMMON_HEADER) RollbackAddNullOffset.c
RollbackAddNullQueue_SOURCES     = $(COMMON_HEADER) RollbackAddNullQueue.c
RollbackAddPass_SOURCES          = $(COMMON_HEADER) RollbackAddPass.c
RollbackRemoveNullOffset_SOURCES = $(COMMON_HEADER) RollbackRemoveNullOffset.c
RollbackRemoveNullQueue_SOURCES  = $(COMMON_HEADER) RollbackRemoveNullQueue.c
RollbackRemovePass_SOURCES       = $(COMMON_HEADER) RollbackRemovePass.c
StatusNullQueue_SOURCES          = $(COMMON_HEADER) StatusNullQueue.c
StatusNullStatus_SOURCES         = $(COMMON_HEADER) StatusNullStatus.c
StatusPass_SOURCES               = $(COMMON_HEADER) StatusPass.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                          \
        CommitAddNullOffset      \
        CommitAddNullQueue       \
        CommitAddPass            \
        CommitRemoveNullContext  \
        CommitRemoveNullOffset   \
        CommitRemoveNullQueue    \
        CommitRemovePass         \
        FiniNullContext          \
        FiniNullQueue            \
        FiniPass                 \
        GetNullContext           \
        GetNullQueue             \
        GetNullIterator          \
        GetPass                  \
        GetWrongLength           \
        InitNullContext          \
        InitNullDataDef          \
        InitNullDefinition       \
        InitNullHashItem         \
        InitNullQueue            \
        InitNullName             \
        InitNullParent           \
        InitNullStatus           \
        InitPass                 \
        InitZeroBlocks           \
        InitZeroLength           \
        InsertNowNullContext     \
        InsertNowNullItem        \
        InsertNowNullQueue       \
        InsertNowPass            \
        InsertNowWrongFlag       \
        InsertNowZeroLengthItem  \
        InsertNullContext        \
        InsertNullQueue          \
        InsertNullItem           \
        InsertPass               \
        InsertWrongFlag          \
        InsertZeroLengthItem     \
        ReleaseNullContext       \
        ReleaseNullItem          \
        ReleaseNullQueue         \
        ReleasePass              \
        ReleaseNoLockNullContext \
        ReleaseNoLockNullItem    \
        ReleaseNoLockNullQueue   \
        ReleaseNoLockPass        \
        RemoveNullContext        \
        RemoveNullItem           \
        RemoveNullQueue          \
        RemovePass               \
        RemoveWrongFlag          \
        RemoveWrongLength        \
        RollbackAddNullContext   \
        RollbackAddNullOffset    \
        RollbackAddNullQueue     \
        RollbackAddPass          \
        RollbackRemoveNullOffset \
        RollbackRemoveNullQueue  \
        RollbackRemovePass       \
        StatusNullQueue          \
        StatusNullStatus         \
        StatusPass               

XFAIL_TESTS =                    \
        CommitAddNullOffset      \
        CommitAddNullQueue       \
        CommitRemoveNullContext  \
        CommitRemoveNullOffset   \
        CommitRemoveNullQueue    \
        FiniNullContext          \
        FiniNullQueue            \
        GetNullContext           \
        GetNullQueue             \
        GetNullIterator          \
        InitNullContext          \
        InitNullDataDef          \
        InitNullDefinition       \
        InitNullHashItem         \
        InitNullQueue            \
        InitNullName             \
        InitNullParent           \
        InitNullStatus           \
        InitZeroBlocks           \
        InitZeroLength           \
        InsertNowNullContext     \
        InsertNowNullItem        \
        InsertNowNullQueue       \
        InsertNowWrongFlag       \
        InsertNowZeroLengthItem  \
        InsertNullContext        \
        InsertNullQueue          \
        InsertNullItem           \
        InsertWrongFlag          \
        InsertZeroLengthItem     \
        ReleaseNullContext       \
        ReleaseNullItem          \
        ReleaseNullQueue         \
        ReleaseNoLockNullContext \
        ReleaseNoLockNullItem    \
        ReleaseNoLockNullQueue   \
        RemoveNullContext        \
        RemoveNullItem           \
        RemoveNullQueue          \
        RemoveWrongFlag          \
        RollbackAddNullContext   \
        RollbackAddNullOffset    \
        RollbackAddNullQueue     \
        RollbackRemoveNullOffset \
        RollbackRemoveNullQueue  \
        StatusNullQueue          \
        StatusNullStatus         


LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

