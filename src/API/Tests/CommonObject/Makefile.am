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

CLEANFILES = *~ core

check_PROGRAMS =        \
        CloseNullObject \
        ClosePass       \
        OpenNullName    \
        OpenNullObject  \
        OpenPass        \
        OpenWrongType   \
        OpenZeroLength  

CloseNullObject_SOURCES = CloseNullObject.c
ClosePass_SOURCES       = ClosePass.c
OpenNullName_SOURCES    = OpenNullName.c
OpenNullObject_SOURCES  = OpenNullObject.c
OpenPass_SOURCES        = OpenPass.c
OpenWrongType_SOURCES   = OpenWrongType.c
OpenZeroLength_SOURCES  = OpenZeroLength.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.sh    \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        tcp_port=10701               \
        top_srcdir=$(top_srcdir)     \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=0                    \
        PYTHON=$(PYTHON)

TESTS = RunTests.sh 

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/API/libphoton.la      \
        $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la   

