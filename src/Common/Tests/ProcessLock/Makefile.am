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

# This flag allows you to examine the assembler output produced by the
# gcc compiler. Useful for debugging, once in a while...
# AM_CPPFLAGS += -save-temps

check_PROGRAMS =             \
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

EXTRA_DIST =                  \
        LockConcurrency.sh    \
        LockConcurrencyTry.sh \
        Makefile.win32        \
        RunTests.vbs          

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                       \
        AcquireInvalidSig     \
        AcquireNullLock       \
        AcquirePass           \
        AcquireZeroValue      \
        FiniInvalidSig        \
        FiniNullLock          \
        FiniPass              \
        InitNullLock          \
        InitPass              \
        IsItLockedInvalidSig  \
        IsItLockedNullLock    \
        IsItLockedPass        \
        LockConcurrency.sh    \
        LockConcurrencyTry.sh \
        LockTests             \
        ReleaseInvalidSig     \
        ReleaseNullLock       \
        ReleasePass           \
        TestPidInvalidSig     \
        TestPidNullLock       \
        TestPidPass           \
        TestPidZeroPid        \
        TryAcqInvalidSig      \
        TryAcqNullLock        \
        TryAcqPass            \
        TryAcqZeroValue       

XFAIL_TESTS =                \
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

