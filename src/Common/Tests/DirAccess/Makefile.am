# Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~ core

check_PROGRAMS =                   \
                 CloseInvalidSig   \
                 CloseNullDir      \
                 ClosePass         \
                 FiniInvalidSig    \
                 FiniNullDir       \
                 FiniPass          \
                 GetNextInvalidSig \
                 GetNextNoOpen     \
                 GetNextNoSuchDir  \
                 GetNextNullDir    \
                 GetNextNullError  \
                 GetNextNullpDir   \
                 GetNextPass       \
                 InitNullDir       \
                 InitPass          \
                 OpenCallTwice     \
                 OpenInvalidSig    \
                 OpenNoInit        \
                 OpenNoSuchDir     \
                 OpenNullDir       \
                 OpenNullDirname   \
                 OpenNullError     \
                 OpenPass          \
                 Tests             

CloseInvalidSig_SOURCES   = CloseInvalidSig.c
CloseNullDir_SOURCES      = CloseNullDir.c
ClosePass_SOURCES         = ClosePass.c
FiniInvalidSig_SOURCES    = FiniInvalidSig.c
FiniNullDir_SOURCES       = FiniNullDir.c
FiniPass_SOURCES          = FiniPass.c
GetNextInvalidSig_SOURCES = GetNextInvalidSig.c
GetNextNoOpen_SOURCES     = GetNextNoOpen.c
GetNextNoSuchDir_SOURCES  = GetNextNoSuchDir.c
GetNextNullDir_SOURCES    = GetNextNullDir.c
GetNextNullError_SOURCES  = GetNextNullError.c
GetNextNullpDir_SOURCES   = GetNextNullpDir.c
GetNextPass_SOURCES       = GetNextPass.c
InitNullDir_SOURCES       = InitNullDir.c
InitPass_SOURCES          = InitPass.c
OpenCallTwice_SOURCES     = OpenCallTwice.c
OpenInvalidSig_SOURCES    = OpenInvalidSig.c
OpenNoInit_SOURCES        = OpenNoInit.c
OpenNoSuchDir_SOURCES     = OpenNoSuchDir.c
OpenNullDir_SOURCES       = OpenNullDir.c
OpenNullDirname_SOURCES   = OpenNullDirname.c
OpenNullError_SOURCES     = OpenNullError.c
OpenPass_SOURCES          = OpenPass.c
Tests_SOURCES             = Tests.c

EXTRA_DIST = 

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS = 

if COND_USE_DBC
TESTS +=                  \
        CloseInvalidSig   \
        CloseNullDir      \
        ClosePass         \
        FiniInvalidSig    \
        FiniNullDir       \
        FiniPass          \
        GetNextInvalidSig \
        GetNextNoOpen     \
        GetNextNoSuchDir  \
        GetNextNullDir    \
        GetNextNullError  \
        GetNextNullpDir   \
        GetNextPass       \
        InitNullDir       \
        InitPass          \
        OpenCallTwice     \
        OpenInvalidSig    \
        OpenNoInit        \
        OpenNoSuchDir     \
        OpenNullDir       \
        OpenNullDirname   \
        OpenNullError     \
        OpenPass
endif

if COND_USE_EXTREME_DBC
TESTS += 
endif

TESTS += Tests 

XFAIL_TESTS =                   \
              CloseInvalidSig   \
              CloseNullDir      \
              FiniInvalidSig    \
              FiniNullDir       \
              GetNextInvalidSig \
              GetNextNoOpen     \
              GetNextNullDir    \
              GetNextNullError  \
              GetNextNullpDir   \
              InitNullDir       \
              OpenCallTwice     \
              OpenInvalidSig    \
              OpenNoInit        \
              OpenNullDir       \
              OpenNullDirname   \
              OpenNullError

LDADD = $(top_builddir)/src/Common/libCommon.la

AM_CPPFLAGS = -I$(top_srcdir)/src/Common \
              -I$(top_srcdir)/src/include

if COND_USE_DBC
   AM_CPPFLAGS += -DUSE_DBC
endif
if COND_USE_EXTREME_DBC
   AM_CPPFLAGS += -DUSE_EXTREME_DBC
endif

distclean-local:
	-rm -rf $(testsubdir)

