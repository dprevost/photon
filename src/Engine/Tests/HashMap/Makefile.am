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

check_PROGRAMS =                      \
                 DeleteNullContext    \
                 DeleteNullHash       \
                 DeleteNullKey        \
                 DeletePass           \
                 DeleteZeroKeyLength  \
                 FiniNullContext      \
                 FiniNullHash         \
                 FiniPass             \
                 GetNullContext       \
                 GetNullHash          \
                 GetNullItem          \
                 GetNullKey           \
                 GetPass              \
                 GetZeroKeyLength     \
                 InitNullContext      \
                 InitNullHash         \
                 InitNullName         \
                 InitNullParent       \
                 InitNullStatus       \
                 InitPass             \
                 InitZeroBlocks       \
                 InitZeroLength       \
                 InsertNullContext    \
                 InsertNullHash       \
                 InsertNullItem       \
                 InsertNullKey        \
                 InsertPass           \
                 InsertZeroLengthItem \
                 InsertZeroLengthKey  

COMMON_HEADER = hashMapTest.h

DeleteNullContext_SOURCES    = $(COMMON_HEADER) DeleteNullContext.c
DeleteNullHash_SOURCES       = $(COMMON_HEADER) DeleteNullHash.c
DeleteNullKey_SOURCES        = $(COMMON_HEADER) DeleteNullKey.c
DeletePass_SOURCES           = $(COMMON_HEADER) DeletePass.c
DeleteZeroKeyLength_SOURCES  = $(COMMON_HEADER) DeleteZeroKeyLength.c
FiniNullContext_SOURCES      = $(COMMON_HEADER) FiniNullContext.c
FiniNullHash_SOURCES         = $(COMMON_HEADER) FiniNullHash.c
FiniPass_SOURCES             = $(COMMON_HEADER) FiniPass.c
GetNullContext_SOURCES       = $(COMMON_HEADER) GetNullContext.c
GetNullHash_SOURCES          = $(COMMON_HEADER) GetNullHash.c
GetNullItem_SOURCES          = $(COMMON_HEADER) GetNullItem.c
GetNullKey_SOURCES           = $(COMMON_HEADER) GetNullKey.c
GetPass_SOURCES              = $(COMMON_HEADER) GetPass.c
GetZeroKeyLength_SOURCES     = $(COMMON_HEADER) GetZeroKeyLength.c
InitNullContext_SOURCES      = $(COMMON_HEADER) InitNullContext.c
InitNullHash_SOURCES         = $(COMMON_HEADER) InitNullHash.c
InitNullName_SOURCES         = $(COMMON_HEADER) InitNullName.c
InitNullParent_SOURCES       = $(COMMON_HEADER) InitNullParent.c
InitNullStatus_SOURCES       = $(COMMON_HEADER) InitNullStatus.c
InitPass_SOURCES             = $(COMMON_HEADER) InitPass.c
InitZeroBlocks_SOURCES       = $(COMMON_HEADER) InitZeroBlocks.c
InitZeroLength_SOURCES       = $(COMMON_HEADER) InitZeroLength.c
InsertNullContext_SOURCES    = $(COMMON_HEADER) InsertNullContext.c
InsertNullHash_SOURCES       = $(COMMON_HEADER) InsertNullHash.c
InsertNullItem_SOURCES       = $(COMMON_HEADER) InsertNullItem.c
InsertNullKey_SOURCES        = $(COMMON_HEADER) InsertNullKey.c
InsertPass_SOURCES           = $(COMMON_HEADER) InsertPass.c
InsertZeroLengthItem_SOURCES = $(COMMON_HEADER) InsertZeroLengthItem.c
InsertZeroLengthKey_SOURCES  = $(COMMON_HEADER) InsertZeroLengthKey.c


EXTRA_DIST = 

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS =                      \
        DeleteNullContext    \
        DeleteNullHash       \
        DeleteNullKey        \
        DeletePass           \
        DeleteZeroKeyLength  \
        FiniNullContext      \
        FiniNullHash         \
        FiniPass             \
        GetNullContext       \
        GetNullHash          \
        GetNullItem          \
        GetNullKey           \
        GetPass              \
        GetZeroKeyLength     \
        InitNullContext      \
        InitNullHash         \
        InitNullName         \
        InitNullParent       \
        InitNullStatus       \
        InitPass             \
        InitZeroBlocks       \
        InitZeroLength       \
        InsertNullContext    \
        InsertNullHash       \
        InsertNullItem       \
        InsertNullKey        \
        InsertPass           \
        InsertZeroLengthItem \
        InsertZeroLengthKey  

XFAIL_TESTS =                      \
              DeleteNullContext    \
              DeleteNullHash       \
              DeleteNullKey        \
              DeleteZeroKeyLength  \
              FiniNullContext      \
              FiniNullHash         \
              GetNullContext       \
              GetNullHash          \
              GetNullItem          \
              GetNullKey           \
              GetZeroKeyLength     \
              InitNullContext      \
              InitNullHash         \
              InitNullName         \
              InitNullParent       \
              InitNullStatus       \
              InitZeroBlocks       \
              InitZeroLength       \
              InsertNullContext    \
              InsertNullHash       \
              InsertNullItem       \
              InsertNullKey        \
              InsertZeroLengthItem \
              InsertZeroLengthKey  

LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

