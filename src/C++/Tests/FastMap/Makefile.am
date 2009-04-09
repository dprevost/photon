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

check_PROGRAMS =    \
        Close       \
        DataDefODBC \
        Definition  \
        Delete      \
        Empty       \
        Get         \
        GetFirst    \
        GetNext     \
        Insert      \
        Open        \
        Replace     \
        Status      

Close_SOURCES       = Close.cpp
DataDefODBC_SOURCES = DataDefODBC.cpp
Definition_SOURCES  = Definition.cpp
Delete_SOURCES      = Delete.cpp
Empty_SOURCES       = Empty.cpp
Get_SOURCES         = Get.cpp
GetFirst_SOURCES    = GetFirst.cpp
GetNext_SOURCES     = GetNext.cpp
Insert_SOURCES      = Insert.cpp
Open_SOURCES        = Open.cpp
Replace_SOURCES     = Replace.cpp
Status_SOURCES      = Status.cpp

EXTRA_DIST =           \
        Close.sh       \
        DataDefODBC.sh \
        Definition.sh  \
        Delete.sh      \
        Empty.sh       \
        Get.sh         \
        GetFirst.sh    \
        GetNext.sh     \
        Insert.sh      \
        Makefile.win32 \
        Open.sh        \
        Replace.sh     \
        RunTests.vbs   \
        Status.sh      

TESTS_ENVIRONMENT =                  \
        tcp_port=10701               \
        top_srcdir=$(top_srcdir)     \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)

TESTS =                \
        Close.sh       \
        DataDefODBC.sh \
        Definition.sh  \
        Delete.sh      \
        Empty.sh       \
        Get.sh         \
        GetFirst.sh    \
        GetNext.sh     \
        Insert.sh      \
        Open.sh        \
        Replace.sh     \
        Status.sh      

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/C++/libphotoncpp.la 

