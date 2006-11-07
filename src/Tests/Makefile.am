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

SUBDIRS =          \
          Common   \
          Engine   \
          API      \
          Watchdog

#          include       
#          java          
#          CrashRecovery 
#          Demo

noinst_PROGRAMS = \
                  Interrupted


check_PROGRAMS = \
                 bget             

#DoNothing

bget_SOURCES             = bget.cpp
#DoNothing_SOURCES        = DoNothing.cpp
Interrupted_SOURCES = Interrupted.cpp

EXTRA_DIST =                    \
             bget.sh            

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS =                    \
        TimerTest          \
        bget.sh            

LDADD = $(top_builddir)/src/API/libvdsf.la

AM_CPPFLAGS = -I$(top_srcdir)/src/include    \
              -I$(top_srcdir)/src/API        \
              -I$(top_srcdir)/src/Common     \
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

distclean-local:
	-rm -rf $(testsubdir)

