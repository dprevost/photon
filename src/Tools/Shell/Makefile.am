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

CLEANFILES = *~

AM_LDFLAGS = @LIBXML2_LIBS@

bin_PROGRAMS = psosh

man_MANS = psosh.1

if COND_USE_DOCBOOK2XMAN
psosh.1: psosh.xml
	docbook2x-man --encoding=iso-8859-1 psosh.xml
endif
if COND_USE_DOCBOOK2MAN
psosh.1: psosh.xml
	db2x_docbook2man --encoding=iso-8859-1 psosh.xml
endif

EXTRA_DIST =                \
        $(man_MANS)         \
        psosh.xml           \
        Makefile.win32      \
        psosh.rc            \
        psosh.vs2005.vcproj \
        psosh.vs2008.vcproj 

psosh_SOURCES =       \
        cat.cpp       \
        cat.h         \
        constants.cpp \
        constants.h   \
        shell.cpp     \
        shell.h       \
        psosh.cpp     \
        resource.h    \
        util.cpp      \
        util.h        

psosh_LDADD = $(top_builddir)/src/C++/libphotoncpp.la

