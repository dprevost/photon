# Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~ genError.c genError.h

#
# 
#
check_PROGRAMS = testParser

testParser_SOURCES = testParser.c testError.h
nodist_testParser_SOURCES = genError.c genError.h

genError.c genError.h: testError.h
	../errorParser -i testError.h -o genError -p myErr

EXTRA_DIST =             \
             selfTest.sh 

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS =

if COND_USE_DBC
TESTS += 
#         TryAcqZeroValue 
endif

if COND_USE_EXTREME_DBC
TESTS += 
endif

TESTS +=             \
         selfTest.sh \
         testParser

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/Common/libCommon.la

AM_CPPFLAGS = -I$(top_srcdir)/src/Common \
              -I$(top_srcdir)/src/include


if COND_USE_DBC
   AM_CPPFLAGS += -DUSE_DBC
endif
if COND_USE_EXTREME_DBC
   AM_CPPFLAGS += -DUSE_EXTREME_DBC
endif

