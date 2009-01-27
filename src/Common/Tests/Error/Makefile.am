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

check_PROGRAMS =              \
        AddHandlerNullDef     \
        AddHandlerNullHandler \
        AddHandlerNullName    \
        AddHandlerPass        \
        AnyErrorNullError     \
        AnyErrorPass          \
        ChainErrorNullDef     \
        ChainErrorNullHandler \
        ChainErrorInvalidSig  \
        ChainErrorNoSetError  \
        ChainErrorTooMany     \
        ChainErrorPass        \
        FiniInvalidSig        \
        FiniNullError         \
        FiniPass              \
        GetErrorInvalidSig    \
        GetErrorNullError     \
        GetErrorNullMsg       \
        GetErrorZeroLength    \
        GetLastNullError      \
        GetLastPass           \
        GetLengthNullError    \
        GetLengthPass         \
        InitNoInitDefs        \
        InitNullError         \
        InitPass              \
        SetErrorInvalidSig    \
        SetErrorInvalidValue  \
        SetErrorNullError     \
        Tests                 

AddHandlerNullDef_SOURCES     = AddHandlerNullDef.c
AddHandlerNullHandler_SOURCES = AddHandlerNullHandler.c
AddHandlerNullName_SOURCES    = AddHandlerNullName.c
AddHandlerPass_SOURCES        = AddHandlerPass.c
AnyErrorNullError_SOURCES     = AnyErrorNullError.c
AnyErrorPass_SOURCES          = AnyErrorPass.c
ChainErrorNullDef_SOURCES     = ChainErrorNullDef.c
ChainErrorNullHandler_SOURCES = ChainErrorNullHandler.c
ChainErrorInvalidSig_SOURCES  = ChainErrorInvalidSig.c
ChainErrorNoSetError_SOURCES  = ChainErrorNoSetError.c
ChainErrorTooMany_SOURCES     = ChainErrorTooMany.c
ChainErrorPass_SOURCES        = ChainErrorPass.c
FiniInvalidSig_SOURCES        = FiniInvalidSig.c
FiniNullError_SOURCES         = FiniNullError.c
FiniPass_SOURCES              = FiniPass.c
GetErrorInvalidSig_SOURCES    = GetErrorInvalidSig.c
GetErrorNullError_SOURCES     = GetErrorNullError.c
GetErrorNullMsg_SOURCES       = GetErrorNullMsg.c
GetErrorZeroLength_SOURCES    = GetErrorZeroLength.c
GetLastNullError_SOURCES      = GetLastNullError.c
GetLastPass_SOURCES           = GetLastPass.c
GetLengthNullError_SOURCES    = GetLengthNullError.c
GetLengthPass_SOURCES         = GetLengthPass.c
InitNoInitDefs_SOURCES        = InitNoInitDefs.c
InitNullError_SOURCES         = InitNullError.c
InitPass_SOURCES              = InitPass.c
SetErrorInvalidSig_SOURCES    = SetErrorInvalidSig.c
SetErrorInvalidValue_SOURCES  = SetErrorInvalidValue.c
SetErrorNullError_SOURCES     = SetErrorNullError.c
Tests_SOURCES                 = Tests.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                       \
        AddHandlerNullDef     \
        AddHandlerNullHandler \
        AddHandlerNullName    \
        AddHandlerPass        \
        AnyErrorNullError     \
        AnyErrorPass          \
        ChainErrorNullDef     \
        ChainErrorNullHandler \
        ChainErrorInvalidSig  \
        ChainErrorNoSetError  \
        ChainErrorTooMany     \
        ChainErrorPass        \
        FiniInvalidSig        \
        FiniNullError         \
        FiniPass              \
        GetErrorInvalidSig    \
        GetErrorNullError     \
        GetErrorNullMsg       \
        GetErrorZeroLength    \
        GetLastNullError      \
        GetLastPass           \
        GetLengthNullError    \
        GetLengthPass         \
        InitNoInitDefs        \
        InitNullError         \
        InitPass              \
        SetErrorInvalidSig    \
        SetErrorInvalidValue  \
        SetErrorNullError     \
        Tests                 

XFAIL_TESTS =                 \
        AddHandlerNullDef     \
        AddHandlerNullHandler \
        AddHandlerNullName    \
        AnyErrorNullError     \
        ChainErrorNullDef     \
        ChainErrorNullHandler \
        ChainErrorInvalidSig  \
        ChainErrorNoSetError  \
        ChainErrorTooMany     \
        FiniInvalidSig        \
        FiniNullError         \
        GetErrorInvalidSig    \
        GetErrorNullError     \
        GetErrorNullMsg       \
        GetErrorZeroLength    \
        GetLastNullError      \
        GetLengthNullError    \
        InitNoInitDefs        \
        InitNullError         \
        SetErrorInvalidSig    \
        SetErrorInvalidValue  \
        SetErrorNullError     

LDADD = $(top_builddir)/src/Common/libCommon.la

