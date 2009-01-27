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

check_PROGRAMS =            \
        AddNullApiSession   \
        AddNullContext      \
        AddNullProcess      \
        AddNullSession      \
        AddPass             \
        FiniNullContext     \
        FiniNullProcess     \
        FiniPass            \
        GetFirstNullContext \
        GetFirstNullProcess \
        GetFirstNullSession \
        GetFirstPass        \
        GetNextNullContext  \
        GetNextNullCurrent  \
        GetNextNullNext     \
        GetNextNullProcess  \
        GetNextPass         \
        InitNullContext     \
        InitNullProcess     \
        InitPass            \
        InitZeroPid         \
        RemoveNullContext   \
        RemoveNullProcess   \
        RemoveNullSession   \
        RemovePass          

COMMON_HEADER = processTest.h

AddNullApiSession_SOURCES   = $(COMMON_HEADER) AddNullApiSession.c
AddNullContext_SOURCES      = $(COMMON_HEADER) AddNullContext.c
AddNullProcess_SOURCES      = $(COMMON_HEADER) AddNullProcess.c
AddNullSession_SOURCES      = $(COMMON_HEADER) AddNullSession.c
AddPass  _SOURCES           = $(COMMON_HEADER) AddPass.c
FiniNullContext_SOURCES     = $(COMMON_HEADER) FiniNullContext.c
FiniNullProcess_SOURCES     = $(COMMON_HEADER) FiniNullProcess.c
FiniPass  _SOURCES          = $(COMMON_HEADER) FiniPass.c
GetFirstNullContext_SOURCES = $(COMMON_HEADER) GetFirstNullContext.c
GetFirstNullProcess_SOURCES = $(COMMON_HEADER) GetFirstNullProcess.c
GetFirstNullSession_SOURCES = $(COMMON_HEADER) GetFirstNullSession.c
GetFirstPass_SOURCES        = $(COMMON_HEADER) GetFirstPass.c
GetNextNullContext_SOURCES  = $(COMMON_HEADER) GetNextNullContext.c
GetNextNullCurrent_SOURCES  = $(COMMON_HEADER) GetNextNullCurrent.c
GetNextNullNext_SOURCES     = $(COMMON_HEADER) GetNextNullNext.c
GetNextNullProcess_SOURCES  = $(COMMON_HEADER) GetNextNullProcess.c
GetNextPass_SOURCES         = $(COMMON_HEADER) GetNextPass.c
InitNullContext_SOURCES     = $(COMMON_HEADER) InitNullContext.c
InitNullProcess_SOURCES     = $(COMMON_HEADER) InitNullProcess.c
InitPass  _SOURCES          = $(COMMON_HEADER) InitPass.c
InitZeroPid_SOURCES         = $(COMMON_HEADER) InitZeroPid.c
RemoveNullContext_SOURCES   = $(COMMON_HEADER) RemoveNullContext.c
RemoveNullProcess_SOURCES   = $(COMMON_HEADER) RemoveNullProcess.c
RemoveNullSession_SOURCES   = $(COMMON_HEADER) RemoveNullSession.c
RemovePass_SOURCES          = $(COMMON_HEADER) RemovePass.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                     \
        AddNullApiSession   \
        AddNullContext      \
        AddNullProcess      \
        AddNullSession      \
        AddPass             \
        FiniNullContext     \
        FiniNullProcess     \
        FiniPass            \
        GetFirstNullContext \
        GetFirstNullProcess \
        GetFirstNullSession \
        GetFirstPass        \
        GetNextNullContext  \
        GetNextNullCurrent  \
        GetNextNullNext     \
        GetNextNullProcess  \
        GetNextPass         \
        InitNullContext     \
        InitNullProcess     \
        InitPass            \
        InitZeroPid         \
        RemoveNullContext   \
        RemoveNullProcess   \
        RemoveNullSession   \
        RemovePass          

XFAIL_TESTS =               \
        AddNullApiSession   \
        AddNullContext      \
        AddNullProcess      \
        AddNullSession      \
        FiniNullContext     \
        FiniNullProcess     \
        GetFirstNullContext \
        GetFirstNullProcess \
        GetFirstNullSession \
        GetNextNullContext  \
        GetNextNullCurrent  \
        GetNextNullNext     \
        GetNextNullProcess  \
        InitNullContext     \
        InitNullProcess     \
        InitZeroPid         \
        RemoveNullContext   \
        RemoveNullProcess   \
        RemoveNullSession   

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la   

