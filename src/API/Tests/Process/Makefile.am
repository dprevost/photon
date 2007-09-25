# Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

check_PROGRAMS =                  \
                 CloseNullContext \
                 CloseNullProcess \
                 ClosePass        \
                 FiniNullProcess  \
                 FiniPass         \
                 InitNullAddr     \
                 InitNullProcess  \
                 InitPass         \
                 InitWrongAddr    \
                 OpenNullContext  \
                 OpenNullFilename \
                 OpenNullProcess  \
                 OpenPass         

CloseNullContext_SOURCES = CloseNullContext.c
CloseNullProcess_SOURCES = CloseNullProcess.c
ClosePass_SOURCES        = ClosePass.c
FiniNullProcess_SOURCES  = FiniNullProcess.c
FiniPass_SOURCES         = FiniPass.c
InitNullAddr_SOURCES     = InitNullAddr.c
InitNullProcess_SOURCES  = InitNullProcess.c
InitPass_SOURCES         = InitPass.c
InitWrongAddr_SOURCES    = InitWrongAddr.c
OpenNullContext_SOURCES  = OpenNullContext.c
OpenNullFilename_SOURCES = OpenNullFilename.c
OpenNullProcess_SOURCES  = OpenNullProcess.c
OpenPass_SOURCES         = OpenPass.c

EXTRA_DIST =         

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)


TESTS =                 \
        CloseNullContext \
        CloseNullProcess \
        ClosePass        \
        FiniNullProcess  \
        FiniPass         \
        InitNullAddr     \
        InitNullProcess  \
        InitPass         \
        InitWrongAddr    \
        OpenNullContext  \
        OpenNullFilename \
        OpenNullProcess  \
        OpenPass         

XFAIL_TESTS =                  \
              CloseNullContext \
              CloseNullProcess \
              FiniNullProcess  \
              InitNullAddr     \
              InitNullProcess  \
              OpenNullContext  \
              OpenNullFilename \
              OpenNullProcess  

LDADD = $(top_builddir)/src/API/libvdsf.la      \
        $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la
