# Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

CLEANFILES = *~ core Memfile.mem

THREAD_TEST_CODE =                 \
        ../ThreadLock/Barrier.c    \
        ../ThreadLock/Barrier.h    \
        ../ThreadLock/ThreadWrap.c \
        ../ThreadLock/ThreadWrap.h 

check_PROGRAMS =              \
        ProcessLockShouldFail \
        ThreadLockShouldFail  

ProcessLockShouldFail_SOURCES = ProcessLockShouldFail.c
ThreadLockShouldFail_SOURCES  = ThreadLockShouldFail.c $(THREAD_TEST_CODE)

EXTRA_DIST =                     \
        ProcessLockShouldFail.sh \
        ThreadLockShouldFail.sh  

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                          \
        ProcessLockShouldFail.sh \
        ThreadLockShouldFail.sh  

# ProcessLockShouldFail.sh won't
# work on a uniprocessor if the kernel was built
# without CONFIG_PREEMPT (linux kernel that is).
#         ProcessLockShouldFail.sh     

LDADD = $(top_builddir)/src/Common/libCommon.la

