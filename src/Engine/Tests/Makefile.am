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

SUBDIRS =           \
          List      \
          LogFile   \
          MemAlloc  \
          MemObject

check_PROGRAMS =                  \
                 LinkNodeNULLInit \
                 LinkNodeNULLTest \
                 LinkNodeTestTest            

LinkNodeNULLInit_SOURCES = LinkNodeNULLInit.c
LinkNodeNULLTest_SOURCES = LinkNodeNULLTest.c
LinkNodeTestTest_SOURCES = LinkNodeTestTest.c

EXTRA_DIST = EngineTestCommon.h

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS1 = 

TESTS =                  \
        LinkNodeNULLInit \
        LinkNodeNULLTest \
        LinkNodeTestTest            

XFAIL_TESTS =                  \
              LinkNodeNULLInit \
              LinkNodeNULLTest            

LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

AM_CPPFLAGS = -I$(top_srcdir)/src/Engine \
              -I$(top_srcdir)/src/Common \
              -I$(top_srcdir)/src/include

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

