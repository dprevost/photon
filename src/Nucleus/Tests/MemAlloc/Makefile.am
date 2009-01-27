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

check_PROGRAMS =         \
        CloseNullAlloc   \
        CloseNullError   \
        ClosePass        \
        FreeNullAlloc    \
        FreeNullError    \
        FreeNullPtr      \
        FreePass         \
        FreeZeroLength   \
        InitNullAddress  \
        InitNullAlloc    \
        InitNullError    \
        InitPass         \
        InitSmallLength  \
        IsPageFreeTest   \
        MallocNullAlloc  \
        MallocNullError  \
        MallocPass       \
        MallocZeroLength

CloseNullAlloc_SOURCES   = CloseNullAlloc.c
CloseNullError_SOURCES   = CloseNullError.c
ClosePass_SOURCES        = ClosePass.c
FreeNullAlloc_SOURCES    = FreeNullAlloc.c
FreeNullError_SOURCES    = FreeNullError.c
FreeNullPtr_SOURCES      = FreeNullPtr.c
FreePass_SOURCES         = FreePass.c
FreeZeroLength_SOURCES   = FreeZeroLength.c
InitNullAddress_SOURCES  = InitNullAddress.c
InitNullAlloc_SOURCES    = InitNullAlloc.c
InitNullError_SOURCES    = InitNullError.c
InitPass_SOURCES         = InitPass.c
InitSmallLength_SOURCES  = InitSmallLength.c
IsPageFreeTest_SOURCES   = IsPageFreeTest.c
MallocNullAlloc_SOURCES  = MallocNullAlloc.c
MallocNullError_SOURCES  = MallocNullError.c
MallocPass_SOURCES       = MallocPass.c
MallocZeroLength_SOURCES = MallocZeroLength.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                  \
        CloseNullAlloc   \
        CloseNullError   \
        ClosePass        \
        FreeNullAlloc    \
        FreeNullError    \
        FreeNullPtr      \
        FreePass         \
        FreeZeroLength   \
        InitNullAddress  \
        InitNullAlloc    \
        InitNullError    \
        InitPass         \
        InitSmallLength  \
        IsPageFreeTest   \
        MallocNullAlloc  \
        MallocNullError  \
        MallocPass       \
        MallocZeroLength

XFAIL_TESTS =            \
        CloseNullAlloc   \
        CloseNullError   \
        FreeNullAlloc    \
        FreeNullError    \
        FreeNullPtr      \
        FreeZeroLength   \
        InitNullAddress  \
        InitNullAlloc    \
        InitNullError    \
        InitSmallLength  \
        MallocNullAlloc  \
        MallocNullError  \
        MallocZeroLength

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

