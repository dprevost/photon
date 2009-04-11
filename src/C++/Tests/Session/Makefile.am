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

check_PROGRAMS =        \
        Commit          \
        CreateFolder    \
        CreateObject    \
        CreateObjectMap \
        Destroy         \
        ErrorMsg        \
        Exit            \
        GetInfo         \
        Init            \
        LastError       \
        Rollback        \
        Status          

Commit_SOURCES          = Commit.cpp
CreateFolder_SOURCES    = CreateFolder.cpp
CreateObject_SOURCES    = CreateObject.cpp
CreateObjectMap_SOURCES = CreateObjectMap.cpp
Destroy_SOURCES         = Destroy.cpp
ErrorMsg_SOURCES        = ErrorMsg.cpp
Exit_SOURCES            = Exit.cpp
GetInfo_SOURCES         = GetInfo.cpp
Init_SOURCES            = Init.cpp
LastError_SOURCES       = LastError.cpp
Rollback_SOURCES        = Rollback.cpp
Status_SOURCES          = Status.cpp

EXTRA_DIST =               \
        Commit.sh          \
        CreateFolder.sh    \
        CreateObject.sh    \
        CreateObjectMap.sh \
        Destroy.sh         \
        ErrorMsg.sh        \
        Exit.sh            \
        GetInfo.sh         \
        Init.sh            \
        LastError.sh       \
        Makefile.win32     \
        Rollback.sh        \
        RunTests.vbs       \
        Status.sh          

TESTS_ENVIRONMENT =                  \
        tcp_port=10701               \
        top_srcdir=$(top_srcdir)     \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                    \
        Commit.sh          \
        CreateFolder.sh    \
        CreateObject.sh    \
        CreateObjectMap.sh \
        Destroy.sh         \
        ErrorMsg.sh        \
        Exit.sh            \
        GetInfo.sh         \
        Init.sh            \
        LastError.sh       \
        Rollback.sh        \
        Status.sh          

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/C++/libphotoncpp.la 

