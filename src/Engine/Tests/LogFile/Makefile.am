# Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

CLEANFILES = *~ core *.tlog

check_PROGRAMS =                 \
                 CloseNoInit     \
                 CloseNULLLog    \
                 CloseNULLError  \
                 ClosePass       \
                 InitNULLDir     \
                 InitNULLError   \
                 InitNULLLog     \
                 InitNULLSession \
                 InitPass        \
                 LogNoInit       \
                 LogNULLError    \
                 LogNULLLog      \
                 LogPass         \
                 LogFileTest

CloseNoInit_SOURCES     = CloseNoInit.c
CloseNULLLog_SOURCES    = CloseNULLLog.c
CloseNULLError_SOURCES  = CloseNULLError.c
ClosePass_SOURCES       = ClosePass.c
InitNULLDir_SOURCES     = InitNULLDir.c
InitNULLError_SOURCES   = InitNULLError.c
InitNULLLog_SOURCES     = InitNULLLog.c
InitNULLSession_SOURCES = InitNULLSession.c
InitPass_SOURCES        = InitPass.c
LogNoInit_SOURCES       = LogNoInit.c
LogNULLError_SOURCES    = LogNULLError.c
LogNULLLog_SOURCES      = LogNULLLog.c
LogPass_SOURCES         = LogPass.c
LogFileTest_SOURCES     = LogFileTest.c

EXTRA_DIST = 

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS =                 \
        CloseNoInit     \
        CloseNULLLog    \
        CloseNULLError  \
        ClosePass       \
        InitNULLDir     \
        InitNULLError   \
        InitNULLLog     \
        InitNULLSession \
        InitPass        \
        LogNoInit       \
        LogNULLError    \
        LogNULLLog      \
        LogPass         \
        LogFileTest

XFAIL_TESTS =                 \
              CloseNoInit     \
              CloseNULLLog    \
              CloseNULLError  \
              InitNULLDir     \
              InitNULLError   \
              InitNULLLog     \
              InitNULLSession \
              LogNoInit       \
              LogNULLError    \
              LogNULLLog      

LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

