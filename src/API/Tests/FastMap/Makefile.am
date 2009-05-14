# Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

check_PROGRAMS =            \
        Close               \
        Create              \
        Delete              \
        Definition          \
        Edit                \
        Empty               \
        FirstNullData       \
        FirstNullDataLength \
        FirstNullHandle     \
        FirstNullKey        \
        FirstNullKeyLength  \
        FirstPass           \
        FirstWrongHandle    \
        Get                 \
        GetFirst            \
        GetNext             \
        Insert              \
        NextNoFirst         \
        NextNullData        \
        NextNullDataLength  \
        NextNullHandle      \
        NextNullKey         \
        NextNullKeyLength   \
        NextPass            \
        NextWrongHandle     \
        OpenNoSession       \
        OpenPass            \
        RecordDef           \
        Replace             \
        ResetNullMap        \
        ResetPass           \
        ResetWrongType      \
        RetrieveNullData    \
        RetrieveNullHandle  \
        RetrieveNullKey     \
        RetrieveNullLength  \
        RetrievePass        \
        RetrieveWrongHandle \
        RetrieveZeroLength  \
        Status              \
        Tests               

Close_SOURCES               = Close.c
Create_SOURCES              = Create.c
Delete_SOURCES              = Delete.c
Definition_SOURCES          = Definition.c
Edit_SOURCES                = Edit.c
Empty_SOURCES               = Empty.c
FirstNullData_SOURCES       = FirstNullData.c
FirstNullDataLength_SOURCES = FirstNullDataLength.c
FirstNullHandle_SOURCES     = FirstNullHandle.c
FirstNullKey_SOURCES        = FirstNullKey.c
FirstNullKeyLength_SOURCES  = FirstNullKeyLength.c
FirstPass_SOURCES           = FirstPass.c
FirstWrongHandle_SOURCES    = FirstWrongHandle.c
Get_SOURCES                 = Get.c
GetFirst_SOURCES            = GetFirst.c
GetNext_SOURCES             = GetNext.c
Insert_SOURCES              = Insert.c
NextNoFirst_SOURCES         = NextNoFirst.c
NextNullData_SOURCES        = NextNullData.c
NextNullDataLength_SOURCES  = NextNullDataLength.c
NextNullHandle_SOURCES      = NextNullHandle.c
NextNullKey_SOURCES         = NextNullKey.c
NextNullKeyLength_SOURCES   = NextNullKeyLength.c
NextPass_SOURCES            = NextPass.c
NextWrongHandle_SOURCES     = NextWrongHandle.c
OpenNoSession_SOURCES       = OpenNoSession.c
OpenPass_SOURCES            = OpenPass.c
RecordDef_SOURCES           = RecordDef.c
Replace_SOURCES             = Replace.c
ResetPass_SOURCES           = ResetPass.c
RetrieveNullData_SOURCES    = RetrieveNullData.c
RetrieveNullHandle_SOURCES  = RetrieveNullHandle.c
RetrieveNullKey_SOURCES     = RetrieveNullKey.c
RetrieveNullLength_SOURCES  = RetrieveNullLength.c
RetrievePass_SOURCES        = RetrievePass.c
RetrieveWrongHandle_SOURCES = RetrieveWrongHandle.c
RetrieveZeroLength_SOURCES  = RetrieveZeroLength.c
Status_SOURCES              = Status.c
Tests_SOURCES               = Tests.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.sh    \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        tcp_port=10701               \
        top_srcdir=$(top_srcdir)     \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)

TESTS = RunTests.sh

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/API/libphoton.la        \
        $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la   

