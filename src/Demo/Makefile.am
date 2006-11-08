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

noinst_PROGRAMS = InitCrashDemo \
                  CrashClient   \
                  TMDemo        \
                  TMReadLoop


InitCrashDemo_SOURCES = InitCrashDemo.cpp demo.h
CrashClient_SOURCES   = CrashClient.cpp demo.h
TMDemo_SOURCES        = TMDemo.cpp
TMReadLoop_SOURCES    = TMReadLoop.cpp

EXTRA_DIST               =                  \
                         CrashClient.sh     \
                         StartCrashDemo.sh  \
                         StartTMDemo.sh     \
                         TMDemo.sh          \
                         TMReadLoop.sh 


LDADD = $(top_builddir)/src/API/libvdsf.la

AM_CPPFLAGS = -I$(top_srcdir)/src/include    \
              -I$(top_srcdir)/src/API        \
              -I$(top_srcdir)/src/Common     \
              -I$(top_srcdir)/src/CoreEngine

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

#distclean-local:
#	-rm -rf $(testsubdir)

