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

CLEANFILES        = *~

AM_LDFLAGS = @LIBXML2_LIBS@

SUBDIRS = . Shell
 
bin_PROGRAMS = psoinfo

psoinfo_SOURCES =  \
        info.cpp   \
        resource.h 

EXTRA_DIST =                  \
        Makefile.win32        \
        psoInfo.rc            \
        psoInfo.vs2005.vcproj \
        psoInfo.vs2008.vcproj 

psoinfo_LDADD = $(top_builddir)/src/C++/libphotoncpp.la 

