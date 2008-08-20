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

check_PROGRAMS =                   \
        CommitEditNullContext      \
        CommitEditNullFolder       \
        CommitEditNullItem         \
        CommitEditPass             \
        CommitEditWrongObjType     \
        CreateInvalidNameLength    \
        CreateNullContext          \
        CreateNullDefinition       \
        CreateNullFolder           \
        CreateNullName             \
        CreatePass                 \
        CreateWrongType            \
        DeleteNullContext          \
        DeleteNullFolder           \
        DeleteNullName             \
        DeletePass                 \
        DeleteWrongObjType         \
        DeleteZeroNameLength       \
        DestroyNullContext         \
        DestroyNullFolder          \
        DestroyNullName            \
        DestroyPass                \
        EditNullContext            \
        EditNullFolder             \
        EditNullItem               \
        EditNullName               \
        EditPass                   \
        EditWrongObjectType        \
        EditZeroNameLength         \
        FiniNullContext            \
        FiniNullFolder             \
        FiniPass                   \
        FiniWrongObjType           \
        GetFirstEmpty              \
        GetFirstNullContext        \
        GetFirstNullFolder         \
        GetFirstNullItem           \
        GetFirstPass               \
        GetFirstWrongObjType       \
        GetInvalidSig              \
        GetNextItemNullHashItem    \
        GetNextItemNullOffset      \
        GetNextNullContext         \
        GetNextNullFolder          \
        GetNextNullItem            \
        GetNextPass                \
        GetNextWrongObjType        \
        GetNullContext             \
        GetNullItem                \
        GetNullFolder              \
        GetNullName                \
        GetPass                    \
        GetStatusNullContext       \
        GetStatusNullFolder        \
        GetStatusNullName          \
        GetStatusNullStatus        \
        GetStatusPass              \
        GetStatusZeroLength        \
        GetWrongObjType            \
        GetZeroNameLength          \
        InitNullContext            \
        InitNullFolder             \
        InitNullHashItemOffset     \
        InitNullName               \
        InitNullParentOffset       \
        InitNullStatus             \
        InitPass                   \
        InitZeroBlocks             \
        InitZeroNameLength         \
        InsertNullContext          \
        InsertNullDefinition       \
        InsertNullFolder           \
        InsertNullName             \
        InsertNullOrigName         \
        InsertPass                 \
        InsertWrongObjType         \
        InsertZeroBlocks           \
        InsertZeroNameLength       \
        ReleaseNullContext         \
        ReleaseNullFolder          \
        ReleaseNullItem            \
        ReleasePass                \
        ReleaseWrongObjType        \
        RemoveNullContext          \
        RemoveNullFolder           \
        RemoveNullItem             \
        RemovePass                 \
        RemoveWrongObjType         \
        ResizeNullContext          \
        ResizeNullFolder           \
        ResizePass                 \
        StatusNullFolder           \
        StatusNullStatus           \
        StatusPass                 \
        Tests                      \
        TopCloseNullContext        \
        TopCloseNullItem           \
        TopClosePass               \
        TopCreateInvalidNameLength \
        TopCreateNullContext       \
        TopCreateNullDefinition    \
        TopCreateNullFolder        \
        TopCreateNullName          \
        TopCreatePass              \
        TopCreateWrongType         \
        TopDestroyNullContext      \
        TopDestroyNullFolder       \
        TopDestroyNullName         \
        TopDestroyPass             \
        TopEditNullContext         \
        TopEditNullFolder          \
        TopEditNullItem            \
        TopEditNullName            \
        TopEditPass                \
        TopEditWrongLength         \
        TopEditWrongType           \
        TopEditZeroLength          \
        TopOpenNullContext         \
        TopOpenNullItem            \
        TopOpenNullFolder          \
        TopOpenNullName            \
        TopOpenPass                \
        TopOpenWrongLength         \
        TopOpenWrongType           \
        TopOpenZeroLength          \
        TopStatusNullContext       \
        TopStatusNullFolder        \
        TopStatusNullName          \
        TopStatusNullStatus        \
        TopStatusPass              \
        ValidatePass               

COMMON_HEADER = folderTest.h

CreateInvalidNameLength_SOURCES    = $(COMMON_HEADER) CreateInvalidNameLength.c
CreateNullContext_SOURCES          = $(COMMON_HEADER) CreateNullContext.c
CreateNullDefinition_SOURCES       = $(COMMON_HEADER) CreateNullDefinition.c
CreateNullFolder_SOURCES           = $(COMMON_HEADER) CreateNullFolder.c
CreateNullName_SOURCES             = $(COMMON_HEADER) CreateNullName.c
CreatePass_SOURCES                 = $(COMMON_HEADER) CreatePass.c
CreateWrongType_SOURCES            = $(COMMON_HEADER) CreateWrongType.c
DeleteNullContext_SOURCES          = $(COMMON_HEADER) DeleteNullContext.c
DeleteNullFolder_SOURCES           = $(COMMON_HEADER) DeleteNullFolder.c
DeleteNullName_SOURCES             = $(COMMON_HEADER) DeleteNullName.c
DeletePass_SOURCES                 = $(COMMON_HEADER) DeletePass.c
DeleteWrongObjType_SOURCES         = $(COMMON_HEADER) DeleteWrongObjType.c
DeleteZeroNameLength_SOURCES       = $(COMMON_HEADER) DeleteZeroNameLength.c
DestroyNullContext_SOURCES         = $(COMMON_HEADER) DestroyNullContext.c
DestroyNullFolder_SOURCES          = $(COMMON_HEADER) DestroyNullFolder.c
DestroyNullName_SOURCES            = $(COMMON_HEADER) DestroyNullName.c
DestroyPass_SOURCES                = $(COMMON_HEADER) DestroyPass.c
EditNullContext_SOURCES            = $(COMMON_HEADER) EditNullContext.c
EditNullFolder_SOURCES             = $(COMMON_HEADER) EditNullFolder.c
EditNullItem_SOURCES               = $(COMMON_HEADER) EditNullItem.c
EditNullName_SOURCES               = $(COMMON_HEADER) EditNullName.c
EditPass_SOURCES                   = $(COMMON_HEADER) EditPass.c
EditWrongObjectType_SOURCES        = $(COMMON_HEADER) EditWrongObjectType.c
EditZeroNameLength_SOURCES         = $(COMMON_HEADER) EditZeroNameLength.c
FiniNullContext_SOURCES            = $(COMMON_HEADER) FiniNullContext.c
FiniNullFolder_SOURCES             = $(COMMON_HEADER) FiniNullFolder.c
FiniPass_SOURCES                   = $(COMMON_HEADER) FiniPass.c
FiniWrongObjType_SOURCES           = $(COMMON_HEADER) FiniWrongObjType.c
GetFirstEmpty_SOURCES              = $(COMMON_HEADER) GetFirstEmpty.c
GetFirstNullContext_SOURCES        = $(COMMON_HEADER) GetFirstNullContext.c
GetFirstNullFolder_SOURCES         = $(COMMON_HEADER) GetFirstNullFolder.c
GetFirstNullItem_SOURCES           = $(COMMON_HEADER) GetFirstNullItem.c
GetFirstPass_SOURCES               = $(COMMON_HEADER) GetFirstPass.c
GetFirstWrongObjType_SOURCES       = $(COMMON_HEADER) GetFirstWrongObjType.c
GetInvalidSig_SOURCES              = $(COMMON_HEADER) GetInvalidSig.c
GetNextItemNullHashItem_SOURCES    = $(COMMON_HEADER) GetNextItemNullHashItem.c
GetNextItemNullOffset_SOURCES      = $(COMMON_HEADER) GetNextItemNullOffset.c
GetNextNullContext_SOURCES         = $(COMMON_HEADER) GetNextNullContext.c
GetNextNullFolder_SOURCES          = $(COMMON_HEADER) GetNextNullFolder.c
GetNextNullItem_SOURCES            = $(COMMON_HEADER) GetNextNullItem.c
GetNextPass_SOURCES                = $(COMMON_HEADER) GetNextPass.c
GetNextWrongObjType_SOURCES        = $(COMMON_HEADER) GetNextWrongObjType.c
GetNullContext_SOURCES             = $(COMMON_HEADER) GetNullContext.c
GetNullItem_SOURCES                = $(COMMON_HEADER) GetNullItem.c
GetNullFolder_SOURCES              = $(COMMON_HEADER) GetNullFolder.c
GetNullName_SOURCES                = $(COMMON_HEADER) GetNullName.c
GetPass_SOURCES                    = $(COMMON_HEADER) GetPass.c
GetStatusNullContext_SOURCES       = $(COMMON_HEADER) GetStatusNullContext.c
GetStatusNullFolder_SOURCES        = $(COMMON_HEADER) GetStatusNullFolder.c
GetStatusNullName_SOURCES          = $(COMMON_HEADER) GetStatusNullName.c
GetStatusNullStatus_SOURCES        = $(COMMON_HEADER) GetStatusNullStatus.c
GetStatusPass_SOURCES              = $(COMMON_HEADER) GetStatusPass.c
GetStatusZeroLength_SOURCES        = $(COMMON_HEADER) GetStatusZeroLength.c
GetWrongObjType_SOURCES            = $(COMMON_HEADER) GetWrongObjType.c
GetZeroNameLength_SOURCES          = $(COMMON_HEADER) GetZeroNameLength.c
InitNullContext_SOURCES            = $(COMMON_HEADER) InitNullContext.c
InitNullFolder_SOURCES             = $(COMMON_HEADER) InitNullFolder.c
InitNullName_SOURCES               = $(COMMON_HEADER) InitNullName.c
InitNullHashItemOffset_SOURCES     = $(COMMON_HEADER) InitNullHashItemOffset.c
InitNullParentOffset_SOURCES       = $(COMMON_HEADER) InitNullParentOffset.c
InitNullStatus_SOURCES             = $(COMMON_HEADER) InitNullStatus.c
InitPass_SOURCES                   = $(COMMON_HEADER) InitPass.c
InitZeroBlocks_SOURCES             = $(COMMON_HEADER) InitZeroBlocks.c
InitZeroNameLength_SOURCES         = $(COMMON_HEADER) InitZeroNameLength.c
InsertNullContext_SOURCES          = $(COMMON_HEADER) InsertNullContext.c
InsertNullDefinition_SOURCES       = $(COMMON_HEADER) InsertNullDefinition.c
InsertNullFolder_SOURCES           = $(COMMON_HEADER) InsertNullFolder.c
InsertNullName_SOURCES             = $(COMMON_HEADER) InsertNullName.c
InsertNullOrigName_SOURCES         = $(COMMON_HEADER) InsertNullOrigName.c
InsertPass_SOURCES                 = $(COMMON_HEADER) InsertPass.c
InsertWrongObjType_SOURCES         = $(COMMON_HEADER) InsertWrongObjType.c
InsertZeroBlocks_SOURCES           = $(COMMON_HEADER) InsertZeroBlocks.c
InsertZeroNameLength_SOURCES       = $(COMMON_HEADER) InsertZeroNameLength.c
ReleaseNullContext_SOURCES         = $(COMMON_HEADER) ReleaseNullContext.c
ReleaseNullFolder_SOURCES          = $(COMMON_HEADER) ReleaseNullFolder.c
ReleaseNullItem_SOURCES            = $(COMMON_HEADER) ReleaseNullItem.c
ReleasePass_SOURCES                = $(COMMON_HEADER) ReleasePass.c
ReleaseWrongObjType_SOURCES        = $(COMMON_HEADER) ReleaseWrongObjType.c
RemoveNullContext_SOURCES          = $(COMMON_HEADER) RemoveNullContext.c
RemoveNullFolder_SOURCES           = $(COMMON_HEADER) RemoveNullFolder.c
RemoveNullItem_SOURCES             = $(COMMON_HEADER) RemoveNullItem.c
RemovePass_SOURCES                 = $(COMMON_HEADER) RemovePass.c
RemoveWrongObjType_SOURCES         = $(COMMON_HEADER) RemoveWrongObjType.c
ResizeNullContext_SOURCES          = $(COMMON_HEADER) ResizeNullContext.c
ResizeNullFolder_SOURCES           = $(COMMON_HEADER) ResizeNullFolder.c
ResizePass_SOURCES                 = $(COMMON_HEADER) ResizePass.c
StatusNullFolder_SOURCES           = $(COMMON_HEADER) StatusNullFolder.c
StatusNullStatus_SOURCES           = $(COMMON_HEADER) StatusNullStatus.c
StatusPass_SOURCES                 = $(COMMON_HEADER) StatusPass.c
Tests_SOURCES                      = $(COMMON_HEADER) Tests.c
TopCloseNullContext_SOURCES        = $(COMMON_HEADER) TopCloseNullContext.c
TopCloseNullItem_SOURCES           = $(COMMON_HEADER) TopCloseNullItem.c
TopClosePass_SOURCES               = $(COMMON_HEADER) TopClosePass.c
TopCreateInvalidNameLength_SOURCES = $(COMMON_HEADER) TopCreateInvalidNameLength.c
TopCreateNullContext_SOURCES       = $(COMMON_HEADER) TopCreateNullContext.c
TopCreateNullDefinition_SOURCES    = $(COMMON_HEADER) TopCreateNullDefinition.c
TopCreateNullFolder_SOURCES        = $(COMMON_HEADER) TopCreateNullFolder.c
TopCreateNullName_SOURCES          = $(COMMON_HEADER) TopCreateNullName.c
TopCreatePass_SOURCES              = $(COMMON_HEADER) TopCreatePass.c
TopCreateWrongType_SOURCES         = $(COMMON_HEADER) TopCreateWrongType.c
TopDestroyNullContext_SOURCES      = $(COMMON_HEADER) TopDestroyNullContext.c
TopDestroyNullFolder_SOURCES       = $(COMMON_HEADER) TopDestroyNullFolder.c
TopDestroyNullName_SOURCES         = $(COMMON_HEADER) TopDestroyNullName.c
TopDestroyPass_SOURCES             = $(COMMON_HEADER) TopDestroyPass.c
TopEditNullContext_SOURCES         = $(COMMON_HEADER) TopEditNullContext.c
TopEditNullFolder_SOURCES          = $(COMMON_HEADER) TopEditNullFolder.c
TopEditNullItem_SOURCES            = $(COMMON_HEADER) TopEditNullItem.c
TopEditNullName_SOURCES            = $(COMMON_HEADER) TopEditNullName.c
TopEditPass_SOURCES                = $(COMMON_HEADER) TopEditPass.c
TopEditWrongLength_SOURCES         = $(COMMON_HEADER) TopEditWrongLength.c
TopEditWrongType_SOURCES           = $(COMMON_HEADER) TopEditWrongType.c
TopEditZeroLength_SOURCES          = $(COMMON_HEADER) TopEditZeroLength.c
TopOpenNullContext_SOURCES         = $(COMMON_HEADER) TopOpenNullContext.c
TopOpenNullItem_SOURCES            = $(COMMON_HEADER) TopOpenNullItem.c
TopOpenNullFolder_SOURCES          = $(COMMON_HEADER) TopOpenNullFolder.c
TopOpenNullName_SOURCES            = $(COMMON_HEADER) TopOpenNullName.c
TopOpenPass_SOURCES                = $(COMMON_HEADER) TopOpenPass.c
TopOpenWrongLength_SOURCES         = $(COMMON_HEADER) TopOpenWrongLength.c
TopOpenWrongType_SOURCES           = $(COMMON_HEADER) TopOpenWrongType.c
TopOpenZeroLength_SOURCES          = $(COMMON_HEADER) TopOpenZeroLength.c
TopStatusNullContext_SOURCES       = $(COMMON_HEADER) TopStatusNullContext.c
TopStatusNullFolder_SOURCES        = $(COMMON_HEADER) TopStatusNullFolder.c
TopStatusNullName_SOURCES          = $(COMMON_HEADER) TopStatusNullName.c
TopStatusNullStatus_SOURCES        = $(COMMON_HEADER) TopStatusNullStatus.c
TopStatusPass_SOURCES              = $(COMMON_HEADER) TopStatusPass.c
ValidatePass_SOURCES               = $(COMMON_HEADER) ValidatePass.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                            \
        CommitEditNullContext      \
        CommitEditNullFolder       \
        CommitEditNullItem         \
        CommitEditPass             \
        CommitEditWrongObjType     \
        CreateInvalidNameLength    \
        CreateNullContext          \
        CreateNullDefinition       \
        CreateNullFolder           \
        CreateNullName             \
        CreatePass                 \
        CreateWrongType            \
        DeleteNullContext          \
        DeleteNullFolder           \
        DeleteNullName             \
        DeletePass                 \
        DeleteWrongObjType         \
        DeleteZeroNameLength       \
        DestroyNullContext         \
        DestroyNullFolder          \
        DestroyNullName            \
        DestroyPass                \
        EditNullContext            \
        EditNullFolder             \
        EditNullItem               \
        EditNullName               \
        EditPass                   \
        EditWrongObjectType        \
        EditZeroNameLength         \
        FiniNullContext            \
        FiniNullFolder             \
        FiniPass                   \
        FiniWrongObjType           \
        GetFirstEmpty              \
        GetFirstNullContext        \
        GetFirstNullFolder         \
        GetFirstNullItem           \
        GetFirstPass               \
        GetFirstWrongObjType       \
        GetInvalidSig              \
        GetNextItemNullHashItem    \
        GetNextItemNullOffset      \
        GetNextNullContext         \
        GetNextNullFolder          \
        GetNextNullItem            \
        GetNextPass                \
        GetNextWrongObjType        \
        GetNullContext             \
        GetNullItem                \
        GetNullFolder              \
        GetNullName                \
        GetPass                    \
        GetStatusNullContext       \
        GetStatusNullFolder        \
        GetStatusNullName          \
        GetStatusNullStatus        \
        GetStatusPass              \
        GetStatusZeroLength        \
        GetWrongObjType            \
        GetZeroNameLength          \
        InitNullContext            \
        InitNullFolder             \
        InitNullHashItemOffset     \
        InitNullName               \
        InitNullParentOffset       \
        InitNullStatus             \
        InitPass                   \
        InitZeroBlocks             \
        InitZeroNameLength         \
        InsertNullContext          \
        InsertNullDefinition       \
        InsertNullFolder           \
        InsertNullName             \
        InsertNullOrigName         \
        InsertPass                 \
        InsertWrongObjType         \
        InsertZeroBlocks           \
        InsertZeroNameLength       \
        ReleaseNullContext         \
        ReleaseNullFolder          \
        ReleaseNullItem            \
        ReleasePass                \
        ReleaseWrongObjType        \
        RemoveNullContext          \
        RemoveNullFolder           \
        RemoveNullItem             \
        RemovePass                 \
        RemoveWrongObjType         \
        ResizeNullContext          \
        ResizeNullFolder           \
        ResizePass                 \
        StatusNullFolder           \
        StatusNullStatus           \
        StatusPass                 \
        Tests                      \
        TopCloseNullContext        \
        TopCloseNullItem           \
        TopClosePass               \
        TopCreateInvalidNameLength \
        TopCreateNullContext       \
        TopCreateNullDefinition    \
        TopCreateNullFolder        \
        TopCreateNullName          \
        TopCreatePass              \
        TopCreateWrongType         \
        TopDestroyNullContext      \
        TopDestroyNullFolder       \
        TopDestroyNullName         \
        TopDestroyPass             \
        TopEditNullContext         \
        TopEditNullFolder          \
        TopEditNullItem            \
        TopEditNullName            \
        TopEditPass                \
        TopEditWrongLength         \
        TopEditWrongType           \
        TopEditZeroLength          \
        TopOpenNullContext         \
        TopOpenNullItem            \
        TopOpenNullFolder          \
        TopOpenNullName            \
        TopOpenPass                \
        TopOpenWrongLength         \
        TopOpenWrongType           \
        TopOpenZeroLength          \
        TopStatusNullContext       \
        TopStatusNullFolder        \
        TopStatusNullName          \
        TopStatusNullStatus        \
        TopStatusPass              \
        ValidatePass            

XFAIL_TESTS =                      \
        CommitEditNullContext      \
        CommitEditNullFolder       \
        CommitEditNullItem         \
        CommitEditWrongObjType     \
        CreateNullContext          \
        CreateNullDefinition       \
        CreateNullFolder           \
        CreateNullName             \
        CreateWrongType            \
        DeleteNullContext          \
        DeleteNullFolder           \
        DeleteNullName             \
        DeleteWrongObjType         \
        DeleteZeroNameLength       \
        DestroyNullContext         \
        DestroyNullFolder          \
        DestroyNullName            \
        EditNullContext            \
        EditNullFolder             \
        EditNullItem               \
        EditNullName               \
        EditWrongObjectType        \
        EditZeroNameLength         \
        FiniNullContext            \
        FiniNullFolder             \
        FiniWrongObjType           \
        GetFirstNullContext        \
        GetFirstNullFolder         \
        GetFirstNullItem           \
        GetFirstWrongObjType       \
        GetInvalidSig              \
        GetNextItemNullHashItem    \
        GetNextItemNullOffset      \
        GetNextNullContext         \
        GetNextNullFolder          \
        GetNextNullItem            \
        GetNextWrongObjType        \
        GetNullContext             \
        GetNullItem                \
        GetNullFolder              \
        GetNullName                \
        GetStatusNullContext       \
        GetStatusNullFolder        \
        GetStatusNullName          \
        GetStatusNullStatus        \
        GetStatusZeroLength        \
        GetWrongObjType            \
        GetZeroNameLength          \
        InitNullContext            \
        InitNullFolder             \
        InitNullHashItemOffset     \
        InitNullName               \
        InitNullParentOffset       \
        InitNullStatus             \
        InitZeroBlocks             \
        InitZeroNameLength         \
        InsertNullContext          \
        InsertNullDefinition       \
        InsertNullFolder           \
        InsertNullName             \
        InsertNullOrigName         \
        InsertWrongObjType         \
        InsertZeroBlocks           \
        InsertZeroNameLength       \
        ReleaseNullContext         \
        ReleaseNullFolder          \
        ReleaseNullItem            \
        ReleaseWrongObjType        \
        RemoveNullContext          \
        RemoveNullFolder           \
        RemoveNullItem             \
        RemoveWrongObjType         \
        ResizeNullContext          \
        ResizeNullFolder           \
        StatusNullFolder           \
        StatusNullStatus           \
        TopCloseNullContext        \
        TopCloseNullItem           \
        TopCreateNullContext       \
        TopCreateNullDefinition    \
        TopCreateNullFolder        \
        TopCreateNullName          \
        TopCreateWrongType         \
        TopDestroyNullContext      \
        TopDestroyNullFolder       \
        TopDestroyNullName         \
        TopEditNullContext         \
        TopEditNullFolder          \
        TopEditNullItem            \
        TopEditNullName            \
        TopEditWrongType           \
        TopOpenNullContext         \
        TopOpenNullItem            \
        TopOpenNullFolder          \
        TopOpenNullName            \
        TopOpenWrongType           \
        TopStatusNullContext       \
        TopStatusNullFolder        \
        TopStatusNullName          \
        TopStatusNullStatus        

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la
