# Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

AM_LDFLAGS = @LIBXML2_LIBS@

check_PROGRAMS =   \
        AddField   \
        AddKey     \
        Construct  \
        ObjectType \
        Reset      

AddKey_SOURCES     = AddKey.cpp
AddField_SOURCES   = AddField.cpp
Construct_SOURCES  = Construct.cpp
ObjectType_SOURCES = ObjectType.cpp
Reset_SOURCES      = Reset.cpp

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        tcp_port=10701               \
        top_srcdir=$(top_srcdir)     \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)

TESTS =            \
        AddField   \
        AddKey     \
        Construct  \
        ObjectType \
        Reset      

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/C++/libphotoncpp.la 

