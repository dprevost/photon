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

check_PROGRAMS =     \
        Close        \
        Constructor  \
        CreateFolder \
        CreateObject \
        Destroy      \
        GetFirst     \
        GetNext      \
        Status       

Close_SOURCES        = Close.cpp
Constructor_SOURCES  = Constructor.cpp
CreateFolder_SOURCES = CreateFolder.cpp
CreateObject_SOURCES = CreateObject.cpp
Destroy_SOURCES      = Destroy.cpp
GetFirst_SOURCES     = GetFirst.cpp
GetNext_SOURCES      = GetNext.cpp
Status_SOURCES       = Status.cpp

EXTRA_DIST =            \
        Close.sh        \
        Constructor.sh  \
        CreateFolder.sh \
        CreateObject.sh \
        Destroy.sh      \
        GetFirst.sh     \
        GetNext.sh      \
        Makefile.win32  \
        RunTests.vbs    \
        Status.sh       

TESTS_ENVIRONMENT =                  \
        tcp_port=10701               \
        top_srcdir=$(top_srcdir)     \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)

TESTS =                 \
        Close.sh        \
        Constructor.sh  \
        CreateFolder.sh \
        CreateObject.sh \
        Destroy.sh      \
        GetFirst.sh     \
        GetNext.sh      \
        Status.sh       

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/C++/libphotoncpp.la 

