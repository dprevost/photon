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

check_PROGRAMS =               \
        AddInvalidType         \
        AddNullApiObject       \
        AddNullContext         \
        AddNullObject          \
        AddNullOffset          \
        AddNullSession         \
        AddPass                \
        FiniNullContext        \
        FiniNullSession        \
        FiniPass               \
        GetFirstNullContext    \
        GetFirstNullObject     \
        GetFirstNullSession    \
        GetFirstPass           \
        InitNullApiSession     \
        InitNullContext        \
        InitNullSession        \
        InitPass               \
        RemoveNullContext      \
        RemoveNullObject       \
        RemoveNullSession      \
        RemovePass             \
        RemoveFirstNullContext \
        RemoveFirstNullSession \
        RemoveFirstPass        

COMMON_HEADER = sessionTest.h
            
AddInvalidType_SOURCES         = $(COMMON_HEADER) AddInvalidType.c
AddNullApiObject_SOURCES       = $(COMMON_HEADER) AddNullApiObject.c
AddNullContext_SOURCES         = $(COMMON_HEADER) AddNullContext.c
AddNullObject_SOURCES          = $(COMMON_HEADER) AddNullObject.c
AddNullOffset_SOURCES          = $(COMMON_HEADER) AddNullOffset.c
AddNullSession_SOURCES         = $(COMMON_HEADER) AddNullSession.c
AddPass_SOURCES                = $(COMMON_HEADER) AddPass.c
FiniNullContext_SOURCES        = $(COMMON_HEADER) FiniNullContext.c
FiniNullSession_SOURCES        = $(COMMON_HEADER) FiniNullSession.c
FiniPass_SOURCES               = $(COMMON_HEADER) FiniPass.c
GetFirstNullContext_SOURCES    = $(COMMON_HEADER) GetFirstNullContext.c
GetFirstNullObject_SOURCES     = $(COMMON_HEADER) GetFirstNullObject.c
GetFirstNullSession_SOURCES    = $(COMMON_HEADER) GetFirstNullSession.c
GetFirstPass_SOURCES           = $(COMMON_HEADER) GetFirstPass.c
InitNullApiSession_SOURCES     = $(COMMON_HEADER) InitNullApiSession.c
InitNullContext_SOURCES        = $(COMMON_HEADER) InitNullContext.c
InitNullSession_SOURCES        = $(COMMON_HEADER) InitNullSession.c
InitPass_SOURCES               = $(COMMON_HEADER) InitPass.c
RemoveNullContext_SOURCES      = $(COMMON_HEADER) RemoveNullContext.c
RemoveNullObject_SOURCES       = $(COMMON_HEADER) RemoveNullObject.c
RemoveNullSession_SOURCES      = $(COMMON_HEADER) RemoveNullSession.c
RemovePass_SOURCES             = $(COMMON_HEADER) RemovePass.c
RemoveFirstNullContext_SOURCES = $(COMMON_HEADER) RemoveFirstNullContext.c
RemoveFirstNullSession_SOURCES = $(COMMON_HEADER) RemoveFirstNullSession.c
RemoveFirstPass_SOURCES        = $(COMMON_HEADER) RemoveFirstPass.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                        \
        AddInvalidType         \
        AddNullApiObject       \
        AddNullContext         \
        AddNullObject          \
        AddNullOffset          \
        AddNullSession         \
        AddPass                \
        FiniNullContext        \
        FiniNullSession        \
        FiniPass               \
        GetFirstNullContext    \
        GetFirstNullObject     \
        GetFirstNullSession    \
        GetFirstPass           \
        InitNullApiSession     \
        InitNullContext        \
        InitNullSession        \
        InitPass               \
        RemoveNullContext      \
        RemoveNullObject       \
        RemoveNullSession      \
        RemovePass             \
        RemoveFirstNullContext \
        RemoveFirstNullSession \
        RemoveFirstPass        

XFAIL_TESTS =                  \
        AddInvalidType         \
        AddNullApiObject       \
        AddNullContext         \
        AddNullObject          \
        AddNullOffset          \
        AddNullSession         \
        FiniNullContext        \
        FiniNullSession        \
        GetFirstNullContext    \
        GetFirstNullObject     \
        GetFirstNullSession    \
        InitNullApiSession     \
        InitNullContext        \
        InitNullSession        \
        RemoveNullContext      \
        RemoveNullObject       \
        RemoveNullSession      \
        RemoveFirstNullContext \
        RemoveFirstNullSession 

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

