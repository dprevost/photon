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

# The two "real" regression tests are ListTestSmall and ListTestBig.
# I put them at the end to make sure that the interface itself (to 
# the list) is ok. I also put Small before Big since the smaller one
# is doing a lot less iterations but doing many more consistency checks
# (I could have bundled it all together but the test would take too
# much clock time/cpu time). 

AM_CFLAGS = -Wall

check_PROGRAMS =              \
        DeleteNullContext     \
        DeleteNullHash        \
        DeleteNullItem        \
        DeletePass            \
        FiniNullHash          \
        FiniPass              \
        GetFirstNullHash      \
        GetFirstNullOffset    \
        GetFirstPass          \
        GetNextNullHash       \
        GetNextNullNextOffset \
        GetNextNullPrevOffset \
        GetNextPass           \
        GetNullBucket         \
        GetNullContext        \
        GetNullHash           \
        GetNullItem           \
        GetNullKey            \
        GetPass               \
        GetZeroKeyLength      \
        InitNullContext       \
        InitNullHash          \
        InitNullOffset        \
        InitPass              \
        InsertNullContext     \
        InsertNullData        \
        InsertNullHash        \
        InsertNullKey         \
        InsertNullNewItem     \
        InsertPass            \
        InsertZeroDataLength  \
        InsertZeroKeyLength   \
        ResizeNullContext     \
        ResizeNullHash        \
        ResizePass            

COMMON_HEADER = HashTest.h

DeleteNullContext_SOURCES     = $(COMMON_HEADER) DeleteNullContext.c
DeleteNullHash_SOURCES        = $(COMMON_HEADER) DeleteNullHash.c
DeleteNullItem_SOURCES        = $(COMMON_HEADER) DeleteNullItem.c
DeletePass_SOURCES            = $(COMMON_HEADER) DeletePass.c
FiniNullHash_SOURCES          = $(COMMON_HEADER) FiniNullHash.c
FiniPass_SOURCES              = $(COMMON_HEADER) FiniPass.c
GetFirstNullHash_SOURCES      = $(COMMON_HEADER) GetFirstNullHash.c
GetFirstNullOffset_SOURCES    = $(COMMON_HEADER) GetFirstNullOffset.c
GetFirstPass_SOURCES          = $(COMMON_HEADER) GetFirstPass.c
GetNextNullHash_SOURCES       = $(COMMON_HEADER) GetNextNullHash.c
GetNextNullNextOffset_SOURCES = $(COMMON_HEADER) GetNextNullNextOffset.c
GetNextNullPrevOffset_SOURCES = $(COMMON_HEADER) GetNextNullPrevOffset.c
GetNextPass_SOURCES           = $(COMMON_HEADER) GetNextPass.c
GetNullBucket_SOURCES         = $(COMMON_HEADER) GetNullBucket.c
GetNullContext_SOURCES        = $(COMMON_HEADER) GetNullContext.c
GetNullHash_SOURCES           = $(COMMON_HEADER) GetNullHash.c
GetNullItem_SOURCES           = $(COMMON_HEADER) GetNullItem.c
GetNullKey_SOURCES            = $(COMMON_HEADER) GetNullKey.c
GetPass_SOURCES               = $(COMMON_HEADER) GetPass.c
GetZeroKeyLength_SOURCES      = $(COMMON_HEADER) GetZeroKeyLength.c
InitNullContext_SOURCES       = $(COMMON_HEADER) InitNullContext.c
InitNullHash_SOURCES          = $(COMMON_HEADER) InitNullHash.c
InitNullOffset_SOURCES        = $(COMMON_HEADER) InitNullOffset.c
InitPass_SOURCES              = $(COMMON_HEADER) InitPass.c
InsertNullContext_SOURCES     = $(COMMON_HEADER) InsertNullContext.c
InsertNullData_SOURCES        = $(COMMON_HEADER) InsertNullData.c
InsertNullHash_SOURCES        = $(COMMON_HEADER) InsertNullHash.c
InsertNullKey_SOURCES         = $(COMMON_HEADER) InsertNullKey.c
InsertNullNewItem_SOURCES     = $(COMMON_HEADER) InsertNullNewItem.c
InsertPass_SOURCES            = $(COMMON_HEADER) InsertPass.c
InsertZeroDataLength_SOURCES  = $(COMMON_HEADER) InsertZeroDataLength.c
InsertZeroKeyLength_SOURCES   = $(COMMON_HEADER) InsertZeroKeyLength.c
ResizeNullContext_SOURCES     = $(COMMON_HEADER) ResizeNullContext.c
ResizeNullHash_SOURCES        = $(COMMON_HEADER) ResizeNullHash.c
ResizePass_SOURCES            = $(COMMON_HEADER) ResizePass.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                       \
        DeleteNullContext     \
        DeleteNullHash        \
        DeleteNullItem        \
        DeletePass            \
        FiniNullHash          \
        FiniPass              \
        GetFirstNullHash      \
        GetFirstNullOffset    \
        GetFirstPass          \
        GetNextNullHash       \
        GetNextNullNextOffset \
        GetNextNullPrevOffset \
        GetNextPass           \
        GetNullBucket         \
        GetNullContext        \
        GetNullHash           \
        GetNullItem           \
        GetNullKey            \
        GetPass               \
        GetZeroKeyLength      \
        InitNullContext       \
        InitNullHash          \
        InitNullOffset        \
        InitPass              \
        InsertNullContext     \
        InsertNullData        \
        InsertNullHash        \
        InsertNullKey         \
        InsertNullNewItem     \
        InsertPass            \
        InsertZeroDataLength  \
        InsertZeroKeyLength   \
        ResizeNullContext     \
        ResizeNullHash        \
        ResizePass            

XFAIL_TESTS =                 \
        DeleteNullContext     \
        DeleteNullHash        \
        DeleteNullItem        \
        FiniNullHash          \
        GetFirstNullHash      \
        GetFirstNullOffset    \
        GetNextNullHash       \
        GetNextNullNextOffset \
        GetNextNullPrevOffset \
        GetNullBucket         \
        GetNullContext        \
        GetNullHash           \
        GetNullItem           \
        GetNullKey            \
        GetZeroKeyLength      \
        InitNullContext       \
        InitNullHash          \
        InitNullOffset        \
        InsertNullContext     \
        InsertNullData        \
        InsertNullHash        \
        InsertNullKey         \
        InsertNullNewItem     \
        InsertZeroDataLength  \
        InsertZeroKeyLength   \
        ResizeNullContext     \
        ResizeNullHash        

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

