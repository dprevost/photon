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

check_PROGRAMS =                   \
                 DisplayBigVDSSize \
                 InitBigVDS        

COMMON_ENGINE = 

DisplayBigVDSSize_SOURCES = DisplayBigVDSSize.c
InitBigVDS_SOURCES = InitBigVDS.c

EXTRA_DIST =         

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS = 

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

