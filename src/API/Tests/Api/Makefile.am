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

check_PROGRAMS =                 \
                 ExitNullHandle  \
                 ExitPass        \
                 InitNullAddress \
                 InitNullHandle  \
                 InitPass         

ExitNullHandle_SOURCES  = ExitNullHandle.c
ExitPass_SOURCES        = ExitPass.c
InitNullAddress_SOURCES = InitNullAddress.c
InitNullHandle_SOURCES  = InitNullHandle.c
InitPass_SOURCES        = InitPass.c

EXTRA_DIST =         

TESTS_ENVIRONMENT =                              \
                    tcp_port=10701               \
                    top_srcdir=$(top_srcdir)     \
                    srcdir=$(srcdir)             \
                    top_builddir=$(top_builddir) \
                    verbose=1                    \
                    PYTHON=$(PYTHON)

TESTS =                   \
        ExitNullHandle.sh \
        ExitPass.sh       \
        InitNullAddress   \
        InitNullHandle    \
        InitPass.sh       

XFAIL_TESTS =                 \
              ExitNullHandle  

LDADD = $(top_builddir)/src/API/libvdsf.la      \
        $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

