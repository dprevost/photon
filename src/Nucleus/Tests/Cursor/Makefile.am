# Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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

check_PROGRAMS =               \
        EmptyNullContext       \
        EmptyNullCursor        \
        FiniNullContext        \
        FiniNullCursor         \
        GetFirstNullContext    \
        GetFirstNullCursor     \
        GetFirstNullItem       \
        GetLastNullContext     \
        GetLastNullCursor      \
        GetLastNullItem        \
        GetNextNullContext     \
        GetNextNullCursor      \
        GetNextNullItem        \
        GetNextNullOldItem     \
        GetPreviousNullContext \
        GetPreviousNullCursor  \
        GetPreviousNullItem    \
        GetPreviousNullOldItem \
        InitNullCursor         \
        InitNullContext        \
        InitNullParent         \
        InitZeroBlocks         \
        InsertFirstNullContext \
        InsertFirstNullCursor  \
        InsertFirstNullItem    \
        InsertFirstWrongType   \
        InsertLastNullContext  \
        InsertLastNullCursor   \
        InsertLastNullItem     \
        InsertLastWrongType    \
        SizeNullCursor         \
        SizeNullNumItems       \
        Tests                  

COMMON_HEADER = cursorTest.h

EmptyNullContext_SOURCES       = $(COMMON_HEADER) EmptyNullContext.c
EmptyNullCursor_SOURCES        = $(COMMON_HEADER) EmptyNullCursor.c
FiniNullContext_SOURCES        = $(COMMON_HEADER) FiniNullContext.c
FiniNullCursor_SOURCES         = $(COMMON_HEADER) FiniNullCursor.c
GetFirstNullContext_SOURCES    = $(COMMON_HEADER) GetFirstNullContext.c
GetFirstNullCursor_SOURCES     = $(COMMON_HEADER) GetFirstNullCursor.c
GetFirstNullItem_SOURCES       = $(COMMON_HEADER) GetFirstNullItem.c
GetLastNullContext_SOURCES     = $(COMMON_HEADER) GetLastNullContext.c
GetLastNullCursor_SOURCES      = $(COMMON_HEADER) GetLastNullCursor.c
GetLastNullItem_SOURCES        = $(COMMON_HEADER) GetLastNullItem.c
GetNextNullContext_SOURCES     = $(COMMON_HEADER) GetNextNullContext.c
GetNextNullCursor_SOURCES      = $(COMMON_HEADER) GetNextNullCursor.c
GetNextNullItem_SOURCES        = $(COMMON_HEADER) GetNextNullItem.c
GetNextNullOldItem_SOURCES     = $(COMMON_HEADER) GetNextNullOldItem.c
GetPreviousNullContext_SOURCES = $(COMMON_HEADER) GetPreviousNullContext.c
GetPreviousNullCursor_SOURCES  = $(COMMON_HEADER) GetPreviousNullCursor.c
GetPreviousNullItem_SOURCES    = $(COMMON_HEADER) GetPreviousNullItem.c
GetPreviousNullOldItem_SOURCES = $(COMMON_HEADER) GetPreviousNullOldItem.c
InitNullContext_SOURCES        = $(COMMON_HEADER) InitNullContext.c
InitNullCursor_SOURCES         = $(COMMON_HEADER) InitNullCursor.c
InitNullParent_SOURCES         = $(COMMON_HEADER) InitNullParent.c
InitZeroBlocks_SOURCES         = $(COMMON_HEADER) InitZeroBlocks.c
InsertFirstNullContext_SOURCES = $(COMMON_HEADER) InsertFirstNullContext.c
InsertFirstNullCursor_SOURCES  = $(COMMON_HEADER) InsertFirstNullCursor.c
InsertFirstNullItem_SOURCES    = $(COMMON_HEADER) InsertFirstNullItem.c
InsertFirstWrongType_SOURCES   = $(COMMON_HEADER) InsertFirstWrongType.c
InsertLastNullContext_SOURCES  = $(COMMON_HEADER) InsertLastNullContext.c
InsertLastNullCursor_SOURCES   = $(COMMON_HEADER) InsertLastNullCursor.c
InsertLastNullItem_SOURCES     = $(COMMON_HEADER) InsertLastNullItem.c
InsertLastWrongType_SOURCES    = $(COMMON_HEADER) InsertLastWrongType.c
SizeNullCursor_SOURCES         = $(COMMON_HEADER) SizeNullCursor.c
SizeNullNumItems_SOURCES       = $(COMMON_HEADER) SizeNullNumItems.c
Tests_SOURCES                  = $(COMMON_HEADER) Tests.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                        \
        EmptyNullContext       \
        EmptyNullCursor        \
        FiniNullContext        \
        FiniNullCursor         \
        GetFirstNullContext    \
        GetFirstNullCursor     \
        GetFirstNullItem       \
        GetLastNullContext     \
        GetLastNullCursor      \
        GetLastNullItem        \
        GetNextNullContext     \
        GetNextNullCursor      \
        GetNextNullItem        \
        GetNextNullOldItem     \
        GetPreviousNullContext \
        GetPreviousNullCursor  \
        GetPreviousNullItem    \
        GetPreviousNullOldItem \
        InitNullContext        \
        InitNullCursor         \
        InitNullParent         \
        InitZeroBlocks         \
        InsertFirstNullContext \
        InsertFirstNullCursor  \
        InsertFirstNullItem    \
        InsertFirstWrongType   \
        InsertLastNullContext  \
        InsertLastNullCursor   \
        InsertLastNullItem     \
        InsertLastWrongType    \
        SizeNullCursor         \
        SizeNullNumItems       \
        Tests                  

XFAIL_TESTS =                  \
        EmptyNullContext       \
        EmptyNullCursor        \
        FiniNullContext        \
        FiniNullCursor         \
        GetFirstNullContext    \
        GetFirstNullCursor     \
        GetFirstNullItem       \
        GetLastNullContext     \
        GetLastNullCursor      \
        GetLastNullItem        \
        GetNextNullContext     \
        GetNextNullCursor      \
        GetNextNullItem        \
        GetNextNullOldItem     \
        GetPreviousNullContext \
        GetPreviousNullCursor  \
        GetPreviousNullItem    \
        GetPreviousNullOldItem \
        InitNullContext        \
        InitNullCursor         \
        InitNullParent         \
        InitZeroBlocks         \
        InsertFirstNullContext \
        InsertFirstNullCursor  \
        InsertFirstNullItem    \
        InsertFirstWrongType   \
        InsertLastNullContext  \
        InsertLastNullCursor   \
        InsertLastNullItem     \
        InsertLastWrongType    \
        SizeNullCursor         \
        SizeNullNumItems       

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la
