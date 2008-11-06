# Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

check_PROGRAMS =               \
        DeleteAtNullContext    \
        DeleteAtNullHash       \
        DeleteAtNullItem       \
        DeleteAtPass           \
        FiniNullHash           \
        FiniPass               \
        GetFirstNullHash       \
        GetFirstNullOffset     \
        GetFirstPass           \
        GetNextNullHash        \
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
        InsertAtNullContext    \
        InsertAtNullData       \
        InsertAtNullHash       \
        InsertAtNullKey        \
        InsertAtNullNewItem    \
        InsertAtPass           \
        InsertAtZeroDataLength \
        InsertAtZeroKeyLength  \
        ResizeNullContext      \
        ResizeNullHash         \
        ResizePass             

COMMON_HEADER = HashTest.h

DeleteAtNullContext_SOURCES    = $(COMMON_HEADER) DeleteAtNullContext.c
DeleteAtNullHash_SOURCES       = $(COMMON_HEADER) DeleteAtNullHash.c
DeleteAtNullItem_SOURCES       = $(COMMON_HEADER) DeleteAtNullItem.c
DeleteAtPass_SOURCES           = $(COMMON_HEADER) DeleteAtPass.c
FiniNullHash_SOURCES           = $(COMMON_HEADER) FiniNullHash.c
FiniPass_SOURCES               = $(COMMON_HEADER) FiniPass.c
GetFirstNullHash_SOURCES       = $(COMMON_HEADER) GetFirstNullHash.c
GetFirstNullOffset_SOURCES     = $(COMMON_HEADER) GetFirstNullOffset.c
GetFirstPass_SOURCES           = $(COMMON_HEADER) GetFirstPass.c
GetNextNullHash_SOURCES        = $(COMMON_HEADER) GetNextNullHash.c
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
InsertAtNullContext_SOURCES    = $(COMMON_HEADER) InsertAtNullContext.c
InsertAtNullData_SOURCES       = $(COMMON_HEADER) InsertAtNullData.c
InsertAtNullHash_SOURCES       = $(COMMON_HEADER) InsertAtNullHash.c
InsertAtNullKey_SOURCES        = $(COMMON_HEADER) InsertAtNullKey.c
InsertAtNullNewItem_SOURCES    = $(COMMON_HEADER) InsertAtNullNewItem.c
InsertAtPass_SOURCES           = $(COMMON_HEADER) InsertAtPass.c
InsertAtZeroDataLength_SOURCES = $(COMMON_HEADER) InsertAtZeroDataLength.c
InsertAtZeroKeyLength_SOURCES  = $(COMMON_HEADER) InsertAtZeroKeyLength.c
ResizeNullContext_SOURCES      = $(COMMON_HEADER) ResizeNullContext.c
ResizeNullHash_SOURCES         = $(COMMON_HEADER) ResizeNullHash.c
ResizePass_SOURCES             = $(COMMON_HEADER) ResizePass.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                        \
        DeleteAtNullContext    \
        DeleteAtNullHash       \
        DeleteAtNullItem       \
        DeleteAtPass           \
        FiniNullHash           \
        FiniPass               \
        GetFirstNullHash       \
        GetFirstNullOffset     \
        GetFirstPass           \
        GetNextNullHash        \
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
        InsertAtNullContext    \
        InsertAtNullData       \
        InsertAtNullHash       \
        InsertAtNullKey        \
        InsertAtNullNewItem    \
        InsertAtPass           \
        InsertAtZeroDataLength \
        InsertAtZeroKeyLength  \
        ResizeNullContext      \
        ResizeNullHash         \
        ResizePass             

XFAIL_TESTS =                  \
        DeleteAtNullContext    \
        DeleteAtNullHash       \
        DeleteAtNullItem       \
        FiniNullHash           \
        GetFirstNullHash       \
        GetFirstNullOffset     \
        GetNextNullHash        \
        GetNextNullNextOffset  \
        GetNextNullPrevOffset  \
        GetNullBucket          \
        GetNullContext         \
        GetNullHash            \
        GetNullItem            \
        GetNullKey             \
        GetZeroKeyLength       \
        InitNullContext        \
        InitNullHash           \
        InitNullOffset         \
        InsertAtNullContext    \
        InsertAtNullData       \
        InsertAtNullHash       \
        InsertAtNullKey        \
        InsertAtNullNewItem    \
        InsertAtZeroDataLength \
        InsertAtZeroKeyLength  \
        ResizeNullContext      \
        ResizeNullHash         

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

