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

check_PROGRAMS =                  \
        CommitAddNullContext      \
        CommitAddNullHash         \
        CommitAddNullItem         \
        CommitAddPass             \
        CommitRemoveNullContext   \
        CommitRemoveNullHash      \
        CommitRemoveNullItem      \
        CommitRemovePass          \
        DeleteNullContext         \
        DeleteNullHash            \
        DeleteNullKey             \
        DeletePass                \
        DeleteZeroKeyLength       \
        FiniNullContext           \
        FiniNullHash              \
        FiniPass                  \
        GetFirstNullContext       \
        GetFirstNullHash          \
        GetFirstNullItem          \
        GetFirstPass              \
        GetFirstWrongKeyLength    \
        GetFirstWrongLength       \
        GetNextNullContext        \
        GetNextNullHash           \
        GetNextNullItem           \
        GetNextPass               \
        GetNextWrongKeyLength     \
        GetNextWrongLength        \
        GetNullContext            \
        GetNullHash               \
        GetNullItem               \
        GetNullKey                \
        GetPass                   \
        GetWrongLength            \
        GetZeroKeyLength          \
        InitNullContext           \
        InitNullDataDef           \
        InitNullDefinition        \
        InitNullHash              \
        InitNullHashItemOffset    \
        InitNullKeyDef            \
        InitNullName              \
        InitNullParent            \
        InitNullStatus            \
        InitPass                  \
        InitZeroBlocks            \
        InitZeroLength            \
        InsertNullContext         \
        InsertNullData            \
        InsertNullHash            \
        InsertNullKey             \
        InsertPass                \
        InsertZeroLengthItem      \
        InsertZeroLengthKey       \
        ReleaseNullContext        \
        ReleaseNullHash           \
        ReleaseNullItem           \
        ReleasePass               \
        ReplaceNullContext        \
        ReplaceNullData           \
        ReplaceNullHash           \
        ReplaceNullKey            \
        ReplacePass               \
        ReplaceZeroLengthData     \
        ReplaceZeroLengthKey      \
        RollbackAddNullContext    \
        RollbackAddNullHash       \
        RollbackAddNullItem       \
        RollbackAddPass           \
        RollbackRemoveNullContext \
        RollbackRemoveNullHash    \
        RollbackRemoveNullItem    \
        RollbackRemovePass        \
        StatusNullHash            \
        StatusNullStatus          \
        StatusPass                

COMMON_HEADER = hashMapTest.h

CommitAddNullContext_SOURCES      = $(COMMON_HEADER) CommitAddNullContext.c
CommitAddNullHash_SOURCES         = $(COMMON_HEADER) CommitAddNullHash.c
CommitAddNullItem_SOURCES         = $(COMMON_HEADER) CommitAddNullItem.c
CommitAddPass_SOURCES             = $(COMMON_HEADER) CommitAddPass.c
CommitRemoveNullContext_SOURCES   = $(COMMON_HEADER) CommitRemoveNullContext.c
CommitRemoveNullHash_SOURCES      = $(COMMON_HEADER) CommitRemoveNullHash.c
CommitRemoveNullItem_SOURCES      = $(COMMON_HEADER) CommitRemoveNullItem.c
CommitRemovePass_SOURCES          = $(COMMON_HEADER) CommitRemovePass.c
DeleteNullContext_SOURCES         = $(COMMON_HEADER) DeleteNullContext.c
DeleteNullHash_SOURCES            = $(COMMON_HEADER) DeleteNullHash.c
DeleteNullKey_SOURCES             = $(COMMON_HEADER) DeleteNullKey.c
DeletePass_SOURCES                = $(COMMON_HEADER) DeletePass.c
DeleteZeroKeyLength_SOURCES       = $(COMMON_HEADER) DeleteZeroKeyLength.c
FiniNullContext_SOURCES           = $(COMMON_HEADER) FiniNullContext.c
FiniNullHash_SOURCES              = $(COMMON_HEADER) FiniNullHash.c
FiniPass_SOURCES                  = $(COMMON_HEADER) FiniPass.c
GetFirstNullContext_SOURCES       = $(COMMON_HEADER) GetFirstNullContext.c
GetFirstNullHash_SOURCES          = $(COMMON_HEADER) GetFirstNullHash.c
GetFirstNullItem_SOURCES          = $(COMMON_HEADER) GetFirstNullItem.c
GetFirstPass_SOURCES              = $(COMMON_HEADER) GetFirstPass.c
GetFirstWrongKeyLength_SOURCES    = $(COMMON_HEADER) GetFirstWrongKeyLength.c
GetFirstWrongLength_SOURCES       = $(COMMON_HEADER) GetFirstWrongLength.c
GetNextNullContext_SOURCES        = $(COMMON_HEADER) GetNextNullContext.c
GetNextNullHash_SOURCES           = $(COMMON_HEADER) GetNextNullHash.c
GetNextNullItem_SOURCES           = $(COMMON_HEADER) GetNextNullItem.c
GetNextPass_SOURCES               = $(COMMON_HEADER) GetNextPass.c
GetNextWrongKeyLength_SOURCES     = $(COMMON_HEADER) GetNextWrongKeyLength.c
GetNextWrongLength_SOURCES        = $(COMMON_HEADER) GetNextWrongLength.c
GetNullContext_SOURCES            = $(COMMON_HEADER) GetNullContext.c
GetNullHash_SOURCES               = $(COMMON_HEADER) GetNullHash.c
GetNullItem_SOURCES               = $(COMMON_HEADER) GetNullItem.c
GetNullKey_SOURCES                = $(COMMON_HEADER) GetNullKey.c
GetPass_SOURCES                   = $(COMMON_HEADER) GetPass.c
GetWrongLength_SOURCES            = $(COMMON_HEADER) GetFirstWrongLength.c
GetZeroKeyLength_SOURCES          = $(COMMON_HEADER) GetZeroKeyLength.c
InitNullContext_SOURCES           = $(COMMON_HEADER) InitNullContext.c
InitNullDataDef_SOURCES           = $(COMMON_HEADER) InitNullDataDef.c
InitNullDefinition_SOURCES        = $(COMMON_HEADER) InitNullDefinition.c
InitNullHash_SOURCES              = $(COMMON_HEADER) InitNullHash.c
InitNullHashItemOffset_SOURCES    = $(COMMON_HEADER) InitNullHashItemOffset.c
InitNullKeyDef_SOURCES            = $(COMMON_HEADER) InitNullKeyDef.c
InitNullName_SOURCES              = $(COMMON_HEADER) InitNullName.c
InitNullParent_SOURCES            = $(COMMON_HEADER) InitNullParent.c
InitNullStatus_SOURCES            = $(COMMON_HEADER) InitNullStatus.c
InitPass_SOURCES                  = $(COMMON_HEADER) InitPass.c
InitZeroBlocks_SOURCES            = $(COMMON_HEADER) InitZeroBlocks.c
InitZeroLength_SOURCES            = $(COMMON_HEADER) InitZeroLength.c
InsertNullContext_SOURCES         = $(COMMON_HEADER) InsertNullContext.c
InsertNullData_SOURCES            = $(COMMON_HEADER) InsertNullData.c
InsertNullHash_SOURCES            = $(COMMON_HEADER) InsertNullHash.c
InsertNullKey_SOURCES             = $(COMMON_HEADER) InsertNullKey.c
InsertPass_SOURCES                = $(COMMON_HEADER) InsertPass.c
InsertZeroLengthItem_SOURCES      = $(COMMON_HEADER) InsertZeroLengthItem.c
InsertZeroLengthKey_SOURCES       = $(COMMON_HEADER) InsertZeroLengthKey.c
ReleaseNullContext_SOURCES        = $(COMMON_HEADER) ReleaseNullContext.c
ReleaseNullHash_SOURCES           = $(COMMON_HEADER) ReleaseNullHash.c
ReleaseNullItem_SOURCES           = $(COMMON_HEADER) ReleaseNullItem.c
ReleasePass_SOURCES               = $(COMMON_HEADER) ReleasePass.c
ReplaceNullContext_SOURCES        = $(COMMON_HEADER) ReplaceNullContext.c
ReplaceNullData_SOURCES           = $(COMMON_HEADER) ReplaceNullData.c
ReplaceNullHash_SOURCES           = $(COMMON_HEADER) ReplaceNullHash.c
ReplaceNullKey_SOURCES            = $(COMMON_HEADER) ReplaceNullKey.c
ReplacePass_SOURCES               = $(COMMON_HEADER) ReplacePass.c
ReplaceZeroLengthData_SOURCES     = $(COMMON_HEADER) ReplaceZeroLengthData.c
ReplaceZeroLengthKey_SOURCES      = $(COMMON_HEADER) ReplaceZeroLengthKey.c
RollbackAddNullContext_SOURCES    = $(COMMON_HEADER) RollbackAddNullContext.c
RollbackAddNullHash_SOURCES       = $(COMMON_HEADER) RollbackAddNullHash.c
RollbackAddNullItem_SOURCES       = $(COMMON_HEADER) RollbackAddNullItem.c
RollbackAddPass_SOURCES           = $(COMMON_HEADER) RollbackAddPass.c
RollbackRemoveNullContext_SOURCES = $(COMMON_HEADER) RollbackRemoveNullContext.c
RollbackRemoveNullHash_SOURCES    = $(COMMON_HEADER) RollbackRemoveNullHash.c
RollbackRemoveNullItem_SOURCES    = $(COMMON_HEADER) RollbackRemoveNullItem.c
RollbackRemovePass_SOURCES        = $(COMMON_HEADER) RollbackRemovePass.c
StatusNullHash_SOURCES            = $(COMMON_HEADER) StatusNullHash.c
StatusNullStatus_SOURCES          = $(COMMON_HEADER) StatusNullStatus.c
StatusPass_SOURCES                = $(COMMON_HEADER) StatusPass.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                           \
        CommitAddNullContext      \
        CommitAddNullHash         \
        CommitAddNullItem         \
        CommitAddPass             \
        CommitRemoveNullContext   \
        CommitRemoveNullHash      \
        CommitRemoveNullItem      \
        CommitRemovePass          \
        DeleteNullContext         \
        DeleteNullHash            \
        DeleteNullKey             \
        DeletePass                \
        DeleteZeroKeyLength       \
        FiniNullContext           \
        FiniNullHash              \
        FiniPass                  \
        GetFirstNullContext       \
        GetFirstNullHash          \
        GetFirstNullItem          \
        GetFirstPass              \
        GetFirstWrongKeyLength    \
        GetFirstWrongLength       \
        GetNextNullContext        \
        GetNextNullHash           \
        GetNextNullItem           \
        GetNextPass               \
        GetNextWrongKeyLength     \
        GetNextWrongLength        \
        GetNullContext            \
        GetNullHash               \
        GetNullItem               \
        GetNullKey                \
        GetPass                   \
        GetWrongLength            \
        GetZeroKeyLength          \
        InitNullContext           \
        InitNullDataDef           \
        InitNullDefinition        \
        InitNullHash              \
        InitNullHashItemOffset    \
        InitNullKeyDef            \
        InitNullName              \
        InitNullParent            \
        InitNullStatus            \
        InitPass                  \
        InitZeroBlocks            \
        InitZeroLength            \
        InsertNullContext         \
        InsertNullData            \
        InsertNullHash            \
        InsertNullKey             \
        InsertPass                \
        InsertZeroLengthItem      \
        InsertZeroLengthKey       \
        ReleaseNullContext        \
        ReleaseNullHash           \
        ReleaseNullItem           \
        ReleasePass               \
        ReplaceNullContext        \
        ReplaceNullData           \
        ReplaceNullHash           \
        ReplaceNullKey            \
        ReplacePass               \
        ReplaceZeroLengthData     \
        ReplaceZeroLengthKey      \
        RollbackAddNullContext    \
        RollbackAddNullHash       \
        RollbackAddNullItem       \
        RollbackAddPass           \
        RollbackRemoveNullContext \
        RollbackRemoveNullHash    \
        RollbackRemoveNullItem    \
        RollbackRemovePass        \
        StatusNullHash            \
        StatusNullStatus          \
        StatusPass                

XFAIL_TESTS =                     \
        CommitAddNullContext      \
        CommitAddNullHash         \
        CommitAddNullItem         \
        CommitRemoveNullContext   \
        CommitRemoveNullHash      \
        CommitRemoveNullItem      \
        DeleteNullContext         \
        DeleteNullHash            \
        DeleteNullKey             \
        DeleteZeroKeyLength       \
        FiniNullContext           \
        FiniNullHash              \
        GetFirstNullContext       \
        GetFirstNullHash          \
        GetFirstNullItem          \
        GetNextNullContext        \
        GetNextNullHash           \
        GetNextNullItem           \
        GetNullContext            \
        GetNullHash               \
        GetNullItem               \
        GetNullKey                \
        GetZeroKeyLength          \
        InitNullContext           \
        InitNullDataDef           \
        InitNullDefinition        \
        InitNullHash              \
        InitNullHashItemOffset    \
        InitNullKeyDef            \
        InitNullName              \
        InitNullParent            \
        InitNullStatus            \
        InitZeroBlocks            \
        InitZeroLength            \
        InsertNullContext         \
        InsertNullData            \
        InsertNullHash            \
        InsertNullKey             \
        InsertZeroLengthItem      \
        InsertZeroLengthKey       \
        ReleaseNullContext        \
        ReleaseNullHash           \
        ReleaseNullItem           \
        ReplaceNullContext        \
        ReplaceNullData           \
        ReplaceNullHash           \
        ReplaceNullKey            \
        ReplaceZeroLengthData     \
        ReplaceZeroLengthKey      \
        RollbackAddNullContext    \
        RollbackAddNullHash       \
        RollbackAddNullItem       \
        RollbackRemoveNullContext \
        RollbackRemoveNullHash    \
        RollbackRemoveNullItem    \
        StatusNullHash            \
        StatusNullStatus          

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

