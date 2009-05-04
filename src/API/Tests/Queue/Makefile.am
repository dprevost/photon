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

check_PROGRAMS =          \
        ClosePass         \
        Create            \
        Definition        \
        FirstNullData     \
        FirstNullHandle   \
        FirstNullLength   \
        FirstPass         \
        FirstWrongHandle  \
        GetFirst          \
        GetNext           \
        NextNoFirst       \
        NextNullData      \
        NextNullHandle    \
        NextNullLength    \
        NextPass          \
        NextWrongHandle   \
        OpenNoSession     \
        OpenPass          \
        Pop               \
        PushNow           \
        PushPass          \
        RemoveNullData    \
        RemoveNullHandle  \
        RemoveNullLength  \
        RemovePass        \
        RemoveWrongHandle \
        StatusPass        

ClosePass_SOURCES         = ClosePass.c
Create_SOURCES            = Create.c
Definition_SOURCES        = Definition.c
FirstNullData_SOURCES     = FirstNullData.c
FirstNullHandle_SOURCES   = FirstNullHandle.c
FirstNullLength_SOURCES   = FirstNullLength.c
FirstPass_SOURCES         = FirstPass.c
FirstWrongHandle_SOURCES  = FirstWrongHandle.c
GetFirst_SOURCES          = GetFirst.c
GetNext_SOURCES           = GetNext.c
NextNoFirst_SOURCES       = NextNoFirst.c
NextNullData_SOURCES      = NextNullData.c
NextNullHandle_SOURCES    = NextNullHandle.c
NextNullLength_SOURCES    = NextNullLength.c
NextPass_SOURCES          = NextPass.c
NextWrongHandle_SOURCES   = NextWrongHandle.c
OpenNoSession_SOURCES     = OpenNoSession.c
OpenPass_SOURCES          = OpenPass.c
Pop_SOURCES               = Pop.c
PushNow_SOURCES           = PushNow.c
PushPass_SOURCES          = PushPass.c
RemoveNullData_SOURCES    = RemoveNullData.c
RemoveNullHandle_SOURCES  = RemoveNullHandle.c
RemoveNullLength_SOURCES  = RemoveNullLength.c
RemovePass_SOURCES        = RemovePass.c
RemoveWrongHandle_SOURCES = RemoveWrongHandle.c
StatusPass_SOURCES        = StatusPass.c

EXTRA_DIST =                 \
        ClosePass.sh         \
        Create.sh            \
        Definition.sh        \
        FirstNullData.sh     \
        FirstNullHandle.sh   \
        FirstNullLength.sh   \
        FirstPass.sh         \
        FirstWrongHandle.sh  \
        GetFirst.sh          \
        GetNext.sh           \
        Makefile.win32       \
        NextNoFirst.sh       \
        NextNullData.sh      \
        NextNullHandle.sh    \
        NextNullLength.sh    \
        NextPass.sh          \
        NextWrongHandle.sh   \
        OpenNoSession.sh     \
        OpenPass.sh          \
        Pop.sh               \
        PushNow.sh           \
        PushPass.sh          \
        RemoveNullData.sh    \
        RemoveNullHandle.sh  \
        RemoveNullLength.sh  \
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
        Create.sh            \
        Definition.sh        \
        FirstNullData.sh     \
        FirstNullHandle.sh   \
        FirstNullLength.sh   \
        FirstPass.sh         \
        FirstWrongHandle.sh  \
        GetFirst.sh          \
        GetNext.sh           \
        NextNoFirst.sh       \
        NextNullData.sh      \
        NextNullHandle.sh    \
        NextNullLength.sh    \
        NextPass.sh          \
        NextWrongHandle.sh   \
        OpenNoSession.sh     \
        OpenPass.sh          \
        Pop.sh               \
        PushNow.sh           \
        PushPass.sh          \
        RemoveNullData.sh    \
        RemoveNullHandle.sh  \
        RemoveNullLength.sh  \
        RemovePass.sh        \
        RemoveWrongHandle.sh \
        StatusPass.sh        

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/API/libphoton.la      \
        $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la   

