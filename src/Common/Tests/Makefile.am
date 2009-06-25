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

SUBDIRS =           \
        DirAccess   \
        Error       \
        MemFile     \
        Options     \
        ProcessLock \
        ThreadLock  

check_PROGRAMS = TimerTest     

TimerTest_SOURCES = TimerTest.c

EXTRA_DIST =                              \
        Makefile.inc.win32                \
        Makefile.win32                    \
        RunTests.vbs                      \
        Optional/Makefile.am              \
        Optional/Makefile.in              \
        Optional/Makefile.win32           \
        Optional/ProcessLockShouldFail.c  \
        Optional/ProcessLockShouldFail.sh \
        Optional/RunTests.vbs             \
        Optional/ThreadLockShouldFail.c   \
        Optional/ThreadLockShouldFail.sh  

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS = TimerTest

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/Common/libCommon.la

distclean-local:
	-rm -f $(top_builddir)/src/Common/Tests/Optional/Makefile
	-rm -f $(top_builddir)/src/Common/Tests/Optional/.deps/*
	-rmdir $(top_builddir)/src/Common/Tests/Optional/.deps

