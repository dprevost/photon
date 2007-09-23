# Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

SUBDIRS =  Process

check_PROGRAMS =                  \
                 SimpleTest1      \
                 TransactionTests 

SimpleTest1_SOURCES = SimpleTest1.cpp
TransactionTests_SOURCES = TransactionTests.cpp 

EXTRA_DIST =                 \
             Common.py       \
             api_tests.py    \
             transactions.py \
             python_tests.sh 

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS =                 \
        python_tests.sh 

LDADD = $(top_builddir)/src/API/libvdsf.la

distclean-local:
	-rm -rf $(testsubdir)

