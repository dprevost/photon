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
              -Wall

include $(top_srcdir)/src/Makefile.Common.am


