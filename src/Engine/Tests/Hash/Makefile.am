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

# The two "real" regression tests are ListTestSmall and ListTestBig.
# I put them at the end to make sure that the interface itself (to 
# the list) is ok. I also put Small before Big since the smaller one
# is doing a lot less iterations but doing many more consistency checks
# (I could have bundled it all together but the test would take too
# much clock time/cpu time). 

AM_CFLAGS = -Wall

check_PROGRAMS =                        \
                 DeleteAtNullContext    \
                 DeleteAtNullHash       \
                 DeleteAtNullItem       \
                 DeleteAtPass           \
                 DeleteNullContext      \
                 DeleteNullHash         \
                 DeleteNullKey          \
                 DeletePass             \
                 DeleteZeroLength       \
                 EmptyNullContext       \
                 EmptyNullHash          \
                 EmptyPass              \
                 FiniNullHash           \
                 FiniPass               \
                 GetFirstNullBucket     \
                 GetFirstNullHash       \
                 GetFirstNullOffset     \
                 GetFirstPass           \
                 GetNextNullHash        \
                 GetNextNullNextBucket  \
                 GetNextNullNextOffset  \
                 GetNextNullPrevOffset  \
                 GetNextPass            \
                 GetNullBucket          \
                 GetNullContext         \
                 GetNullHash            \
                 GetNullItem            \
                 GetNullKey             \
                 GetPass                \
                 GetZeroKeyLength       \
                 InitNullContext        \
                 InitNullHash           \
                 InitNullOffset         \
                 InitPass               \
                 InsertAtEmptyBucket    \
                 InsertAtNullContext    \
                 InsertAtNullData       \
                 InsertAtNullHash       \
                 InsertAtNullKey        \
                 InsertAtNullNewItem    \
                 InsertAtPass           \
                 InsertAtZeroDataLength \
                 InsertAtZeroKeyLength  \
                 InsertNullContext      \
                 InsertNullData         \
                 InsertNullHash         \
                 InsertNullKey          \
                 InsertNullNewItem      \
                 InsertPass             \
                 InsertZeroDataLength   \
                 InsertZeroKeyLength    \
                 ResizeNullContext      \
                 ResizeNullHash         \
                 ResizePass             

COMMON_HEADER = HashTest.h

DeleteAtNullContext_SOURCES    = $(COMMON_HEADER) DeleteAtNullContext.c
DeleteAtNullHash_SOURCES       = $(COMMON_HEADER) DeleteAtNullHash.c
DeleteAtNullItem_SOURCES       = $(COMMON_HEADER) DeleteAtNullItem.c
DeleteAtPass_SOURCES           = $(COMMON_HEADER) DeleteAtPass.c
DeleteNullContext_SOURCES      = $(COMMON_HEADER) DeleteNullContext.c
DeleteNullHash_SOURCES         = $(COMMON_HEADER) DeleteNullHash.c
DeleteNullKey_SOURCES          = $(COMMON_HEADER) DeleteNullKey.c
DeletePass_SOURCES             = $(COMMON_HEADER) DeletePass.c
DeleteZeroLength_SOURCES       = $(COMMON_HEADER) DeleteZeroLength.c
EmptyNullContext_SOURCES       = $(COMMON_HEADER) EmptyNullContext.c
EmptyNullHash_SOURCES          = $(COMMON_HEADER) EmptyNullHash.c
EmptyPass_SOURCES              = $(COMMON_HEADER) EmptyPass.c
FiniNullHash_SOURCES           = $(COMMON_HEADER) FiniNullHash.c
FiniPass_SOURCES               = $(COMMON_HEADER) FiniPass.c
GetFirstNullBucket_SOURCES     = $(COMMON_HEADER) GetFirstNullBucket.c
GetFirstNullHash_SOURCES       = $(COMMON_HEADER) GetFirstNullHash.c
GetFirstNullOffset_SOURCES     = $(COMMON_HEADER) GetFirstNullOffset.c
GetFirstPass_SOURCES           = $(COMMON_HEADER) GetFirstPass.c
GetNextNullHash_SOURCES        = $(COMMON_HEADER) GetNextNullHash.c
GetNextNullNextBucket_SOURCES  = $(COMMON_HEADER) GetNextNullNextBucket.c
GetNextNullNextOffset_SOURCES  = $(COMMON_HEADER) GetNextNullNextOffset.c
GetNextNullPrevOffset_SOURCES  = $(COMMON_HEADER) GetNextNullPrevOffset.c
GetNextPass_SOURCES            = $(COMMON_HEADER) GetNextPass.c
GetNullBucket_SOURCES          = $(COMMON_HEADER) GetNullBucket.c
GetNullContext_SOURCES         = $(COMMON_HEADER) GetNullContext.c
GetNullHash_SOURCES            = $(COMMON_HEADER) GetNullHash.c
GetNullItem_SOURCES            = $(COMMON_HEADER) GetNullItem.c
GetNullKey_SOURCES             = $(COMMON_HEADER) GetNullKey.c
GetPass_SOURCES                = $(COMMON_HEADER) GetPass.c
GetZeroKeyLength_SOURCES       = $(COMMON_HEADER) GetZeroKeyLength.c
InitNullContext_SOURCES        = $(COMMON_HEADER) InitNullContext.c
InitNullHash_SOURCES           = $(COMMON_HEADER) InitNullHash.c
InitNullOffset_SOURCES         = $(COMMON_HEADER) InitNullOffset.c
InitPass_SOURCES               = $(COMMON_HEADER) InitPass.c
InsertAtEmptyBucket_SOURCES    = $(COMMON_HEADER) InsertAtEmptyBucket.c
InsertAtNullContext_SOURCES    = $(COMMON_HEADER) InsertAtNullContext.c
InsertAtNullData_SOURCES       = $(COMMON_HEADER) InsertAtNullData.c
InsertAtNullHash_SOURCES       = $(COMMON_HEADER) InsertAtNullHash.c
InsertAtNullKey_SOURCES        = $(COMMON_HEADER) InsertAtNullKey.c
InsertAtNullNewItem_SOURCES    = $(COMMON_HEADER) InsertAtNullNewItem.c
InsertAtPass_SOURCES           = $(COMMON_HEADER) InsertAtPass.c
InsertAtZeroDataLength_SOURCES = $(COMMON_HEADER) InsertAtZeroDataLength.c
InsertAtZeroKeyLength_SOURCES  = $(COMMON_HEADER) InsertAtZeroKeyLength.c
InsertNullContext_SOURCES      = $(COMMON_HEADER) InsertNullContext.c
InsertNullData_SOURCES         = $(COMMON_HEADER) InsertNullData.c
InsertNullHash_SOURCES         = $(COMMON_HEADER) InsertNullHash.c
InsertNullKey_SOURCES          = $(COMMON_HEADER) InsertNullKey.c
InsertNullNewItem_SOURCES      = $(COMMON_HEADER) InsertNullNewItem.c
InsertPass_SOURCES             = $(COMMON_HEADER) InsertPass.c
InsertZeroDataLength_SOURCES   = $(COMMON_HEADER) InsertZeroDataLength.c
InsertZeroKeyLength_SOURCES    = $(COMMON_HEADER) InsertZeroKeyLength.c
ResizeNullContext_SOURCES      = $(COMMON_HEADER) ResizeNullContext.c
ResizeNullHash_SOURCES         = $(COMMON_HEADER) ResizeNullHash.c
ResizePass_SOURCES             = $(COMMON_HEADER) ResizePass.c

EXTRA_DIST =

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS = 

if COND_USE_DBC
TESTS +=                        \
         DeleteAtNullContext    \
         DeleteAtNullHash       \
         DeleteAtNullItem       \
         DeleteAtPass           \
         DeleteNullContext      \
         DeleteNullHash         \
         DeleteNullKey          \
         DeletePass             \
         DeleteZeroLength       \
         EmptyNullContext       \
         EmptyNullHash          \
         EmptyPass              \
         FiniNullHash           \
         FiniPass               \
         GetFirstNullBucket     \
         GetFirstNullHash       \
         GetFirstNullOffset     \
         GetFirstPass           \
         GetNextNullHash        \
         GetNextNullNextBucket  \
         GetNextNullNextOffset  \
         GetNextNullPrevOffset  \
         GetNextPass            \
         GetNullBucket          \
         GetNullContext         \
         GetNullHash            \
         GetNullItem            \
         GetNullKey             \
         GetPass                \
         GetZeroKeyLength       \
         InitNullContext        \
         InitNullHash           \
         InitNullOffset         \
         InitPass               \
         InsertAtEmptyBucket    \
         InsertAtNullContext    \
         InsertAtNullData       \
         InsertAtNullHash       \
         InsertAtNullKey        \
         InsertAtNullNewItem    \
         InsertAtPass           \
         InsertAtZeroDataLength \
         InsertAtZeroKeyLength  \
         InsertNullContext      \
         InsertNullData         \
         InsertNullHash         \
         InsertNullKey          \
         InsertNullNewItem      \
         InsertPass             \
         InsertZeroDataLength   \
         InsertZeroKeyLength    \
         ResizeNullContext      \
         ResizeNullHash         \
         ResizePass             
endif

if COND_USE_EXTREME_DBC
TESTS += 
endif

TESTS += 

XFAIL_TESTS =                        \
              DeleteAtNullContext    \
              DeleteAtNullHash       \
              DeleteAtNullItem       \
              DeleteNullContext      \
              DeleteNullHash         \
              DeleteNullKey          \
              DeleteZeroLength       \
              EmptyNullContext       \
              EmptyNullHash          \
              FiniNullHash           \
              GetFirstNullBucket     \
              GetFirstNullHash       \
              GetFirstNullOffset     \
              GetNextNullHash        \
              GetNextNullNextBucket  \
              GetNextNullNextOffset  \
              GetNextNullPrevOffset  \
              GetNullContext         \
              GetNullHash            \
              GetNullItem            \
              GetNullKey             \
              GetZeroKeyLength       \
              InitNullContext        \
              InitNullHash           \
              InitNullOffset         \
              InsertAtEmptyBucket    \
              InsertAtNullContext    \
              InsertAtNullData       \
              InsertAtNullHash       \
              InsertAtNullKey        \
              InsertAtNullNewItem    \
              InsertAtZeroDataLength \
              InsertAtZeroKeyLength  \
              InsertNullContext      \
              InsertNullData         \
              InsertNullHash         \
              InsertNullKey          \
              InsertNullNewItem      \
              InsertZeroDataLength   \
              InsertZeroKeyLength    \
              ResizeNullContext      \
              ResizeNullHash         

LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

