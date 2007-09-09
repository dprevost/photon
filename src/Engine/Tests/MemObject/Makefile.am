# Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

check_PROGRAMS =                   \
                 FiniNullObj       \
                 FiniPass          \
                 FreeNullContext   \
                 FreeNullObj       \
                 FreeNullPtr       \
                 FreePass          \
                 FreeZeroBytes     \
                 InitNullObj       \
                 InitInvType       \
                 InitPass          \
                 InitZeroPages     \
                 InitZeroType      \
                 MallocNullContext \
                 MallocNullObj     \
                 MallocPass        \
                 MallocZeroBytes   


FiniNullObj_SOURCES       = FiniNullObj.c
FiniPass_SOURCES          = FiniPass.c
FreeNullContext_SOURCES   = FreeNullContext.c
FreeNullObj_SOURCES       = FreeNullObj.c
FreeNullPtr_SOURCES       = FreeNullPtr.c
FreePass_SOURCES          = FreePass.c
FreeZeroBytes_SOURCES     = FreeZeroBytes.c
InitNullObj_SOURCES       = InitNullObj.c
InitInvType_SOURCES       = InitInvType.c
InitPass_SOURCES          = InitPass.c
InitZeroPages_SOURCES     = InitZeroPages.c
InitZeroType_SOURCES      = InitZeroType.c
MallocNullContext_SOURCES = MallocNullContext.c
MallocNullObj_SOURCES     = MallocNullObj.c
MallocPass_SOURCES        = MallocPass.c
MallocZeroBytes_SOURCES   = MallocZeroBytes.c

EXTRA_DIST =         

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)


TESTS =                   \
        FiniNullObj       \
        FiniPass          \
        FreeNullContext   \
        FreeNullObj       \
        FreeNullPtr       \
        FreePass          \
        FreeZeroBytes     \
        InitNullObj       \
        InitInvType       \
        InitPass          \
        InitZeroPages     \
        InitZeroType      \
        MallocNullContext \
        MallocNullObj     \
        MallocPass        \
        MallocZeroBytes   

XFAIL_TESTS =                   \
              FiniNullObj       \
              FreeNullContext   \
              FreeNullObj       \
              FreeNullPtr       \
              FreeZeroBytes     \
              InitNullObj       \
              InitInvType       \
              InitZeroPages     \
              InitZeroType      \
              MallocNullContext \
              MallocNullObj     \
              MallocZeroBytes   
              
LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

