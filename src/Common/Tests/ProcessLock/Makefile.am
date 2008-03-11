# Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

INCLUDE_FILES = 

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

AcquireInvalidSig_SOURCES    = $(INCLUDE_FILES) AcquireInvalidSig.c
AcquireNullLock_SOURCES      = $(INCLUDE_FILES) AcquireNullLock.c
AcquirePass_SOURCES          = $(INCLUDE_FILES) AcquirePass.c
AcquireZeroValue_SOURCES     = $(INCLUDE_FILES) AcquireZeroValue.c
FiniInvalidSig_SOURCES       = $(INCLUDE_FILES) FiniInvalidSig.c
FiniNullLock_SOURCES         = $(INCLUDE_FILES) FiniNullLock.c
FiniPass_SOURCES             = $(INCLUDE_FILES) FiniPass.c
InitNullLock_SOURCES         = $(INCLUDE_FILES) InitNullLock.c
InitPass_SOURCES             = $(INCLUDE_FILES) InitPass.c
IsItLockedInvalidSig_SOURCES = $(INCLUDE_FILES) IsItLockedInvalidSig.c
IsItLockedNullLock_SOURCES   = $(INCLUDE_FILES) IsItLockedNullLock.c
IsItLockedPass_SOURCES       = $(INCLUDE_FILES) IsItLockedPass.c
LockConcurrency_SOURCES      = $(INCLUDE_FILES) LockConcurrency.c
LockTests_SOURCES            = $(INCLUDE_FILES) LockTests.c
ReleaseInvalidSig_SOURCES    = $(INCLUDE_FILES) ReleaseInvalidSig.c
ReleaseNullLock_SOURCES      = $(INCLUDE_FILES) ReleaseNullLock.c
ReleasePass_SOURCES          = $(INCLUDE_FILES) ReleasePass.c
TestPidInvalidSig_SOURCES    = $(INCLUDE_FILES) TestPidInvalidSig.c
TestPidNullLock_SOURCES      = $(INCLUDE_FILES) TestPidNullLock.c
TestPidPass_SOURCES          = $(INCLUDE_FILES) TestPidPass.c
TestPidZeroPid_SOURCES       = $(INCLUDE_FILES) TestPidZeroPid.c
TryAcqInvalidSig_SOURCES     = $(INCLUDE_FILES) TryAcqInvalidSig.c
TryAcqNullLock_SOURCES       = $(INCLUDE_FILES) TryAcqNullLock.c
TryAcqPass_SOURCES           = $(INCLUDE_FILES) TryAcqPass.c
TryAcqZeroValue_SOURCES      = $(INCLUDE_FILES) TryAcqZeroValue.c


EXTRA_DIST =             \
   LockConcurrency.sh    \
   LockConcurrencyTry.sh \
   Makefile.win32        \
   RunTests.vbs          

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
         LockTests             

XFAIL_TESTS =                      \
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

