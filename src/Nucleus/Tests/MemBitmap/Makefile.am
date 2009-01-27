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

check_PROGRAMS =             \
        FiniNullBitmap       \
        FiniPass             \
        GetLengthPass        \
        GetLengthPowerOfTwo7 \
        GetLengthPowerOfTwo9 \
        GetLengthZeroLength  \
        GetLengthZeroGranu   \
        InitNullBitmap       \
        InitNullOffset       \
        InitPass             \
        InitPowerOfTwo7      \
        InitPowerOfTwo9      \
        InitZeroGranu        \
        InitZeroLength       \
        IsFreeNullBitmap     \
        IsFreeNullOffset     \
        IsFreePass           \
        SetAllocNullBitmap   \
        SetAllocNullOffset   \
        SetAllocPass         \
        SetAllocZeroLength   \
        SetFreeNullBitmap    \
        SetFreeNullOffset    \
        SetFreePass          

FiniNullBitmap_SOURCES       = FiniNullBitmap.c
FiniPass_SOURCES             = FiniPass.c
GetLengthPass_SOURCES        = GetLengthPass.c
GetLengthPowerOfTwo7_SOURCES = GetLengthPowerOfTwo7.c
GetLengthPowerOfTwo9_SOURCES = GetLengthPowerOfTwo9.c
GetLengthZeroGranu_SOURCES   = GetLengthZeroGranu.c
GetLengthZeroLength_SOURCES  = GetLengthZeroLength.c
InitNullBitmap_SOURCES       = InitNullBitmap.c
InitNullOffset_SOURCES       = InitNullOffset.c
InitPass_SOURCES             = InitPass.c
InitPowerOfTwo7_SOURCES      = InitPowerOfTwo7.c
InitPowerOfTwo9_SOURCES      = InitPowerOfTwo9.c
InitZeroGranu_SOURCES        = InitZeroGranu.c
InitZeroLength_SOURCES       = InitZeroLength.c
IsFreeNullBitmap_SOURCES     = IsFreeNullBitmap.c
IsFreeNullOffset_SOURCES     = IsFreeNullOffset.c
IsFreePass_SOURCES           = IsFreePass.c
SetAllocNullBitmap_SOURCES   = SetAllocNullBitmap.c
SetAllocNullOffset_SOURCES   = SetAllocNullOffset.c
SetAllocPass_SOURCES         = SetAllocPass.c
SetAllocZeroLength_SOURCES   = SetAllocZeroLength.c
SetFreeNullBitmap_SOURCES    = SetFreeNullBitmap.c
SetFreeNullOffset_SOURCES    = SetFreeNullOffset.c
SetFreePass_SOURCES          = SetFreePass.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                      \
        FiniNullBitmap       \
        FiniPass             \
        GetLengthPass        \
        GetLengthPowerOfTwo7 \
        GetLengthPowerOfTwo9 \
        GetLengthZeroGranu   \
        GetLengthZeroLength  \
        InitNullBitmap       \
        InitNullOffset       \
        InitPass             \
        InitPowerOfTwo7      \
        InitPowerOfTwo9      \
        InitZeroGranu        \
        InitZeroLength       \
        IsFreeNullBitmap     \
        IsFreeNullOffset     \
        IsFreePass           \
        SetAllocNullBitmap   \
        SetAllocNullOffset   \
        SetAllocPass         \
        SetAllocZeroLength   \
        SetFreeNullBitmap    \
        SetFreeNullOffset    \
        SetFreePass          

XFAIL_TESTS =                \
        FiniNullBitmap       \
        GetLengthPowerOfTwo7 \
        GetLengthPowerOfTwo9 \
        GetLengthZeroGranu   \
        GetLengthZeroLength  \
        InitNullBitmap       \
        InitNullOffset       \
        InitPowerOfTwo7      \
        InitPowerOfTwo9      \
        InitZeroGranu        \
        InitZeroLength       \
        IsFreeNullBitmap     \
        IsFreeNullOffset     \
        SetAllocNullBitmap   \
        SetAllocNullOffset   \
        SetAllocZeroLength   \
        SetFreeNullBitmap    \
        SetFreeNullOffset    

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

