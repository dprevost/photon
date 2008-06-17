# Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

check_PROGRAMS =          \
        ClosePass         \
        Definition        \
        FirstNullEntry    \
        FirstNullHandle   \
        FirstPass         \
        FirstWrongHandle  \
        GetFirst          \
        GetNext           \
        NextNoFirst       \
        NextNullEntry     \
        NextNullHandle    \
        NextPass          \
        NextWrongHandle   \
        OpenNoSession     \
        OpenPass          \
        Pop               \
        PushPass          \
        RemoveNullEntry   \
        RemoveNullHandle  \
        RemovePass        \
        RemoveWrongHandle \
        StatusPass        

ClosePass_SOURCES         = ClosePass.c
Definition_SOURCES        = Definition.c
FirstNullEntry_SOURCES    = FirstNullEntry.c
FirstNullHandle_SOURCES   = FirstNullHandle.c
FirstPass_SOURCES         = FirstPass.c
FirstWrongHandle_SOURCES  = FirstWrongHandle.c
GetFirst_SOURCES          = GetFirst.c
GetNext_SOURCES           = GetNext.c
NextNoFirst_SOURCES       = NextNoFirst.c
NextNullEntry_SOURCES     = NextNullEntry.c
NextNullHandle_SOURCES    = NextNullHandle.c
NextPass_SOURCES          = NextPass.c
NextWrongHandle_SOURCES   = NextWrongHandle.c
OpenNoSession_SOURCES     = OpenNoSession.c
OpenPass_SOURCES          = OpenPass.c
Pop_SOURCES               = Pop.c
PushPass_SOURCES          = PushPass.c
RemoveNullEntry_SOURCES   = RemoveNullEntry.c
RemoveNullHandle_SOURCES  = RemoveNullHandle.c
RemovePass_SOURCES        = RemovePass.c
RemoveWrongHandle_SOURCES = RemoveWrongHandle.c
StatusPass_SOURCES        = StatusPass.c

EXTRA_DIST =                 \
        ClosePass.sh         \
        Definition.sh        \
        FirstNullEntry.sh    \
        FirstNullHandle.sh   \
        FirstPass.sh         \
        FirstWrongHandle.sh  \
        GetFirst.sh          \
        GetNext.sh           \
        Makefile.win32       \
        NextNoFirst.sh       \
        NextNullEntry.sh     \
        NextNullHandle.sh    \
        NextPass.sh          \
        NextWrongHandle.sh   \
        OpenNoSession.sh     \
        OpenPass.sh          \
        Pop.sh               \
        PushPass.sh          \
        RemoveNullEntry.sh   \
        RemoveNullHandle.sh  \
        RemovePass.sh        \
        RemoveWrongHandle.sh \
        RunTests.vbs         \
        StatusPass.sh        

TESTS_ENVIRONMENT =                  \
        tcp_port=10701               \
        top_srcdir=$(top_srcdir)     \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                      \
        ClosePass.sh         \
        Definition.sh        \
        FirstNullEntry.sh    \
        FirstNullHandle.sh   \
        FirstPass.sh         \
        FirstWrongHandle.sh  \
        GetFirst.sh          \
        GetNext.sh           \
        NextNoFirst.sh       \
        NextNullEntry.sh     \
        NextNullHandle.sh    \
        NextPass.sh          \
        NextWrongHandle.sh   \
        OpenNoSession.sh     \
        OpenPass.sh          \
        Pop.sh               \
        PushPass.sh          \
        RemoveNullEntry.sh   \
        RemoveNullHandle.sh  \
        RemovePass.sh        \
        RemoveWrongHandle.sh \
        StatusPass.sh        

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/API/libvdsf.la      \
        $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

