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

check_PROGRAMS =                  \
                 CloseNullAlloc   \
                 CloseNullError   \
                 ClosePass        \
                 FreeNullAlloc    \
                 FreeNullError    \
                 FreeNullPtr      \
                 FreePass         \
                 FreeZeroLength   \
                 InitBigVDS       \
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

COMMON_ENGINE = EngineTestCommon.h

CloseNullAlloc_SOURCES     = $(COMMON_ENGINE) CloseNullAlloc.c
CloseNullError_SOURCES     = $(COMMON_ENGINE) CloseNullError.c
ClosePass_SOURCES          = $(COMMON_ENGINE) ClosePass.c
FreeNullAlloc_SOURCES      = $(COMMON_ENGINE) FreeNullAlloc.c
FreeNullError_SOURCES      = $(COMMON_ENGINE) FreeNullError.c
FreeNullPtr_SOURCES        = $(COMMON_ENGINE) FreeNullPtr.c
FreePass_SOURCES           = $(COMMON_ENGINE) FreePass.c
FreeZeroLength_SOURCES     = $(COMMON_ENGINE) FreeZeroLength.c
InitBigVDS_SOURCES         = $(COMMON_ENGINE) InitBigVDS.c
InitNullAddress_SOURCES    = $(COMMON_ENGINE) InitNullAddress.c
InitNullAlloc_SOURCES      = $(COMMON_ENGINE) InitNullAlloc.c
InitNullError_SOURCES      = $(COMMON_ENGINE) InitNullError.c
InitPass_SOURCES           = $(COMMON_ENGINE) InitPass.c
InitSmallLength_SOURCES    = $(COMMON_ENGINE) InitSmallLength.c
IsPageFreeTest_SOURCES     = $(COMMON_ENGINE) IsPageFreeTest.c
MallocNullAlloc_SOURCES    = $(COMMON_ENGINE) MallocNullAlloc.c
MallocNullError_SOURCES    = $(COMMON_ENGINE) MallocNullError.c
MallocPass_SOURCES         = $(COMMON_ENGINE) MallocPass.c
MallocZeroLength_SOURCES   = $(COMMON_ENGINE) MallocZeroLength.c

EXTRA_DIST =         

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS =                  \
        CloseNullAlloc   \
        CloseNullError   \
        ClosePass        \
        FreeNullAlloc    \
        FreeNullError    \
        FreeNullPtr      \
        FreePass         \
        FreeZeroLength   \
        InitBigVDS       \
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

XFAIL_TESTS =                  \
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

LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

AM_CPPFLAGS = -I$(top_srcdir)/src/Engine       \
              -I$(top_srcdir)/src/Engine/Tests \
              -I$(top_srcdir)/src/Common       \
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


