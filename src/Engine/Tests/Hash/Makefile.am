# Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~ core

# The two "real" regression tests are ListTestSmall and ListTestBig.
# I put them at the end to make sure that the interface itself (to 
# the list) is ok. I also put Small before Big since the smaller one
# is doing a lot less iterations but doing many more consistency checks
# (I could have bundled it all together but the test would take too
# much clock time/cpu time). 

AM_CFLAGS = -Wall

check_PROGRAMS =                       \
                 DeleteNullContext     \
                 DeleteNullHash        \
                 DeleteNullKey         \
                 DeletePass            \
                 DeleteZeroLength      \
                 EmptyNullContext      \
                 EmptyNullHash         \
                 EmptyPass             \
                 FiniNullContext       \
                 FiniNullHash          \
                 FiniPass              \
                 GetFirstNullBucket    \
                 GetFirstNullHash      \
                 GetFirstNullOffset    \
                 GetFirstPass          \
                 GetNextNullHash       \
                 GetNextNullNextBucket \
                 GetNextNullNextOffset \
                 GetNextPass           \
                 GetNullBucket         \
                 GetNullContext        \
                 GetNullData           \
                 GetNullDataLength     \
                 GetNullHash           \
                 GetNullKey            \
                 GetPass               \
                 GetZeroKeyLength      \
                 InitNullContext       \
                 InitNullHash          \
                 InitPass              \
                 InsertNullContext     \
                 InsertNullData        \
                 InsertNullHash        \
                 InsertNullKey         \
                 InsertNullOffset      \
                 InsertPass            \
                 InsertZeroDataLength  \
                 InsertZeroKeyLength   \
                 ResizeNullContext     \
                 ResizeNullHash        \
                 ResizePass           

DeleteNullContext_SOURCES     = DeleteNullContext.c
DeleteNullHash_SOURCES        = DeleteNullHash.c
DeleteNullKey_SOURCES         = DeleteNullKey.c
DeletePass_SOURCES            = DeletePass.c
DeleteZeroLength_SOURCES      = DeleteZeroLength.c
EmptyNullContext_SOURCES      = EmptyNullContext.c
EmptyNullHash_SOURCES         = EmptyNullHash.c
EmptyPass_SOURCES             = EmptyPass.c
FiniNullContext_SOURCES       = FiniNullContext.c
FiniNullHash_SOURCES          = FiniNullHash.c
FiniPass_SOURCES              = FiniPass.c
GetFirstNullBucket_SOURCES    = GetFirstNullBucket.c
GetFirstNullHash_SOURCES      = GetFirstNullHash.c
GetFirstNullOffset_SOURCES    = GetFirstNullOffset.c
GetFirstPass_SOURCES          = GetFirstPass.c
GetNextNullHash_SOURCES       = GetNextNullHash.c
GetNextNullNextBucket_SOURCES = GetNextNullNextBucket.c
GetNextNullNextOffset_SOURCES = GetNextNullNextOffset.c
GetNextPass_SOURCES           = GetNextPass.c
GetNullBucket_SOURCES         = GetNullBucket.c
GetNullContext_SOURCES        = GetNullContext.c
GetNullData_SOURCES           = GetNullData.c
GetNullDataLength_SOURCES     = GetNullDataLength.c
GetNullHash_SOURCES           = GetNullHash.c
GetNullKey_SOURCES            = GetNullKey.c
GetPass_SOURCES               = GetPass.c
GetZeroKeyLength_SOURCES      = GetZeroKeyLength.c
InitNullContext_SOURCES       = InitNullContext.c
InitNullHash_SOURCES          = InitNullHash.c
InitPass_SOURCES              = InitPass.c
InsertNullContext_SOURCES     = InsertNullContext.c
InsertNullData_SOURCES        = InsertNullData.c
InsertNullHash_SOURCES        = InsertNullHash.c
InsertNullKey_SOURCES         = InsertNullKey.c
InsertNullOffset_SOURCES      = InsertNullOffset.c
InsertPass_SOURCES            = InsertPass.c
InsertZeroDataLength_SOURCES  = InsertZeroDataLength.c
InsertZeroKeyLength_SOURCES   = InsertZeroKeyLength.c
ResizeNullContext_SOURCES     = ResizeNullContext.c
ResizeNullHash_SOURCES        = ResizeNullHash.c
ResizePass_SOURCES            = ResizePass.c

EXTRA_DIST =

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS = 

if COND_USE_DBC
TESTS +=                       \
         DeleteNullContext     \
         DeleteNullHash        \
         DeleteNullKey         \
         DeletePass            \
         DeleteZeroLength      \
         EmptyNullContext      \
         EmptyNullHash         \
         EmptyPass             \
         FiniNullContext       \
         FiniNullHash          \
         FiniPass              \
         GetFirstNullBucket    \
         GetFirstNullHash      \
         GetFirstNullOffset    \
         GetFirstPass          \
         GetNextNullHash       \
         GetNextNullNextBucket \
         GetNextNullNextOffset \
         GetNextPass           \
         GetNullBucket         \
         GetNullContext        \
         GetNullData           \
         GetNullDataLength     \
         GetNullHash           \
         GetNullKey            \
         GetPass               \
         GetZeroKeyLength      \
         InitNullContext       \
         InitNullHash          \
         InitPass              \
         InsertNullContext     \
         InsertNullData        \
         InsertNullHash        \
         InsertNullKey         \
         InsertNullOffset      \
         InsertPass            \
         InsertZeroDataLength  \
         InsertZeroKeyLength   \
         ResizeNullContext     \
         ResizeNullHash        \
         ResizePass            
endif

if COND_USE_EXTREME_DBC
TESTS += 
endif

TESTS += 

XFAIL_TESTS =                       \
              DeleteNullContext     \
              DeleteNullHash        \
              DeleteNullKey         \
              DeleteZeroLength      \
              EmptyNullContext      \
              EmptyNullHash         \
              FiniNullContext       \
              FiniNullHash          \
              GetFirstNullBucket    \
              GetFirstNullHash      \
              GetFirstNullOffset    \
              GetNextNullHash       \
              GetNextNullNextBucket \
              GetNextNullNextOffset \
              GetNullContext        \
              GetNullData           \
              GetNullDataLength     \
              GetNullHash           \
              GetNullKey            \
              GetZeroKeyLength      \
              InitNullContext       \
              InitNullHash          \
              InsertNullContext     \
              InsertNullData        \
              InsertNullHash        \
              InsertNullKey         \
              InsertNullOffset      \
              InsertZeroDataLength  \
              InsertZeroKeyLength   \
              ResizeNullContext     \
              ResizeNullHash        

LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

AM_CPPFLAGS = -I$(top_srcdir)/src/Engine       \
              -I$(top_srcdir)/src/Engine/Tests \
              -I$(top_srcdir)/src/Common       \
              -I$(top_srcdir)/src/Tests        \
              -I$(top_srcdir)/src/include

if COND_USE_DBC
   AM_CPPFLAGS += -DUSE_DBC
endif
if COND_USE_EXTREME_DBC
   AM_CPPFLAGS += -DUSE_EXTREME_DBC
endif

