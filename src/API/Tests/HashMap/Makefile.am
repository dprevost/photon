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

check_PROGRAMS =                     \
                 CloseNullHandle     \
                 ClosePass           \
                 CloseWrongHandle    \
                 OpenNullName        \
                 OpenNullObjHandle   \
                 OpenNullSessHandle  \
                 OpenPass            \
                 OpenWrongSessHandle 

#                 GetFirstNullEntry   \
                 GetFirstNullHandle  \
                 GetFirstPass        \
                 GetFirstWrongHandle \
                 GetNextNoFirst      \
                 GetNextNullEntry    \
                 GetNextNullHandle   \
                 GetNextPass         \
                 GetNextWrongHandle  

CloseNullHandle_SOURCES     = CloseNullHandle.c
ClosePass_SOURCES           = ClosePass.c
CloseWrongHandle_SOURCES    = CloseWrongHandle.c
GetFirstNullEntry_SOURCES   = GetFirstNullEntry.c
GetFirstNullHandle_SOURCES  = GetFirstNullHandle.c
GetFirstPass_SOURCES        = GetFirstPass.c
GetFirstWrongHandle_SOURCES = GetFirstWrongHandle.c
GetNextPass_SOURCES         = GetNextPass.c
OpenNullName_SOURCES        = OpenNullName.c
OpenNullObjHandle_SOURCES   = OpenNullObjHandle.c
OpenNullSessHandle_SOURCES  = OpenNullSessHandle.c
OpenPass_SOURCES            = OpenPass.c
OpenWrongSessHandle_SOURCES = OpenWrongSessHandle.c

EXTRA_DIST =                        \
             CloseNullHandle.sh     \
             ClosePass.sh           \
             CloseWrongHandle.sh    \
             GetFirstNullEntry.sh   \
             GetFirstNullHandle.sh  \
             GetFirstPass.sh        \
             GetFirstWrongHandle.sh \
             GetNextNoFirst.sh      \
             GetNextNullEntry.sh    \
             GetNextNullHandle.sh   \
             GetNextPass.sh         \
             GetNextWrongHandle.sh  \
             OpenNullName.sh        \
             OpenNullObjHandle.sh   \
             OpenNullSessHandle.sh  \
             OpenPass.sh            \
             OpenWrongSessHandle.sh 

TESTS_ENVIRONMENT =                              \
                    tcp_port=10701               \
                    top_srcdir=$(top_srcdir)     \
                    srcdir=$(srcdir)             \
                    top_builddir=$(top_builddir) \
                    verbose=1                    \
                    PYTHON=$(PYTHON)

TESTS =                        \
        CloseNullHandle.sh     \
        ClosePass.sh           \
        CloseWrongHandle.sh    
#        OpenNullName.sh        \
        OpenNullObjHandle.sh   \
        OpenNullSessHandle.sh  \
        OpenPass.sh            \
        OpenWrongSessHandle.sh 

#        GetFirstNullEntry.sh   \
        GetFirstNullHandle.sh  \
        GetFirstPass.sh        \
        GetFirstWrongHandle.sh \
        GetNextNoFirst.sh      \
        GetNextNullEntry.sh    \
        GetNextNullHandle.sh   \
        GetNextPass.sh         \
        GetNextWrongHandle.sh  

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/API/libvdsf.la      \
        $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

