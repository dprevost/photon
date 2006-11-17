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

check_PROGRAMS =               \
                 InitNullObj   \
                 InitInvType   \
                 InitPass      \
                 InitZeroPages \
                 InitZeroSize  \
                 InitZeroType  

InitNullObj_SOURCES    = InitNullObj.c
InitInvType_SOURCES    = InitInvType.c
InitPass_SOURCES       = InitPass.c
InitZeroPages_SOURCES  = InitZeroPages.c
InitZeroSize_SOURCES   = InitZeroSize.c
InitZeroType_SOURCES   = InitZeroType.c

EXTRA_DIST =         

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)


TESTS =               \
        InitNullObj   \
        InitInvType   \
        InitPass      \
        InitZeroPages \
        InitZeroSize  \
        InitZeroType  

XFAIL_TESTS =               \
              InitNullObj   \
              InitInvType   \
              InitZeroPages \
              InitZeroSize  \
              InitZeroType  
              
LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

AM_CPPFLAGS = -I$(top_srcdir)/src/Engine \
              -I$(top_srcdir)/src/Common \
              -I$(top_srcdir)/src/include

if COND_USE_ASSERT
   AM_CPPFLAGS += -DUSE_ASSERT
endif
if COND_USE_TRAP_ERRORS
   AM_CPPFLAGS += -DUSE_TRAP_ERRORS
endif

if COND_USE_DBC
   AM_CPPFLAGS += -DUSE_DBC
endif
if COND_USE_EXTREME_DBC
   AM_CPPFLAGS += -DUSE_EXTREME_DBC
endif


