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

INCLUDE_FILES = PrintError.h

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

CloseInvalidSig_SOURCES   = $(INCLUDE_FILES) CloseInvalidSig.c
CloseNullDir_SOURCES      = $(INCLUDE_FILES) CloseNullDir.c
ClosePass_SOURCES         = $(INCLUDE_FILES) ClosePass.c
FiniInvalidSig_SOURCES    = $(INCLUDE_FILES) FiniInvalidSig.c
FiniNullDir_SOURCES       = $(INCLUDE_FILES) FiniNullDir.c
FiniPass_SOURCES          = $(INCLUDE_FILES) FiniPass.c
GetNextInvalidSig_SOURCES = $(INCLUDE_FILES) GetNextInvalidSig.c
GetNextNoOpen_SOURCES     = $(INCLUDE_FILES) GetNextNoOpen.c
GetNextNoSuchDir_SOURCES  = $(INCLUDE_FILES) GetNextNoSuchDir.c
GetNextNullDir_SOURCES    = $(INCLUDE_FILES) GetNextNullDir.c
GetNextNullError_SOURCES  = $(INCLUDE_FILES) GetNextNullError.c
GetNextNullpDir_SOURCES   = $(INCLUDE_FILES) GetNextNullpDir.c
GetNextPass_SOURCES       = $(INCLUDE_FILES) GetNextPass.c
InitNullDir_SOURCES       = $(INCLUDE_FILES) InitNullDir.c
InitPass_SOURCES          = $(INCLUDE_FILES) InitPass.c
OpenCallTwice_SOURCES     = $(INCLUDE_FILES) OpenCallTwice.c
OpenInvalidSig_SOURCES    = $(INCLUDE_FILES) OpenInvalidSig.c
OpenNoInit_SOURCES        = $(INCLUDE_FILES) OpenNoInit.c
OpenNoSuchDir_SOURCES     = $(INCLUDE_FILES) OpenNoSuchDir.c
OpenNullDir_SOURCES       = $(INCLUDE_FILES) OpenNullDir.c
OpenNullDirname_SOURCES   = $(INCLUDE_FILES) OpenNullDirname.c
OpenNullError_SOURCES     = $(INCLUDE_FILES) OpenNullError.c
OpenPass_SOURCES          = $(INCLUDE_FILES) OpenPass.c
Tests_SOURCES             = $(INCLUDE_FILES) Tests.c

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

AM_CPPFLAGS = -I$(top_srcdir)/src/Common       \
              -I$(top_srcdir)/src/Common/Tests \
              -I$(top_srcdir)/src/include

if COND_USE_DBC
   AM_CPPFLAGS += -DUSE_DBC
endif
if COND_USE_EXTREME_DBC
   AM_CPPFLAGS += -DUSE_EXTREME_DBC
endif

distclean-local:
	-rm -rf $(testsubdir)

