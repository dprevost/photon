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

check_PROGRAMS =          \
        FiniNullContext   \
        FiniNullObj       \
        FiniPass          \
        FreeNullContext   \
        FreeNullObj       \
        FreeNullPtr       \
        FreePass          \
        FreeZeroBytes     \
        InitInvType       \
        InitNullGroup     \
        InitNullObj       \
        InitPass          \
        InitZeroPages     \
        InitZeroType      \
        MallocNullContext \
        MallocNullObj     \
        MallocPass        \
        MallocZeroBytes   \
        StatusNullMemObj  \
        StatusNullStatus  \
        StatusPass        \
        Ticket13

COMMON_HEADER = MemObjTest.h

FiniNullContext_SOURCES   = $(COMMON_HEADER) FiniNullContext.c
FiniNullObj_SOURCES       = $(COMMON_HEADER) FiniNullObj.c
FiniPass_SOURCES          = $(COMMON_HEADER) FiniPass.c
FreeNullContext_SOURCES   = $(COMMON_HEADER) FreeNullContext.c
FreeNullObj_SOURCES       = $(COMMON_HEADER) FreeNullObj.c
FreeNullPtr_SOURCES       = $(COMMON_HEADER) FreeNullPtr.c
FreePass_SOURCES          = $(COMMON_HEADER) FreePass.c
FreeZeroBytes_SOURCES     = $(COMMON_HEADER) FreeZeroBytes.c
InitInvType_SOURCES       = $(COMMON_HEADER) InitInvType.c
InitNullGroup_SOURCES     = $(COMMON_HEADER) InitNullGroup.c
InitNullObj_SOURCES       = $(COMMON_HEADER) InitNullObj.c
InitPass_SOURCES          = $(COMMON_HEADER) InitPass.c
InitZeroPages_SOURCES     = $(COMMON_HEADER) InitZeroPages.c
InitZeroType_SOURCES      = $(COMMON_HEADER) InitZeroType.c
MallocNullContext_SOURCES = $(COMMON_HEADER) MallocNullContext.c
MallocNullObj_SOURCES     = $(COMMON_HEADER) MallocNullObj.c
MallocPass_SOURCES        = $(COMMON_HEADER) MallocPass.c
MallocZeroBytes_SOURCES   = $(COMMON_HEADER) MallocZeroBytes.c
StatusNullMemObj_SOURCES  = $(COMMON_HEADER) StatusNullMemObj.c
StatusNullStatus_SOURCES  = $(COMMON_HEADER) StatusNullStatus.c
StatusPass_SOURCES        = $(COMMON_HEADER) StatusPass.c
Ticket13_SOURCES          = $(COMMON_HEADER) Ticket13.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                   \
        FiniNullContext   \
        FiniNullObj       \
        FiniPass          \
        FreeNullContext   \
        FreeNullObj       \
        FreeNullPtr       \
        FreePass          \
        FreeZeroBytes     \
        InitInvType       \
        InitNullGroup     \
        InitNullObj       \
        InitPass          \
        InitZeroPages     \
        InitZeroType      \
        MallocNullContext \
        MallocNullObj     \
        MallocPass        \
        MallocZeroBytes   \
        StatusNullMemObj  \
        StatusNullStatus  \
        StatusPass        \
        Ticket13

XFAIL_TESTS =             \
        FiniNullContext   \
        FiniNullObj       \
        FreeNullContext   \
        FreeNullObj       \
        FreeNullPtr       \
        FreeZeroBytes     \
        InitInvType       \
        InitNullGroup     \
        InitNullObj       \
        InitZeroPages     \
        InitZeroType      \
        MallocNullContext \
        MallocNullObj     \
        MallocZeroBytes   \
        StatusNullMemObj  \
        StatusNullStatus  

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

