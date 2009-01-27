# Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
        FiniNullGroup   \
        FiniPass        \
        InitInvalidType \
        InitNullGroup   \
        InitNullOffset  \
        InitPass        \
        InitZeroPages   

FiniNullGroup_SOURCES   = FiniNullGroup.c
FiniPass_SOURCES        = FiniPass.c
InitInvalidType_SOURCES = InitInvalidType.c
InitNullGroup_SOURCES   = InitNullGroup.c
InitNullOffset_SOURCES  = InitNullOffset.c
InitPass_SOURCES        = InitPass.c
InitZeroPages_SOURCES   = InitZeroPages.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                 \
        FiniNullGroup   \
        FiniPass        \
        InitInvalidType \
        InitNullGroup   \
        InitNullOffset  \
        InitPass        \
        InitZeroPages   

XFAIL_TESTS =           \
        FiniNullGroup   \
        InitInvalidType \
        InitNullGroup   \
        InitNullOffset  \
        InitZeroPages   

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la   

