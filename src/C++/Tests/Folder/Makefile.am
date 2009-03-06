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

CLEANFILES = *~ core

AM_LDFLAGS = @LIBXML2_LIBS@

check_PROGRAMS =     \
        ClosePass    \
        Create       \
        Destroy      \
        GetFirstPass \
        GetNextPass  \
        OpenPass     \
        StatusPass   

#        CreateXML    
#CreateXML_SOURCES    = CreateXML.cpp

ClosePass_SOURCES    = ClosePass.cpp
Create_SOURCES       = Create.cpp
Destroy_SOURCES      = Destroy.cpp
GetFirstPass_SOURCES = GetFirstPass.cpp
GetNextPass_SOURCES  = GetNextPass.cpp
OpenPass_SOURCES     = OpenPass.cpp
StatusPass_SOURCES   = StatusPass.cpp

EXTRA_DIST =            \
        ClosePass.sh    \
        Create.sh       \
        CreateXML.sh    \
        Destroy.sh      \
        GetFirstPass.sh \
        GetNextPass.sh  \
        Makefile.win32  \
        OpenPass.sh     \
        RunTests.vbs    \
        StatusPass.sh   

TESTS_ENVIRONMENT =                  \
        tcp_port=10701               \
        top_srcdir=$(top_srcdir)     \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)

TESTS =                 \
        ClosePass.sh    \
        Create.sh       \
        Destroy.sh      \
        GetFirstPass.sh \
        GetNextPass.sh  \
        OpenPass.sh     \
        StatusPass.sh   

#        CreateXML.sh    

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/C++/libphotoncpp.la 

