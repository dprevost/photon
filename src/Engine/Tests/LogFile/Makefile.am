# Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~ core *.tlog

check_PROGRAMS =                 \
                 CloseNoInit     \
                 CloseNULLLog    \
                 CloseNULLInfo   \
                 ClosePass       \
                 InitNULLDir     \
                 InitNULLInfo    \
                 InitNULLLog     \
                 InitNULLSession \
                 InitPass        \
                 LogNoInit       \
                 LogNULLInfo     \
                 LogNULLLog      \
                 LogPass         \
                 LogFileTest

CloseNoInit_SOURCES     = CloseNoInit.c
CloseNULLLog_SOURCES    = CloseNULLLog.c
CloseNULLInfo_SOURCES   = CloseNULLInfo.c
ClosePass_SOURCES       = ClosePass.c
InitNULLDir_SOURCES     = InitNULLDir.c
InitNULLInfo_SOURCES    = InitNULLInfo.c
InitNULLLog_SOURCES     = InitNULLLog.c
InitNULLSession_SOURCES = InitNULLSession.c
InitPass_SOURCES        = InitPass.c
LogNoInit_SOURCES       = LogNoInit.c
LogNULLInfo_SOURCES     = LogNULLInfo.c
LogNULLLog_SOURCES      = LogNULLLog.c
LogPass_SOURCES         = LogPass.c
LogFileTest_SOURCES     = LogFileTest.c

EXTRA_DIST = 

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS1 = Macros

TESTS =                 \
        CloseNoInit     \
        CloseNULLLog    \
        CloseNULLInfo   \
        ClosePass       \
        InitNULLDir     \
        InitNULLInfo    \
        InitNULLLog     \
        InitNULLSession \
        InitPass        \
        LogNoInit       \
        LogNULLInfo     \
        LogNULLLog      \
        LogPass         \
        LogFileTest

XFAIL_TESTS =                 \
              CloseNoInit     \
              CloseNULLLog    \
              CloseNULLInfo   \
              InitNULLDir     \
              InitNULLInfo    \
              InitNULLLog     \
              InitNULLSession \
              LogNoInit       \
              LogNULLInfo     \
              LogNULLLog      

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

