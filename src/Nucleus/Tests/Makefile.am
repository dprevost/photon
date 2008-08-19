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

CLEANFILES = *~ core

# There is an order in the tests - for example the Hash tests are done
# before the Folder tests or the HashMaps tests since the last two use
# Hash but the Hash tests are after the memory allocation tests since 
# Hash uses it..
SUBDIRS =              \
        List           \
        LogFile        \
        MemAlloc       \
        MemBitmap      \
        BlockGroup     \
        MemObject      \
        Hash           \
        Transaction    \
        ProcessManager \
        Process        \
        Session        \
        Folder         \
        HashMap        \
        Map            \
        Queue          \
        Optional       

check_PROGRAMS =         \
        LinkNodeNULLInit \
        LinkNodeNULLTest \
        LinkNodeTestTest 

LinkNodeNULLInit_SOURCES = LinkNodeNULLInit.c
LinkNodeNULLTest_SOURCES = LinkNodeNULLTest.c
LinkNodeTestTest_SOURCES = LinkNodeTestTest.c

EXTRA_DIST =               \
        EngineTestCommon.h \
        Makefile.win32     \
        Makefile.inc.win32 \
        RunTests.vbs       

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                  \
        LinkNodeNULLInit \
        LinkNodeNULLTest \
        LinkNodeTestTest 

XFAIL_TESTS =            \
        LinkNodeNULLInit \
        LinkNodeNULLTest 

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

