# Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

check_PROGRAMS =                \
        CloseNullContext        \
        CloseNullItem           \
        ClosePass               \
        CreateInvalidNameLength \
        CreateNullContext       \
        CreateNullDefinition    \
        CreateNullFolder        \
        CreateNullName          \
        CreatePass              \
        CreateWrongType         \
        DeleteInvalidSig        \
        DeleteNullContext       \
        DeleteNullFolder        \
        DeleteNullName          \
        DeletePass              \
        DeleteWrongObjType      \
        DeleteZeroNameLength    \
        DestroyNullContext      \
        DestroyNullFolder       \
        DestroyNullName         \
        DestroyPass             \
        FiniInvalidSig          \
        FiniNullContext         \
        FiniNullFolder          \
        FiniPass                \
        FiniWrongObjType        \
        GetFirstEmpty           \
        GetFirstNullContext     \
        GetFirstNullFolder      \
        GetFirstNullItem        \
        GetFirstPass            \
        GetFirstWrongObjType    \
        GetInvalidSig           \
        GetNextItemNullHashItem \
        GetNextItemNullOffset   \
        GetNextNullContext      \
        GetNextNullFolder       \
        GetNextNullItem         \
        GetNextPass             \
        GetNextWrongObjType     \
        GetNullContext          \
        GetNullItem             \
        GetNullFolder           \
        GetNullName             \
        GetPass                 \
        GetStatusNullContext    \
        GetStatusNullFolder     \
        GetStatusNullName       \
        GetStatusNullStatus     \
        GetStatusPass           \
        GetStatusZeroLength     \
        GetWrongObjType         \
        GetZeroNameLength       \
        InitNullContext         \
        InitNullFolder          \
        InitNullKeyOffset       \
        InitNullName            \
        InitNullParentOffset    \
        InitNullStatus          \
        InitPass                \
        InitZeroBlocks          \
        InitZeroNameLength      \
        InsertInvalidSig        \
        InsertNullContext       \
        InsertNullDefinition    \
        InsertNullFolder        \
        InsertNullName          \
        InsertNullOrigName      \
        InsertPass              \
        InsertWrongObjType      \
        InsertZeroBlocks        \
        InsertZeroNameLength    \
        OpenNullContext         \
        OpenNullItem            \
        OpenNullFolder          \
        OpenNullName            \
        OpenPass                \
        ReleaseNullContext      \
        ReleaseNullFolder       \
        ReleaseNullItem         \
        ReleasePass             \
        ReleaseWrongObjType     \
        RemoveNullContext       \
        RemoveNullFolder        \
        RemoveNullItem          \
        RemovePass              \
        RemoveWrongObjType      \
        StatusNullFolder        \
        StatusNullStatus        \
        StatusPass              \
        Tests                   \
        TopStatusNullContext    \
        TopStatusNullFolder     \
        TopStatusNullName       \
        TopStatusNullStatus     \
        TopStatusPass           \
        ValidatePass            

COMMON_HEADER = folderTest.h

CloseNullContext_SOURCES        = $(COMMON_HEADER) CloseNullContext.c
CloseNullItem_SOURCES           = $(COMMON_HEADER) CloseNullItem.c
ClosePass_SOURCES               = $(COMMON_HEADER) ClosePass.c
CreateInvalidNameLength_SOURCES = $(COMMON_HEADER) CreateInvalidNameLength.c
CreateNullContext_SOURCES       = $(COMMON_HEADER) CreateNullContext.c
CreateNullDefinition_SOURCES    = $(COMMON_HEADER) CreateNullDefinition.c
CreateNullFolder_SOURCES        = $(COMMON_HEADER) CreateNullFolder.c
CreateNullName_SOURCES          = $(COMMON_HEADER) CreateNullName.c
CreatePass_SOURCES              = $(COMMON_HEADER) CreatePass.c
CreateWrongType_SOURCES         = $(COMMON_HEADER) CreateWrongType.c
DeleteInvalidSig_SOURCES        = $(COMMON_HEADER) DeleteInvalidSig.c
DeleteNullContext_SOURCES       = $(COMMON_HEADER) DeleteNullContext.c
DeleteNullFolder_SOURCES        = $(COMMON_HEADER) DeleteNullFolder.c
DeleteNullName_SOURCES          = $(COMMON_HEADER) DeleteNullName.c
DeletePass_SOURCES              = $(COMMON_HEADER) DeletePass.c
DeleteWrongObjType_SOURCES      = $(COMMON_HEADER) DeleteWrongObjType.c
DeleteZeroNameLength_SOURCES    = $(COMMON_HEADER) DeleteZeroNameLength.c
DestroyNullContext_SOURCES      = $(COMMON_HEADER) DestroyNullContext.c
DestroyNullFolder_SOURCES       = $(COMMON_HEADER) DestroyNullFolder.c
DestroyNullName_SOURCES         = $(COMMON_HEADER) DestroyNullName.c
DestroyPass_SOURCES             = $(COMMON_HEADER) DestroyPass.c
FiniInvalidSig_SOURCES          = $(COMMON_HEADER) FiniInvalidSig.c
FiniNullContext_SOURCES         = $(COMMON_HEADER) FiniNullContext.c
FiniNullFolder_SOURCES          = $(COMMON_HEADER) FiniNullFolder.c
FiniPass_SOURCES                = $(COMMON_HEADER) FiniPass.c
FiniWrongObjType_SOURCES        = $(COMMON_HEADER) FiniWrongObjType.c
GetFirstEmpty_SOURCES           = $(COMMON_HEADER) GetFirstEmpty.c
GetFirstNullContext_SOURCES     = $(COMMON_HEADER) GetFirstNullContext.c
GetFirstNullFolder_SOURCES      = $(COMMON_HEADER) GetFirstNullFolder.c
GetFirstNullItem_SOURCES        = $(COMMON_HEADER) GetFirstNullItem.c
GetFirstPass_SOURCES            = $(COMMON_HEADER) GetFirstPass.c
GetFirstWrongObjType_SOURCES    = $(COMMON_HEADER) GetFirstWrongObjType.c
GetInvalidSig_SOURCES           = $(COMMON_HEADER) GetInvalidSig.c
GetNextItemNullHashItem_SOURCES = $(COMMON_HEADER) GetNextItemNullHashItem.c
GetNextItemNullOffset_SOURCES   = $(COMMON_HEADER) GetNextItemNullOffset.c
GetNextNullContext_SOURCES      = $(COMMON_HEADER) GetNextNullContext.c
GetNextNullFolder_SOURCES       = $(COMMON_HEADER) GetNextNullFolder.c
GetNextNullItem_SOURCES         = $(COMMON_HEADER) GetNextNullItem.c
GetNextPass_SOURCES             = $(COMMON_HEADER) GetNextPass.c
GetNextWrongObjType_SOURCES     = $(COMMON_HEADER) GetNextWrongObjType.c
GetNullContext_SOURCES          = $(COMMON_HEADER) GetNullContext.c
GetNullItem_SOURCES             = $(COMMON_HEADER) GetNullItem.c
GetNullFolder_SOURCES           = $(COMMON_HEADER) GetNullFolder.c
GetNullName_SOURCES             = $(COMMON_HEADER) GetNullName.c
GetPass_SOURCES                 = $(COMMON_HEADER) GetPass.c
GetStatusNullContext_SOURCES    = $(COMMON_HEADER) GetStatusNullContext.c
GetStatusNullFolder_SOURCES     = $(COMMON_HEADER) GetStatusNullFolder.c
GetStatusNullName_SOURCES       = $(COMMON_HEADER) GetStatusNullName.c
GetStatusNullStatus_SOURCES     = $(COMMON_HEADER) GetStatusNullStatus.c
GetStatusPass_SOURCES           = $(COMMON_HEADER) GetStatusPass.c
GetStatusZeroLength_SOURCES     = $(COMMON_HEADER) GetStatusZeroLength.c
GetWrongObjType_SOURCES         = $(COMMON_HEADER) GetWrongObjType.c
GetZeroNameLength_SOURCES       = $(COMMON_HEADER) GetZeroNameLength.c
InitNullContext_SOURCES         = $(COMMON_HEADER) InitNullContext.c
InitNullFolder_SOURCES          = $(COMMON_HEADER) InitNullFolder.c
InitNullName_SOURCES            = $(COMMON_HEADER) InitNullName.c
InitNullKeyOffset_SOURCES       = $(COMMON_HEADER) InitNullKeyOffset.c
InitNullParentOffset_SOURCES    = $(COMMON_HEADER) InitNullParentOffset.c
InitNullStatus_SOURCES          = $(COMMON_HEADER) InitNullStatus.c
InitPass_SOURCES                = $(COMMON_HEADER) InitPass.c
InitZeroBlocks_SOURCES          = $(COMMON_HEADER) InitZeroBlocks.c
InitZeroNameLength_SOURCES      = $(COMMON_HEADER) InitZeroNameLength.c
InsertInvalidSig_SOURCES        = $(COMMON_HEADER) InsertInvalidSig.c
InsertNullContext_SOURCES       = $(COMMON_HEADER) InsertNullContext.c
InsertNullDefinition_SOURCES    = $(COMMON_HEADER) InsertNullDefinition.c
InsertNullFolder_SOURCES        = $(COMMON_HEADER) InsertNullFolder.c
InsertNullName_SOURCES          = $(COMMON_HEADER) InsertNullName.c
InsertNullOrigName_SOURCES      = $(COMMON_HEADER) InsertNullOrigName.c
InsertPass_SOURCES              = $(COMMON_HEADER) InsertPass.c
InsertWrongObjType_SOURCES      = $(COMMON_HEADER) InsertWrongObjType.c
InsertZeroBlocks_SOURCES        = $(COMMON_HEADER) InsertZeroBlocks.c
InsertZeroNameLength_SOURCES    = $(COMMON_HEADER) InsertZeroNameLength.c
OpenNullContext_SOURCES         = $(COMMON_HEADER) OpenNullContext.c
OpenNullItem_SOURCES            = $(COMMON_HEADER) OpenNullItem.c
OpenNullFolder_SOURCES          = $(COMMON_HEADER) OpenNullFolder.c
OpenNullName_SOURCES            = $(COMMON_HEADER) OpenNullName.c
OpenPass_SOURCES                = $(COMMON_HEADER) OpenPass.c
ReleaseNullContext_SOURCES      = $(COMMON_HEADER) ReleaseNullContext.c
ReleaseNullFolder_SOURCES       = $(COMMON_HEADER) ReleaseNullFolder.c
ReleaseNullItem_SOURCES         = $(COMMON_HEADER) ReleaseNullItem.c
ReleasePass_SOURCES             = $(COMMON_HEADER) ReleasePass.c
ReleaseWrongObjType_SOURCES     = $(COMMON_HEADER) ReleaseWrongObjType.c
RemoveNullContext_SOURCES       = $(COMMON_HEADER) RemoveNullContext.c
RemoveNullFolder_SOURCES        = $(COMMON_HEADER) RemoveNullFolder.c
RemoveNullItem_SOURCES          = $(COMMON_HEADER) RemoveNullItem.c
RemovePass_SOURCES              = $(COMMON_HEADER) RemovePass.c
RemoveWrongObjType_SOURCES      = $(COMMON_HEADER) RemoveWrongObjType.c
StatusNullFolder_SOURCES        = $(COMMON_HEADER) StatusNullFolder.c
StatusNullStatus_SOURCES        = $(COMMON_HEADER) StatusNullStatus.c
StatusPass_SOURCES              = $(COMMON_HEADER) StatusPass.c
Tests_SOURCES                   = $(COMMON_HEADER) Tests.c
TopStatusNullContext_SOURCES    = $(COMMON_HEADER) TopStatusNullContext.c
TopStatusNullFolder_SOURCES     = $(COMMON_HEADER) TopStatusNullFolder.c
TopStatusNullName_SOURCES       = $(COMMON_HEADER) TopStatusNullName.c
TopStatusNullStatus_SOURCES     = $(COMMON_HEADER) TopStatusNullStatus.c
TopStatusPass_SOURCES           = $(COMMON_HEADER) TopStatusPass.c
ValidatePass_SOURCES            = $(COMMON_HEADER) ValidatePass.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                         \
        CloseNullContext        \
        CloseNullItem           \
        ClosePass               \
        CreateInvalidNameLength \
        CreateNullContext       \
        CreateNullDefinition    \
        CreateNullFolder        \
        CreateNullName          \
        CreatePass              \
        CreateWrongType         \
        DeleteInvalidSig        \
        DeleteNullContext       \
        DeleteNullFolder        \
        DeleteNullName          \
        DeletePass              \
        DeleteWrongObjType      \
        DeleteZeroNameLength    \
        DestroyNullContext      \
        DestroyNullFolder       \
        DestroyNullName         \
        DestroyPass             \
        FiniInvalidSig          \
        FiniNullContext         \
        FiniNullFolder          \
        FiniPass                \
        FiniWrongObjType        \
        GetFirstEmpty           \
        GetFirstNullContext     \
        GetFirstNullFolder      \
        GetFirstNullItem        \
        GetFirstPass            \
        GetFirstWrongObjType    \
        GetInvalidSig           \
        GetNextItemNullHashItem \
        GetNextItemNullOffset   \
        GetNextNullContext      \
        GetNextNullFolder       \
        GetNextNullItem         \
        GetNextPass             \
        GetNextWrongObjType     \
        GetNullContext          \
        GetNullItem             \
        GetNullFolder           \
        GetNullName             \
        GetPass                 \
        GetStatusNullContext    \
        GetStatusNullFolder     \
        GetStatusNullName       \
        GetStatusNullStatus     \
        GetStatusPass           \
        GetStatusZeroLength     \
        GetWrongObjType         \
        GetZeroNameLength       \
        InitNullContext         \
        InitNullFolder          \
        InitNullKeyOffset       \
        InitNullName            \
        InitNullParentOffset    \
        InitNullStatus          \
        InitPass                \
        InitZeroBlocks          \
        InitZeroNameLength      \
        InsertInvalidSig        \
        InsertNullContext       \
        InsertNullDefinition    \
        InsertNullFolder        \
        InsertNullName          \
        InsertNullOrigName      \
        InsertPass              \
        InsertWrongObjType      \
        InsertZeroBlocks        \
        InsertZeroNameLength    \
        OpenNullContext         \
        OpenNullItem            \
        OpenNullFolder          \
        OpenNullName            \
        OpenPass                \
        ReleaseNullContext      \
        ReleaseNullFolder       \
        ReleaseNullItem         \
        ReleasePass             \
        ReleaseWrongObjType     \
        RemoveNullContext       \
        RemoveNullFolder        \
        RemoveNullItem          \
        RemovePass              \
        RemoveWrongObjType      \
        StatusNullFolder        \
        StatusNullStatus        \
        StatusPass              \
        Tests                   \
        TopStatusNullContext    \
        TopStatusNullFolder     \
        TopStatusNullName       \
        TopStatusNullStatus     \
        TopStatusPass           \
        ValidatePass            

XFAIL_TESTS =                   \
        CloseNullContext        \
        CloseNullItem           \
        CreateNullContext       \
        CreateNullDefinition    \
        CreateNullFolder        \
        CreateNullName          \
        CreateWrongType         \
        DeleteInvalidSig        \
        DeleteNullContext       \
        DeleteNullFolder        \
        DeleteNullName          \
        DeleteWrongObjType      \
        DeleteZeroNameLength    \
        DestroyNullContext      \
        DestroyNullFolder       \
        DestroyNullName         \
        FiniInvalidSig          \
        FiniNullContext         \
        FiniNullFolder          \
        FiniWrongObjType        \
        GetFirstNullContext     \
        GetFirstNullFolder      \
        GetFirstNullItem        \
        GetFirstWrongObjType    \
        GetInvalidSig           \
        GetNextItemNullHashItem \
        GetNextItemNullOffset   \
        GetNextNullContext      \
        GetNextNullFolder       \
        GetNextNullItem         \
        GetNextWrongObjType     \
        GetNullContext          \
        GetNullItem             \
        GetNullFolder           \
        GetNullName             \
        GetStatusNullContext    \
        GetStatusNullFolder     \
        GetStatusNullName       \
        GetStatusNullStatus     \
        GetStatusZeroLength     \
        GetWrongObjType         \
        GetZeroNameLength       \
        InitNullContext         \
        InitNullFolder          \
        InitNullKeyOffset       \
        InitNullName            \
        InitNullParentOffset    \
        InitNullStatus          \
        InitZeroBlocks          \
        InitZeroNameLength      \
        InsertInvalidSig        \
        InsertNullContext       \
        InsertNullDefinition    \
        InsertNullFolder        \
        InsertNullName          \
        InsertNullOrigName      \
        InsertWrongObjType      \
        InsertZeroBlocks        \
        InsertZeroNameLength    \
        OpenNullContext         \
        OpenNullItem            \
        OpenNullFolder          \
        OpenNullName            \
        ReleaseNullContext      \
        ReleaseNullFolder       \
        ReleaseNullItem         \
        ReleaseWrongObjType     \
        RemoveNullContext       \
        RemoveNullFolder        \
        RemoveNullItem          \
        RemoveWrongObjType      \
        StatusNullFolder        \
        StatusNullStatus        \
        TopStatusNullContext    \
        TopStatusNullFolder     \
        TopStatusNullName       \
        TopStatusNullStatus     

LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

