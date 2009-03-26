# Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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

AM_LDFLAGS = @LIBXML2_LIBS@

check_PROGRAMS =  \
        Close     \
        Create    \
        Get       \
        GetLength \
        Open      

Close_SOURCES     = Close.c
Create_SOURCES    = Create.c
Get_SOURCES       = Get.c
GetLength_SOURCES = GetLength.c
Open_SOURCES      = Open.c

EXTRA_DIST =           \
        Close.sh       \
        Create.sh      \
        Get.sh         \
        GetLength.sh   \
        Makefile.win32 \
        Open.sh        \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        tcp_port=10701               \
        top_srcdir=$(top_srcdir)     \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)

TESTS =              \
        Close.sh     \
        Create.sh    \
        Get.sh       \
        GetLength.sh \
        Open.sh      

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/API/libphoton.la        \
        $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la   
