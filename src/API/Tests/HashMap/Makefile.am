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

check_PROGRAMS =                      \
                 CloseNullHandle      \
                 ClosePass            \
                 CloseWrongHandle     \
                 DeleteNullHandle     \
                 DeleteNullKey        \
                 DeletePass           \
                 DeleteWrongHandle    \
                 DeleteZeroLength     \
                 FirstNullEntry       \
                 FirstNullHandle      \
                 FirstPass            \
                 FirstWrongHandle     \
                 GetNullEntry         \
                 GetNullHandle        \
                 GetNullKey           \
                 GetPass              \
                 GetWrongHandle       \
                 GetZeroLength        \ 
                 InsertNullData       \
                 InsertNullHandle     \
                 InsertNullKey        \
                 InsertPass           \
                 InsertWrongHandle    \
                 InsertZeroDataLength \
                 InsertZeroKeyLength  \
                 NextNoFirst          \
                 NextNullEntry        \
                 NextNullHandle       \
                 NextPass             \
                 NextWrongHandle      \
                 OpenNullName         \
                 OpenNullObjHandle    \
                 OpenNullSessHandle   \
                 OpenPass             \
                 OpenWrongSessHandle  

CloseNullHandle_SOURCES      = CloseNullHandle.c
ClosePass_SOURCES            = ClosePass.c
CloseWrongHandle_SOURCES     = CloseWrongHandle.c
DeleteNullHandle_SOURCES     = DeleteNullHandle.c
DeleteNullKey_SOURCES        = DeleteNullKey.c
DeletePass_SOURCES           = DeletePass.c
DeleteWrongHandle_SOURCES    = DeleteWrongHandle.c
DeleteZeroLength_SOURCES     = DeleteZeroLength.c
FirstNullEntry_SOURCES       = FirstNullEntry.c
FirstNullHandle_SOURCES      = FirstNullHandle.c
FirstPass_SOURCES            = FirstPass.c
FirstWrongHandle_SOURCES     = FirstWrongHandle.c
GetNullEntry_SOURCES         = GetNullEntry.c
GetNullHandle_SOURCES        = GetNullHandle.c
GetNullKey_SOURCES           = GetNullKey.c
GetPass_SOURCES              = GetPass.c
GetWrongHandle_SOURCES       = GetWrongHandle.c
GetZeroLength_SOURCES        = GetZeroLength.c
InsertNullData_SOURCES       = InsertNullData.c
InsertNullHandle_SOURCES     = InsertNullHandle.c
InsertNullKey_SOURCES        = InsertNullKey.c
InsertPass_SOURCES           = InsertPass.c
InsertWrongHandle_SOURCES    = InsertWrongHandle.c
InsertZeroDataLength_SOURCES = InsertZeroDataLength.c
InsertZeroKeyLength_SOURCES  = InsertZeroKeyLength.c
NextPass_SOURCES             = NextPass.c
OpenNullName_SOURCES         = OpenNullName.c
OpenNullObjHandle_SOURCES    = OpenNullObjHandle.c
OpenNullSessHandle_SOURCES   = OpenNullSessHandle.c
OpenPass_SOURCES             = OpenPass.c
OpenWrongSessHandle_SOURCES  = OpenWrongSessHandle.c

EXTRA_DIST =                         \
             CloseNullHandle.sh      \
             ClosePass.sh            \
             CloseWrongHandle.sh     \
             DeleteNullHandle.sh     \
             DeleteNullKey.sh        \
             DeletePass.sh           \
             DeleteWrongHandle.sh    \
             DeleteZeroLength.sh     \
             FirstNullEntry.sh       \
             FirstNullHandle.sh      \
             FirstPass.sh            \
             FirstWrongHandle.sh     \
             GetNullEntry.sh         \
             GetNullHandle.sh        \
             GetNullKey.sh           \
             GetPass.sh              \
             GetWrongHandle.sh       \
             GetZeroLength.sh        \
             NextNoFirst.sh          \
             NextNullEntry.sh        \
             NextNullHandle.sh       \
             NextPass.sh             \
             NextWrongHandle.sh      \
             InsertNullData.sh       \
             InsertNullHandle.sh     \
             InsertNullKey.sh        \
             InsertPass.sh           \
             InsertWrongHandle.sh    \
             InsertZeroDataLength.sh \
             InsertZeroKeyLength.sh  \
             OpenNullName.sh         \
             OpenNullObjHandle.sh    \
             OpenNullSessHandle.sh   \
             OpenPass.sh             \
             OpenWrongSessHandle.sh  

TESTS_ENVIRONMENT =                              \
                    tcp_port=10701               \
                    top_srcdir=$(top_srcdir)     \
                    srcdir=$(srcdir)             \
                    top_builddir=$(top_builddir) \
                    verbose=1                    \
                    PYTHON=$(PYTHON)

TESTS =                         \
        CloseNullHandle.sh      \
        ClosePass.sh            \
        CloseWrongHandle.sh     \
        DeleteNullHandle.sh     \
        DeleteNullKey.sh        \
        DeletePass.sh           \
        DeleteWrongHandle.sh    \
        DeleteZeroLength.sh     \
        FirstNullEntry.sh       \
        FirstNullHandle.sh      \
        FirstPass.sh            \
        FirstWrongHandle.sh     \
        GetNullEntry.sh         \
        GetNullHandle.sh        \
        GetNullKey.sh           \
        GetPass.sh              \
        GetWrongHandle.sh       \
        GetZeroLength.sh        \ 
        InsertNullData.sh       \
        InsertNullHandle.sh     \
        InsertNullKey.sh        \
        InsertPass.sh           \
        InsertWrongHandle.sh    \
        InsertZeroDataLength.sh \
        InsertZeroKeyLength.sh  \
        NextNoFirst.sh          \
        NextNullEntry.sh        \
        NextNullHandle.sh       \
        NextPass.sh             \
        NextWrongHandle.sh      \
        OpenNullName.sh         \
        OpenNullObjHandle.sh    \
        OpenNullSessHandle.sh   \
        OpenPass.sh             \
        OpenWrongSessHandle.sh  

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/API/libvdsf.la      \
        $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

