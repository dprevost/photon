# Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

check_PROGRAMS =           \
        AddNullContext     \
        AddNullManager     \
        AddNullProcess     \
        AddPass            \
        AddZeroPid         \
        FindNullContext    \
        FindNullManager    \
        FindNullProcess    \
        FindPass           \
        FindZeroPid        \
        InitNullContext    \
        InitNullManager    \
        InitPass           \
        RemoveNullContext  \
        RemoveNullManager  \
        RemoveNullProcess  \
        RemovePass         

COMMON_HEADER = procMgrTest.h

AddNullContext_SOURCES      = $(COMMON_HEADER) AddNullContext.c
AddNullManager_SOURCES      = $(COMMON_HEADER) AddNullManager.c
AddNullProcess_SOURCES      = $(COMMON_HEADER) AddNullProcess.c
AddPass_SOURCES             = $(COMMON_HEADER) AddPass.c
AddZeroPid_SOURCES          = $(COMMON_HEADER) AddZeroPid.c
FindNullContext_SOURCES     = $(COMMON_HEADER) FindNullContext.c
FindNullManager_SOURCES     = $(COMMON_HEADER) FindNullManager.c
FindNullProcess_SOURCES     = $(COMMON_HEADER) FindNullProcess.c
FindPass_SOURCES            = $(COMMON_HEADER) FindPass.c
FindZeroPid_SOURCES         = $(COMMON_HEADER) FindZeroPid.c
InitNullContext_SOURCES     = $(COMMON_HEADER) InitNullContext.c
InitNullManager_SOURCES     = $(COMMON_HEADER) InitNullManager.c
InitPass_SOURCES            = $(COMMON_HEADER) InitPass.c
RemoveNullContext_SOURCES   = $(COMMON_HEADER) RemoveNullContext.c
RemoveNullManager_SOURCES   = $(COMMON_HEADER) RemoveNullManager.c
RemoveNullProcess_SOURCES   = $(COMMON_HEADER) RemoveNullProcess.c
RemovePass_SOURCES          = $(COMMON_HEADER) RemovePass.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                    \
        AddNullContext     \
        AddNullManager     \
        AddNullProcess     \
        AddPass            \
        AddZeroPid         \
        FindNullContext    \
        FindNullManager    \
        FindNullProcess    \
        FindPass           \
        FindZeroPid        \
        InitNullContext    \
        InitNullManager    \
        InitPass           \
        RemoveNullContext  \
        RemoveNullManager  \
        RemoveNullProcess  \
        RemovePass         

XFAIL_TESTS =              \
        AddNullContext     \
        AddNullManager     \
        AddNullProcess     \
        AddZeroPid         \
        FindNullContext    \
        FindNullManager    \
        FindNullProcess    \
        FindZeroPid        \
        InitNullContext    \
        InitNullManager    \
        RemoveNullContext  \
        RemoveNullManager  \
        RemoveNullProcess  

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

