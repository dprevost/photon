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
                 FirstNullEntry      \
                 FirstNullHandle     \
                 FirstPass           \
                 FirstWrongHandle    \
                 GetFirst            \
                 GetNext             \
                 NextNoFirst         \
                 NextNullEntry       \
                 NextNullHandle      \
                 NextPass            \
                 NextWrongHandle     \
                 OpenNullName        \
                 OpenNullObjHandle   \
                 OpenNullSessHandle  \
                 OpenPass            \
                 OpenWrongSessHandle \
                 OpenZeroLength      \
                 Pop                 \
                 PushNullData        \
                 PushNullHandle      \
                 PushPass            \
                 PushWrongHandle     \
                 PushZeroLength      \
                 RemoveNullEntry     \
                 RemoveNullHandle    \
                 RemovePass          \
                 RemoveWrongHandle   \
                 StatusNullHandle    \
                 StatusNullStatus    \
                 StatusPass          

CloseNullHandle_SOURCES     = CloseNullHandle.c
ClosePass_SOURCES           = ClosePass.c
CloseWrongHandle_SOURCES    = CloseWrongHandle.c
FirstNullEntry_SOURCES      = FirstNullEntry.c
FirstNullHandle_SOURCES     = FirstNullHandle.c
FirstPass_SOURCES           = FirstPass.c
FirstWrongHandle_SOURCES    = FirstWrongHandle.c
GetFirst_SOURCES            = GetFirst.c
GetNext_SOURCES             = GetNext.c
NextNoFirst_SOURCES         = NextNoFirst.c
NextNullEntry_SOURCES       = NextNullEntry.c
NextNullHandle_SOURCES      = NextNullHandle.c
NextPass_SOURCES            = NextPass.c
NextWrongHandle_SOURCES     = NextWrongHandle.c
OpenNullName_SOURCES        = OpenNullName.c
OpenNullObjHandle_SOURCES   = OpenNullObjHandle.c
OpenNullSessHandle_SOURCES  = OpenNullSessHandle.c
OpenPass_SOURCES            = OpenPass.c
OpenWrongSessHandle_SOURCES = OpenWrongSessHandle.c
OpenZeroLength_SOURCES      = OpenZeroLength.c
Pop_SOURCES                 = Pop.c
PushNullData_SOURCES        = PushNullData.c
PushNullHandle_SOURCES      = PushNullHandle.c
PushPass_SOURCES            = PushPass.c
PushWrongHandle_SOURCES     = PushWrongHandle.c
PushZeroLength_SOURCES      = PushZeroLength.c
RemoveNullEntry_SOURCES     = RemoveNullEntry.c
RemoveNullHandle_SOURCES    = RemoveNullHandle.c
RemovePass_SOURCES          = RemovePass.c
RemoveWrongHandle_SOURCES   = RemoveWrongHandle.c
StatusNullHandle_SOURCES    = StatusNullHandle.c
StatusNullStatus_SOURCES    = StatusNullStatus.c
StatusPass_SOURCES          = StatusPass.c

EXTRA_DIST =                        \
             CloseNullHandle.sh     \
             ClosePass.sh           \
             CloseWrongHandle.sh    \
             FirstNullEntry.sh      \
             FirstNullHandle.sh     \
             FirstPass.sh           \
             FirstWrongHandle.sh    \
             GetFirst.sh            \
             GetNext.sh             \
             NextNoFirst.sh         \
             NextNullEntry.sh       \
             NextNullHandle.sh      \
             NextPass.sh            \
             NextWrongHandle.sh     \
             OpenNullName.sh        \
             OpenNullObjHandle.sh   \
             OpenNullSessHandle.sh  \
             OpenPass.sh            \
             OpenWrongSessHandle.sh \
             OpenZeroLength.sh      \
             Pop.sh                 \
             PushNullData.sh        \
             PushNullHandle.sh      \
             PushPass.sh            \
             PushWrongHandle.sh     \
             PushZeroLength.sh      \
             RemoveNullEntry.sh     \
             RemoveNullHandle.sh    \
             RemovePass.sh          \
             RemoveWrongHandle.sh   \
             StatusNullHandle.sh    \
             StatusNullStatus.sh    \
             StatusPass.sh          

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
        CloseWrongHandle.sh    \
        FirstNullEntry.sh      \
        FirstNullHandle.sh     \
        FirstPass.sh           \
        FirstWrongHandle.sh    \
        GetFirst.sh            \
        GetNext.sh             \
        NextNoFirst.sh         \
        NextNullEntry.sh       \
        NextNullHandle.sh      \
        NextPass.sh            \
        NextWrongHandle.sh     \
        OpenNullName.sh        \
        OpenNullObjHandle.sh   \
        OpenNullSessHandle.sh  \
        OpenPass.sh            \
        OpenWrongSessHandle.sh \
        OpenZeroLength.sh      \
        Pop.sh                 \
        PushNullData.sh        \
        PushNullHandle.sh      \
        PushPass.sh            \
        PushWrongHandle.sh     \
        PushZeroLength.sh      \
        RemoveNullEntry.sh     \
        RemoveNullHandle.sh    \
        RemovePass.sh          \
        RemoveWrongHandle.sh   \
        StatusNullHandle.sh    \
        StatusNullStatus.sh    \
        StatusPass.sh          

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/API/libvdsf.la      \
        $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

