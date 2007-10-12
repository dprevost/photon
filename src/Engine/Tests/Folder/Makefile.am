# Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

check_PROGRAMS =                         \
                 CloseNullContext        \
                 CloseNullItem           \
                 ClosePass               \
                 CreateNullContext       \
                 CreateNullFolder        \
                 CreateNullName          \
                 CreatePass              \
                 CreateWrongType         \
                 DeleteInvalidSig        \
                 DeleteNullContext       \
                 DeleteNullFolder        \
                 DeleteNullName          \
                 DeletePass              \
                 DeleteZeroNameLength    \
                 DestroyNullContext      \
                 DestroyNullFolder       \
                 DestroyNullName         \
                 DestroyPass             \
                 FiniInvalidSig          \
                 FiniNullContext         \
                 FiniNullFolder          \
                 FiniPass                \
                 GetFirstNullContext     \
                 GetFirstNullFolder      \
                 GetFirstNullItem        \
                 GetFirstPass            \
                 GetInvalidSig           \
                 GetNextItemNullHashItem \
                 GetNextItemNullOffset   \
                 GetNextNullContext      \
                 GetNextNullFolder       \
                 GetNextNullItem         \
                 GetNextPass             \
                 GetNullContext          \
                 GetNullItem             \
                 GetNullFolder           \
                 GetNullName             \
                 GetPass                 \
                 GetZeroNameLength       \
                 InitNullContext         \
                 InitNullFolder          \
                 InitNullName            \
                 InitNullParentOffset    \
                 InitPass                \
                 InitZeroBlocks          \
                 InitZeroNameLength      \
                 InsertInvalidSig        \
                 InsertNullContext       \
                 InsertNullFolder        \
                 InsertNullName          \
                 InsertNullOrigName      \
                 InsertPass              \
                 InsertZeroBlocks        \
                 InsertZeroNameLength    \
                 OpenNullContext         \
                 OpenNullItem            \
                 OpenNullFolder          \
                 OpenNullName            \
                 OpenPass                \
                 RemoveNullContext       \
                 RemoveNullFolder        \
                 RemoveNullName          \
                 RemovePass              \
                 RemoveZeroLength        \
                 Tests                   

COMMON_HEADER = folderTest.h

CloseNullContext_SOURCES        = $(COMMON_HEADER) CloseNullContext.c
CloseNullItem_SOURCES           = $(COMMON_HEADER) CloseNullItem.c
ClosePass_SOURCES               = $(COMMON_HEADER) ClosePass.c
CreateNullContext_SOURCES       = $(COMMON_HEADER) CreateNullContext.c
CreateNullFolder_SOURCES        = $(COMMON_HEADER) CreateNullFolder.c
CreateNullName_SOURCES          = $(COMMON_HEADER) CreateNullName.c
CreatePass_SOURCES              = $(COMMON_HEADER) CreatePass.c
CreateWrongType_SOURCES         = $(COMMON_HEADER) CreateWrongType.c
DestroyNullContext_SOURCES      = $(COMMON_HEADER) DestroyNullContext.c
DestroyNullFolder_SOURCES       = $(COMMON_HEADER) DestroyNullFolder.c
DestroyNullName_SOURCES         = $(COMMON_HEADER) DestroyNullName.c
DestroyPass_SOURCES             = $(COMMON_HEADER) DestroyPass.c
FiniInvalidSig_SOURCES          = $(COMMON_HEADER) FiniInvalidSig.c
FiniNullContext_SOURCES         = $(COMMON_HEADER) FiniNullContext.c
FiniNullFolder_SOURCES          = $(COMMON_HEADER) FiniNullFolder.c
FiniPass_SOURCES                = $(COMMON_HEADER) FiniPass.c
GetFirstNullContext_SOURCES     = $(COMMON_HEADER) GetFirstNullContext.c
GetFirstNullFolder_SOURCES      = $(COMMON_HEADER) GetFirstNullFolder.c
GetFirstNullItem_SOURCES        = $(COMMON_HEADER) GetFirstNullItem.c
GetFirstPass_SOURCES            = $(COMMON_HEADER) GetFirstPass.c
GetInvalidSig_SOURCES           = $(COMMON_HEADER) GetInvalidSig.c
GetNextItemNullHashItem_SOURCES = $(COMMON_HEADER) GetNextItemNullHashItem.c
GetNextItemNullOffset_SOURCES   = $(COMMON_HEADER) GetNextItemNullOffset.c
GetNextNullContext_SOURCES      = $(COMMON_HEADER) GetNextNullContext.c
GetNextNullFolder_SOURCES       = $(COMMON_HEADER) GetNextNullFolder.c
GetNextNullItem_SOURCES         = $(COMMON_HEADER) GetNextNullItem.c
GetNextPass_SOURCES             = $(COMMON_HEADER) GetNextPass.c
GetNullContext_SOURCES          = $(COMMON_HEADER) GetNullContext.c
GetNullItem_SOURCES             = $(COMMON_HEADER) GetNullItem.c
GetNullFolder_SOURCES           = $(COMMON_HEADER) GetNullFolder.c
GetNullName_SOURCES             = $(COMMON_HEADER) GetNullName.c
GetPass_SOURCES                 = $(COMMON_HEADER) GetPass.c
GetZeroNameLength_SOURCES       = $(COMMON_HEADER) GetZeroNameLength.c
InitNullContext_SOURCES         = $(COMMON_HEADER) InitNullContext.c
InitNullFolder_SOURCES          = $(COMMON_HEADER) InitNullFolder.c
InitNullName_SOURCES            = $(COMMON_HEADER) InitNullName.c
InitNullParentOffset_SOURCES    = $(COMMON_HEADER) InitNullParentOffset.c
InitPass_SOURCES                = $(COMMON_HEADER) InitPass.c
InitZeroBlocks_SOURCES          = $(COMMON_HEADER) InitZeroBlocks.c
InitZeroNameLength_SOURCES      = $(COMMON_HEADER) InitZeroNameLength.c
InsertInvalidSig_SOURCES        = $(COMMON_HEADER) InsertInvalidSig.c
InsertNullContext_SOURCES       = $(COMMON_HEADER) InsertNullContext.c
InsertNullFolder_SOURCES        = $(COMMON_HEADER) InsertNullFolder.c
InsertNullName_SOURCES          = $(COMMON_HEADER) InsertNullName.c
InsertNullOrigName_SOURCES      = $(COMMON_HEADER) InsertNullOrigName.c
InsertPass_SOURCES              = $(COMMON_HEADER) InsertPass.c
InsertZeroBlocks_SOURCES        = $(COMMON_HEADER) InsertZeroBlocks.c
InsertZeroNameLength_SOURCES    = $(COMMON_HEADER) InsertZeroNameLength.c
OpenNullContext_SOURCES         = $(COMMON_HEADER) OpenNullContext.c
OpenNullItem_SOURCES            = $(COMMON_HEADER) OpenNullItem.c
OpenNullFolder_SOURCES          = $(COMMON_HEADER) OpenNullFolder.c
OpenNullName_SOURCES            = $(COMMON_HEADER) OpenNullName.c
OpenPass_SOURCES                = $(COMMON_HEADER) OpenPass.c
RemoveNullContext_SOURCES       = $(COMMON_HEADER) RemoveNullContext.c
RemoveNullFolder_SOURCES        = $(COMMON_HEADER) RemoveNullFolder.c
RemoveNullName_SOURCES          = $(COMMON_HEADER) RemoveNullName.c
RemovePass_SOURCES              = $(COMMON_HEADER) RemovePass.c
RemoveZeroLength_SOURCES        = $(COMMON_HEADER) RemoveZeroLength.c
Tests_SOURCES                   = $(COMMON_HEADER) Tests.c

EXTRA_DIST = 

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS =                         \
        CloseNullContext        \
        CloseNullItem           \
        ClosePass               \
        CreateNullContext       \
        CreateNullFolder        \
        CreateNullName          \
        CreatePass              \
        CreateWrongType         \
        DeleteInvalidSig        \
        DeleteNullContext       \
        DeleteNullFolder        \
        DeleteNullName          \
        DeletePass              \
        DeleteZeroNameLength    \
        DestroyNullContext      \
        DestroyNullFolder       \
        DestroyNullName         \
        DestroyPass             \
        FiniInvalidSig          \
        FiniNullContext         \
        FiniNullFolder          \
        FiniPass                \
        GetFirstNullContext     \
        GetFirstNullFolder      \
        GetFirstNullItem        \
        GetFirstPass            \
        GetInvalidSig           \
        GetNextItemNullHashItem \
        GetNextItemNullOffset   \
        GetNextNullContext      \
        GetNextNullFolder       \
        GetNextNullItem         \
        GetNextPass             \
        GetNullContext          \
        GetNullItem             \
        GetNullFolder           \
        GetNullName             \
        GetPass                 \
        GetZeroNameLength       \
        InitNullContext         \
        InitNullFolder          \
        InitNullName            \
        InitNullParentOffset    \
        InitPass                \
        InitZeroBlocks          \
        InitZeroNameLength      \
        InsertInvalidSig        \
        InsertNullContext       \
        InsertNullFolder        \
        InsertNullName          \
        InsertNullOrigName      \
        InsertPass              \
        InsertZeroBlocks        \
        InsertZeroNameLength    \
        OpenNullContext         \
        OpenNullItem            \
        OpenNullFolder          \
        OpenNullName            \
        OpenPass                \
        RemoveNullContext       \
        RemoveNullFolder        \
        RemoveNullName          \
        RemovePass              \
        RemoveZeroLength        \
        Tests                   
        
XFAIL_TESTS =                         \
              CloseNullContext        \
              CloseNullItem           \
              CreateNullContext       \
              CreateNullFolder        \
              CreateNullName          \
              CreateWrongType         \
              DeleteInvalidSig        \
              DeleteNullContext       \
              DeleteNullFolder        \
              DeleteNullName          \
              DeleteZeroNameLength    \
              DestroyNullContext      \
              DestroyNullFolder       \
              DestroyNullName         \
              FiniInvalidSig          \
              FiniNullContext         \
              FiniNullFolder          \
              GetFirstNullContext     \
              GetFirstNullFolder      \
              GetFirstNullItem        \
              GetInvalidSig           \
              GetNextItemNullHashItem \
              GetNextItemNullOffset   \
              GetNextNullContext      \
              GetNextNullFolder       \
              GetNextNullItem         \
              GetNullContext          \
              GetNullItem             \
              GetNullFolder           \
              GetNullName             \
              GetZeroNameLength       \
              InitNullContext         \
              InitNullFolder          \
              InitNullName            \
              InitNullParentOffset    \
              InitZeroBlocks          \
              InitZeroNameLength      \
              InsertInvalidSig        \
              InsertNullContext       \
              InsertNullFolder        \
              InsertNullName          \
              InsertNullOrigName      \
              InsertZeroBlocks        \
              InsertZeroNameLength    \
              OpenNullContext         \
              OpenNullItem            \
              OpenNullFolder          \
              OpenNullName            \
              RemoveNullContext       \
              RemoveNullFolder        \
              RemoveNullName          \
              RemoveZeroLength        

LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

