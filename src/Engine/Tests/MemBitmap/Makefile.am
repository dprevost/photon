# Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~ core

check_PROGRAMS =                      \
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

EXTRA_DIST =         

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

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

XFAIL_TESTS =                      \
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

LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

AM_CPPFLAGS = -I$(top_srcdir)/src/Engine       \
              -I$(top_srcdir)/src/Engine/Tests \
              -I$(top_srcdir)/src/Common       \
              -I$(top_srcdir)/src/Tests        \
              -I$(top_srcdir)/src/include

if COND_USE_ASSERT
   AM_CPPFLAGS += -DUSE_ASSERT
endif
if COND_USE_TRAP_ERRORS
   AM_CPPFLAGS += -DUSE_TRAP_ERRORS
endif

if COND_USE_DBC
   AM_CPPFLAGS += -DUSE_DBC
endif
if COND_USE_EXTREME_DBC
   AM_CPPFLAGS += -DUSE_EXTREME_DBC
endif


