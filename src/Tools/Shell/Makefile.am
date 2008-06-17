# Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

bin_PROGRAMS = vdssh

man_MANS = vdssh.1
info_TEXINFOS = vdssh.texi

if COND_USE_DOCBOOK2TEXI
vdssh.texi: vdssh.xml
	docbook2x-texi --encoding=iso-8859-1 vdssh.xml
endif
if COND_USE_DOCBOOK2MAN
vdssh.1: vdssh.xml
	docbook2x-man --encoding=iso-8859-1 vdssh.xml
endif

EXTRA_DIST =           \
        $(man_MANS)    \
        vdssh.xml      \
        Makefile.win32 

vdssh_SOURCES =       \
        constants.cpp \
        constants.h   \
        shell.cpp     \
        shell.h       \
        vdssh.cpp     \
        vdssh.dsp     

vdssh_LDADD = $(top_builddir)/src/C++/libvdsfcpp.la

