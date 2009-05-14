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
        CreateFolder \
        CreateMap    \
        CreateQueue  \
        Destroy      \
        GetDataDef   \
        GetDef       \
        GetFirst     \
        GetKeyDef    \
        GetNext      \
        OpenNoSess   \
        Open         \
        Status       

Close_SOURCES        = Close.c
CreateFolder_SOURCES = CreateFolder.c
CreateMap_SOURCES    = CreateMap.c
CreateQueue_SOURCES  = CreateQueue.c
Destroy_SOURCES      = Destroy.c
GetDataDef_SOURCES   = GetDataDef.c
GetDef_SOURCES       = GetDef.c
GetFirst_SOURCES     = GetFirst.c
GetKeyDef_SOURCES    = GetKeyDef.c
GetNext_SOURCES      = GetNext.c
OpenNoSess_SOURCES   = OpenNoSess.c
Open_SOURCES         = Open.c
Status_SOURCES       = Status.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.sh    \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        tcp_port=10701               \
        top_srcdir=$(top_srcdir)     \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)

TESTS = RunTests.sh

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/API/libphoton.la      \
        $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la   

