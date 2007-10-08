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

check_PROGRAMS =                          \
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
                 GetWrongFlag             \
                 InitNullContext          \
                 InitNullQueue            \
                 InitNullName             \
                 InitNullParent           \
                 InitNullStatus           \
                 InitPass                 \
                 InitZeroBlocks           \
                 InitZeroLength           \
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
                 RollbackAddNullContext   \
                 RollbackAddNullOffset    \
                 RollbackAddNullQueue     \
                 RollbackAddPass          \
                 RollbackRemoveNullOffset \
                 RollbackRemoveNullQueue  \
                 RollbackRemovePass       \
                 StatusNullContext        \
                 StatusNullQueue          \
                 StatusNullTotalItems     \
                 StatusNullValidItems     \
                 StatusPass               

COMMON_HEADER = queueTest.h


CommitAddNullOffset_SOURCES         = $(COMMON_HEADER) CommitAddNullOffset.c
CommitAddNullQueue_SOURCES          = $(COMMON_HEADER) CommitAddNullQueue.c
CommitAddPass_SOURCES               = $(COMMON_HEADER) CommitAddPass.c
CommitRemoveNullContext_SOURCES     = $(COMMON_HEADER) CommitRemoveNullContext.c
CommitRemoveNullOffset_SOURCES      = $(COMMON_HEADER) CommitRemoveNullOffset.c
CommitRemoveNullQueue_SOURCES       = $(COMMON_HEADER) CommitRemoveNullQueue.c
CommitRemovePass_SOURCES            = $(COMMON_HEADER) CommitRemovePass.c
FiniNullContext_SOURCES             = $(COMMON_HEADER) FiniNullContext.c
FiniNullQueue_SOURCES               = $(COMMON_HEADER) FiniNullQueue.c
FiniPass_SOURCES                    = $(COMMON_HEADER) FiniPass.c
GetNullContext_SOURCES              = $(COMMON_HEADER) GetNullContext.c
GetNullQueue_SOURCES                = $(COMMON_HEADER) GetNullQueue.c
GetNullIterator_SOURCES             = $(COMMON_HEADER) GetNullIterator.c
GetPass_SOURCES                     = $(COMMON_HEADER) GetPass.c
GetWrongFlag_SOURCES                = $(COMMON_HEADER) GetWrongFlag.c
InitNullContext_SOURCES             = $(COMMON_HEADER) InitNullContext.c
InitNullQueue_SOURCES               = $(COMMON_HEADER) InitNullQueue.c
InitNullName_SOURCES                = $(COMMON_HEADER) InitNullName.c
InitNullParent_SOURCES              = $(COMMON_HEADER) InitNullParent.c
InitNullStatus_SOURCES              = $(COMMON_HEADER) InitNullStatus.c
InitPass_SOURCES                    = $(COMMON_HEADER) InitPass.c
InitZeroBlocks_SOURCES              = $(COMMON_HEADER) InitZeroBlocks.c
InitZeroLength_SOURCES              = $(COMMON_HEADER) InitZeroLength.c
InsertNullContext_SOURCES           = $(COMMON_HEADER) InsertNullContext.c
InsertNullQueue_SOURCES             = $(COMMON_HEADER) InsertNullQueue.c
InsertNullItem_SOURCES              = $(COMMON_HEADER) InsertNullItem.c
InsertPass_SOURCES                  = $(COMMON_HEADER) InsertPass.c
InsertWrongFlag_SOURCES             = $(COMMON_HEADER) InsertWrongFlag.c
InsertZeroLengthItem_SOURCES        = $(COMMON_HEADER) InsertZeroLengthItem.c
ReleaseNullContext_SOURCES          = $(COMMON_HEADER) ReleaseNullContext.c
ReleaseNullItem_SOURCES             = $(COMMON_HEADER) ReleaseNullItem.c
ReleaseNullQueue_SOURCES            = $(COMMON_HEADER) ReleaseNullQueue.c
ReleasePass_SOURCES                 = $(COMMON_HEADER) ReleasePass.c
ReleaseNoLockNullContext_SOURCES    = $(COMMON_HEADER) ReleaseNoLockNullContext.c
ReleaseNoLockNullItem_SOURCES       = $(COMMON_HEADER) ReleaseNoLockNullItem.c
ReleaseNoLockNullQueue_SOURCES      = $(COMMON_HEADER) ReleaseNoLockNullQueue.c
ReleaseNoLockPass_SOURCES           = $(COMMON_HEADER) ReleaseNoLockPass.c
RemoveNullContext_SOURCES           = $(COMMON_HEADER) RemoveNullContext.c
RemoveNullItem_SOURCES              = $(COMMON_HEADER) RemoveNullItem.c
RemoveNullQueue_SOURCES             = $(COMMON_HEADER) RemoveNullQueue.c
RemovePass_SOURCES                  = $(COMMON_HEADER) RemovePass.c
RemoveWrongFlag_SOURCES             = $(COMMON_HEADER) RemoveWrongFlag.c
RollbackAddNullContext_SOURCES      = $(COMMON_HEADER) RollbackAddNullContext.c
RollbackAddNullOffset_SOURCES       = $(COMMON_HEADER) RollbackAddNullOffset.c
RollbackAddNullQueue_SOURCES        = $(COMMON_HEADER) RollbackAddNullQueue.c
RollbackAddPass_SOURCES             = $(COMMON_HEADER) RollbackAddPass.c
RollbackRemoveNullOffset_SOURCES    = $(COMMON_HEADER) RollbackRemoveNullOffset.c
RollbackRemoveNullQueue_SOURCES     = $(COMMON_HEADER) RollbackRemoveNullQueue.c
RollbackRemovePass_SOURCES          = $(COMMON_HEADER) RollbackRemovePass.c
StatusNullContext_SOURCES           = $(COMMON_HEADER) StatusNullContext.c
StatusNullQueue_SOURCES             = $(COMMON_HEADER) StatusNullQueue.c
StatusNullTotalItems_SOURCES        = $(COMMON_HEADER) StatusNullTotalItems.c
StatusNullValidItems_SOURCES        = $(COMMON_HEADER) StatusNullValidItems.c
StatusPass_SOURCES                  = $(COMMON_HEADER) StatusPass.c
                                                       
EXTRA_DIST = 

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

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
        GetWrongFlag             \
        InitNullContext          \
        InitNullQueue            \
        InitNullName             \
        InitNullParent           \
        InitNullStatus           \
        InitPass                 \
        InitZeroBlocks           \
        InitZeroLength           \
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
        RollbackAddNullContext   \
        RollbackAddNullOffset    \
        RollbackAddNullQueue     \
        RollbackAddPass          \
        RollbackRemoveNullOffset \
        RollbackRemoveNullQueue  \
        RollbackRemovePass       \
        StatusNullContext        \
        StatusNullQueue          \
        StatusNullTotalItems     \
        StatusNullValidItems     \
        StatusPass               

XFAIL_TESTS =                          \
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
              GetWrongFlag             \
              InitNullContext          \
              InitNullQueue            \
              InitNullName             \
              InitNullParent           \
              InitNullStatus           \
              InitZeroBlocks           \
              InitZeroLength           \
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
              StatusNullContext        \
              StatusNullQueue          \
              StatusNullTotalItems     \
              StatusNullValidItems     

LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

