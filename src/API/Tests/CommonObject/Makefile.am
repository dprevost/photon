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

check_PROGRAMS =                  \
                 CloseNullObject  \
                 ClosePass        \
                 LockNullObject   \
                 LockPass         \
                 OpenNullName     \
                 OpenNullObject   \
                 OpenPass         \
                 OpenWrongType    \
                 UnlockNullObject \
                 UnlockPass       

#void vdsaCommonCloseObject( vdsaCommonObject * pObject )????????????????

CloseNullObject_SOURCES  = CloseNullObject.c
ClosePass_SOURCES        = ClosePass.c
LockNullObject_SOURCES   = LockNullObject.c
LockPass_SOURCES         = LockPass.c
OpenNullName_SOURCES     = OpenNullName.c
OpenNullObject_SOURCES   = OpenNullObject.c
OpenPass_SOURCES         = OpenPass.c
OpenWrongType_SOURCES    = OpenWrongType.c
UnlockNullObject_SOURCES = UnlockNullObject.c
UnlockPass_SOURCES       = UnlockPass.c

EXTRA_DIST =                     \
             CloseNullObject.sh  \
             ClosePass.sh        \
             LockNullObject.sh   \
             LockPass.sh         \
             OpenNullName.sh     \
             OpenNullObject.sh   \
             OpenPass.sh         \
             OpenWrongType.sh    \
             UnlockNullObject.sh \
             UnlockPass.sh       

TESTS_ENVIRONMENT =                              \
                    tcp_port=10701               \
                    top_srcdir=$(top_srcdir)     \
                    srcdir=$(srcdir)             \
                    top_builddir=$(top_builddir) \
                    verbose=1                    \
                    PYTHON=$(PYTHON)

TESTS =                     \
        CloseNullObject.sh  \
        ClosePass.sh        \
        LockNullObject.sh   \
        LockPass.sh         \
        OpenNullName.sh     \
        OpenNullObject.sh   \
        OpenPass.sh         \
        OpenWrongType.sh    \
        UnlockNullObject.sh \
        UnlockPass.sh       

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/API/libvdsf.la      \
        $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

