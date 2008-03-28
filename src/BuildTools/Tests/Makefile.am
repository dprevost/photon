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

CLEANFILES = *~ genError.c genError.h

check_PROGRAMS = testParser

testParser_SOURCES        = testParser.c testError.h
nodist_testParser_SOURCES = genError.c genError.h

testParser.o: genError.h

genError.h genError.c: testError.h 
	$(top_builddir)/src/BuildTools/errorParser -i $(srcdir)/testError.h -o genError -p myErr

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   \
        selfTest.sh    

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =             \
        selfTest.sh \
        testParser  

XFAIL_TESTS = 

AM_CPPFLAGS += -I$(top_builddir)/src/BuildTools/Tests

LDADD = $(top_builddir)/src/Common/libCommon.la

