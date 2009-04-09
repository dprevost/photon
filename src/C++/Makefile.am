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
lib_LTLIBRARIES = libphotoncpp.la

SUBDIRS = . Tests
          
# See http://www.gnu.org/software/libtool/manual.html#SEC34
# for more info on version-info. 
# Hints:
#  - the version is not in synch with the version of the whole package
#  - format:  currentversion:revision:age  (age is the tricky one here)

libphotoncpp_la_LDFLAGS = -version-info 0:0:0

include_HEADERS = 

libphotoncpp_la_SOURCES =      \
        DataDefBuilderUser.cpp \
        DataDefBuilderODBC.cpp \
        DataDefinition.cpp     \
        Exception.cpp          \
        FastMap.cpp            \
        FastMapEditor.cpp      \
        Folder.cpp             \
        HashMap.cpp            \
        KeyDefBuilderODBC.cpp  \
        KeyDefBuilderUser.cpp  \
        KeyDefinition.cpp      \
        Lifo.cpp               \
        Process.cpp            \
        Queue.cpp              \
        Session.cpp            

EXTRA_DIST =              \
        CPP.vs2005.vcproj \
        CPP.vs2008.vcproj \
        Makefile.win32    

libphotoncpp_la_LIBADD =                          \
        $(top_builddir)/src/API/libphoton.la      \
        $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la   

