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
lib_LTLIBRARIES = libvdsf.la

SUBDIRS = . Tests
          
# See http://www.gnu.org/software/libtool/manual.html#SEC34
# for more info on version-info. 
# Hints:
#  - the version is not in synch with the version of the whole package
#  - format:  currentversion:revision:age  (age is the tricky one here)

libvdsf_la_LDFLAGS = -version-info 1:0:0

include_HEADERS = \
   Item.h

libvdsf_la_SOURCES = \
   Connector.cpp    \
   Connector.h      \
   Folder.cpp       \
   Process.cpp      \
   ProcessHook.cpp  \
   ProcessHook.h    \
   ProxyObject.cpp  \
   ProxyObject.h    \
   Queue.cpp        \
   Session.cpp      \
   SessionHook.cpp  \
   SessionHook.h    \
   WatchdogCommon.h \
   vdsfcapi.cpp

AM_CPPFLAGS = -I$(top_srcdir)/src/include \
              -I$(top_srcdir)/src/Common  \
              -I$(top_srcdir)/src/Engine

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

libvdsf_la_LIBADD = \
   $(top_builddir)/src/Engine/libEngine.la \
   $(top_builddir)/src/Common/libCommon.la
