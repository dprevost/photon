# Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~

SUBDIRS = . Tests

noinst_LTLIBRARIES   = libCommon.la

libCommon_la_SOURCES =                 \
                       Common.h        \
                       ConfigFile.c    \
                       ConfigFile.h    \
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
                       ProcessLock.inl \
                       spinlock.c \
                       ThreadLock.c    \
                       ThreadLock.h    \
                       ThreadLock.inl  \
                       Timer.c         \
                       Timer.h

EXTRA_DIST =               \
             asctime_r.c   \
             ctime_r.c     \
             localtime_r.c \
             memcmp.c      \
             memset.c      \
             nanosleep.c   \
             private.h     \
             strnlen.c     \
             tzfile.h    

libCommon_la_LIBADD = @LTLIBOBJS@

AM_CPPFLAGS = -I$(top_srcdir)/src/include \
              -I$(top_srcdir)/src/zzz/include \
              -Wall

if COND_USE_KERNEL_HEADERS
   AM_CPPFLAGS += -DCONFIG_KERNEL_HEADERS \
                  -I$(top_srcdir)/src/Locking/include \
                  -include $(top_srcdir)/src/Locking/include/mylinux/autoconf.h \
                  -D__KERNEL__
endif

if COND_USE_SMP
   AM_CPPFLAGS += -DCONFIG_SMP
endif

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

