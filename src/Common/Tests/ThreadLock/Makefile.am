# Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#bin_PROGRAMS      = zzz

CLEANFILES = *~ core

THREAD_TEST_CODE =              \
                   Barrier.c    \
                   Barrier.h    \
                   ThreadWrap.c \
                   ThreadWrap.h

check_PROGRAMS =                      \
                 AcquireInvalidSig    \
                 AcquireNullLock      \
                 AcquirePass          \
                 FiniInvalidSig       \
                 FiniNullLock         \
                 FiniPass             \
                 InitNullLock         \
                 InitPass             \
                 LockConcurrency      \
                 LockShouldFail       \
                 LockTests            \
                 ReleaseInvalidSig    \
                 ReleaseNullLock      \
                 ReleasePass          \
                 TryAcqInvalidSig     \
                 TryAcqNullLock       \
                 TryAcqPass           

AcquireInvalidSig_SOURCES    = AcquireInvalidSig.c
AcquireNullLock_SOURCES      = AcquireNullLock.c
AcquirePass_SOURCES          = AcquirePass.c
FiniInvalidSig_SOURCES       = FiniInvalidSig.c
FiniNullLock_SOURCES         = FiniNullLock.c
FiniPass_SOURCES             = FiniPass.c
InitNullLock_SOURCES         = InitNullLock.c
InitPass_SOURCES             = InitPass.c
LockConcurrency_SOURCES      = LockConcurrency.c $(THREAD_TEST_CODE)
LockShouldFail_SOURCES       = LockShouldFail.c $(THREAD_TEST_CODE)
LockTests_SOURCES            = LockTests.c
ReleaseInvalidSig_SOURCES    = ReleaseInvalidSig.c
ReleaseNullLock_SOURCES      = ReleaseNullLock.c
ReleasePass_SOURCES          = ReleasePass.c
TryAcqInvalidSig_SOURCES     = TryAcqInvalidSig.c
TryAcqNullLock_SOURCES       = TryAcqNullLock.c
TryAcqPass_SOURCES           = TryAcqPass.c

EXTRA_DIST =                       \
             LockConcurrency.sh    \
             LockConcurrencyTry.sh \
             LockShouldFail.sh

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS =

if COND_USE_DBC
TESTS +=                      \
         AcquireInvalidSig    \
         AcquireNullLock      \
         AcquirePass          \
         FiniInvalidSig       \
         FiniNullLock         \
         FiniPass             \
         InitNullLock         \
         InitPass             \
         ReleaseInvalidSig    \
         ReleaseNullLock      \
         ReleasePass          \
         TryAcqInvalidSig     \
         TryAcqNullLock       \
         TryAcqPass           
endif

if COND_USE_EXTREME_DBC
TESTS += 
endif

TESTS +=                       \
         LockConcurrency.sh    \
         LockConcurrencyTry.sh \
         LockShouldFail.sh     \
         LockTests             

XFAIL_TESTS =                   \
              LockShouldFail.sh \
              AcquireInvalidSig \
              AcquireNullLock   \
              FiniInvalidSig    \
              FiniNullLock      \
              InitNullLock      \
              ReleaseInvalidSig \
              ReleaseNullLock   \
              TryAcqInvalidSig  \
              TryAcqNullLock    \
              TryAcqZeroValue 

LDADD = $(top_builddir)/src/Common/libCommon.la

AM_CPPFLAGS = -I$(top_srcdir)/src/Common \
              -I$(top_srcdir)/src/include


if COND_USE_DBC
   AM_CPPFLAGS += -DUSE_DBC
endif
if COND_USE_EXTREME_DBC
   AM_CPPFLAGS += -DUSE_EXTREME_DBC
endif


