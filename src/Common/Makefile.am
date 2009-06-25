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

CLEANFILES = *~ arch/*~

SUBDIRS = . Tests

# This flag allows you to examine the assembler output produced by the
# gcc compiler. Useful for debugging, once in a while...
# AM_CPPFLAGS += -save-temps

noinst_LTLIBRARIES   = libCommon.la

libCommon_la_SOURCES =  \
        Barrier.h       \
        Common.h        \
        DirAccess.c     \
        DirAccess.h     \
        ErrorHandler.c  \
        ErrorHandler.h  \
        MemoryFile.c    \
        MemoryFile.h    \
        MemoryFile.inl  \
        Options.c       \
        Options.h       \
        ProcessLock.c   \
        ProcessLock.h   \
        ThreadLock.c    \
        ThreadLock.h    \
        ThreadLock.inl  \
        Timer.c         \
        Timer.h         

LOCK_FILES =                      \
        arch/Barrier-i386-gcc.h   \
        arch/Barrier-ia64-gcc.h   \
        arch/Barrier-ia64-icc.h   \
        arch/Barrier-ppc-gcc.h    \
        arch/Barrier-ppc-xlc.h    \
        arch/Barrier-sparc-gcc.h  \
        arch/Barrier-win32.h      \
        arch/Barrier-x86_64-gcc.h \
        arch/Lock-i386-gcc.h      \
        arch/Lock-ppc-gcc.h       \
        arch/Lock-ppc-xlc.h       \
        arch/Lock-semaphore.h     \
        arch/Lock-sparc-gcc.h     \
        arch/Lock-win32.h         \
        arch/Lock-x86_64-gcc.h    

EXTRA_DIST =                  \
        asctime_r.c           \
        Common.vs2005.vcproj  \
        Common.vs2008.vcproj  \
        ctime_r.c             \
        localtime_r.c         \
        Makefile.win32        \
        memcmp.c              \
        memset.c              \
        private.h             \
        strnlen.c             \
        tzfile.h              \
        $(LOCK_FILES)         

libCommon_la_LIBADD = @LTLIBOBJS@

