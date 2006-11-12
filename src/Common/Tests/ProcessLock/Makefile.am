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
                 AcquireInvalidSig    \
                 AcquireNullLock      \
                 AcquirePass          \
                 AcquireZeroValue     \
                 FiniInvalidSig       \
                 FiniNullLock         \
                 FiniPass             \
                 InitNullLock         \
                 InitPass             \
                 IsItLockedInvalidSig \
                 IsItLockedNullLock   \
                 IsItLockedPass       \
                 LockConcurrency      \
                 LockShouldFail       \
                 LockTests            \
                 ReleaseInvalidSig    \
                 ReleaseNullLock      \
                 ReleasePass          \
                 TestPidInvalidSig    \
                 TestPidNullLock      \
                 TestPidPass          \
                 TestPidZeroPid       \
                 TryAcqInvalidSig     \
                 TryAcqNullLock       \
                 TryAcqPass           \
                 TryAcqZeroValue 

AcquireInvalidSig_SOURCES    = AcquireInvalidSig.c
AcquireNullLock_SOURCES      = AcquireNullLock.c
AcquirePass_SOURCES          = AcquirePass.c
AcquireZeroValue_SOURCES     = AcquireZeroValue.c
FiniInvalidSig_SOURCES       = FiniInvalidSig.c
FiniNullLock_SOURCES         = FiniNullLock.c
FiniPass_SOURCES             = FiniPass.c
InitNullLock_SOURCES         = InitNullLock.c
InitPass_SOURCES             = InitPass.c
IsItLockedInvalidSig_SOURCES = IsItLockedInvalidSig.c
IsItLockedNullLock_SOURCES   = IsItLockedNullLock.c
IsItLockedPass_SOURCES       = IsItLockedPass.c
LockConcurrency_SOURCES      = LockConcurrency.c
LockShouldFail_SOURCES       = LockShouldFail.c
LockTests_SOURCES            = LockTests.c
ReleaseInvalidSig_SOURCES    = ReleaseInvalidSig.c
ReleaseNullLock_SOURCES      = ReleaseNullLock.c
ReleasePass_SOURCES          = ReleasePass.c
TestPidInvalidSig_SOURCES    = TestPidInvalidSig.c
TestPidNullLock_SOURCES      = TestPidNullLock.c
TestPidPass_SOURCES          = TestPidPass.c
TestPidZeroPid_SOURCES       = TestPidZeroPid.c
TryAcqInvalidSig_SOURCES     = TryAcqInvalidSig.c
TryAcqNullLock_SOURCES       = TryAcqNullLock.c
TryAcqPass_SOURCES           = TryAcqPass.c
TryAcqZeroValue_SOURCES      = TryAcqZeroValue.c


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
         AcquireZeroValue     \
         FiniInvalidSig       \
         FiniNullLock         \
         FiniPass             \
         InitNullLock         \
         InitPass             \
         IsItLockedInvalidSig \
         IsItLockedNullLock   \
         IsItLockedPass       \
         ReleaseInvalidSig    \
         ReleaseNullLock      \
         ReleasePass          \
         TestPidInvalidSig    \
         TestPidNullLock      \
         TestPidPass          \
         TestPidZeroPid       \
         TryAcqInvalidSig     \
         TryAcqNullLock       \
         TryAcqPass           \
         TryAcqZeroValue 
endif

if COND_USE_EXTREME_DBC
TESTS += 
endif

TESTS +=                       \
         LockConcurrency.sh    \
         LockConcurrencyTry.sh \
         LockShouldFail.sh     \
         LockTests             

XFAIL_TESTS =                      \
              LockShouldFail.sh    \
              AcquireInvalidSig    \
              AcquireNullLock      \
              AcquireZeroValue     \
              FiniInvalidSig       \
              FiniNullLock         \
              InitNullLock         \
              IsItLockedInvalidSig \
              IsItLockedNullLock   \
              ReleaseInvalidSig    \
              ReleaseNullLock      \
              TestPidInvalidSig    \
              TestPidNullLock      \
              TestPidZeroPid       \
              TryAcqInvalidSig     \
              TryAcqNullLock       \
              TryAcqZeroValue 

LDADD = $(top_builddir)/src/Common/libCommon.la

AM_CPPFLAGS = -I$(top_srcdir)/src/Common       \
              -I$(top_srcdir)/src/Common/Tests \
              -I$(top_srcdir)/src/include


if COND_USE_DBC
   AM_CPPFLAGS += -DUSE_DBC
endif
if COND_USE_EXTREME_DBC
   AM_CPPFLAGS += -DUSE_EXTREME_DBC
endif


