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

check_PROGRAMS =          \
        Close             \
        Create            \
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
        Push              \
        RemoveNullEntry   \
        RemoveNullHandle  \
        RemovePass        \
        RemoveWrongHandle \
        Status            

Close_SOURCES             = Close.c
Create_SOURCES            = Create.c
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
Push_SOURCES              = Push.c
RemoveNullEntry_SOURCES   = RemoveNullEntry.c
RemoveNullHandle_SOURCES  = RemoveNullHandle.c
RemovePass_SOURCES        = RemovePass.c
RemoveWrongHandle_SOURCES = RemoveWrongHandle.c
Status_SOURCES            = Status.c

EXTRA_DIST =                 \
        Close.sh             \
        Create.sh            \
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
        Push.sh              \
        RemoveNullEntry.sh   \
        RemoveNullHandle.sh  \
        RemovePass.sh        \
        RemoveWrongHandle.sh \
        RunTests.vbs         \
        Status.sh            

TESTS_ENVIRONMENT =                  \
        tcp_port=10701               \
        top_srcdir=$(top_srcdir)     \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                      \
        Close.sh             \
        Create.sh            \
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
        Push.sh              \
        RemoveNullEntry.sh   \
        RemoveNullHandle.sh  \
        RemovePass.sh        \
        RemoveWrongHandle.sh \
        Status.sh            

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/API/libphoton.la      \
        $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la   

