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

INCLUDE_FILES =

check_PROGRAMS =                       \
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

AddHandlerNullDef_SOURCES     = $(INCLUDE_FILES) AddHandlerNullDef.c
AddHandlerNullHandler_SOURCES = $(INCLUDE_FILES) AddHandlerNullHandler.c
AddHandlerNullName_SOURCES    = $(INCLUDE_FILES) AddHandlerNullName.c
AddHandlerPass_SOURCES        = $(INCLUDE_FILES) AddHandlerPass.c
AnyErrorNullError_SOURCES     = $(INCLUDE_FILES) AnyErrorNullError.c
AnyErrorPass_SOURCES          = $(INCLUDE_FILES) AnyErrorPass.c
ChainErrorNullDef_SOURCES     = $(INCLUDE_FILES) ChainErrorNullDef.c
ChainErrorNullHandler_SOURCES = $(INCLUDE_FILES) ChainErrorNullHandler.c
ChainErrorInvalidSig_SOURCES  = $(INCLUDE_FILES) ChainErrorInvalidSig.c
ChainErrorNoSetError_SOURCES  = $(INCLUDE_FILES) ChainErrorNoSetError.c
ChainErrorTooMany_SOURCES     = $(INCLUDE_FILES) ChainErrorTooMany.c
ChainErrorPass_SOURCES        = $(INCLUDE_FILES) ChainErrorPass.c
FiniInvalidSig_SOURCES        = $(INCLUDE_FILES) FiniInvalidSig.c
FiniNullError_SOURCES         = $(INCLUDE_FILES) FiniNullError.c
FiniPass_SOURCES              = $(INCLUDE_FILES) FiniPass.c
GetErrorInvalidSig_SOURCES    = $(INCLUDE_FILES) GetErrorInvalidSig.c
GetErrorNullError_SOURCES     = $(INCLUDE_FILES) GetErrorNullError.c
GetErrorNullMsg_SOURCES       = $(INCLUDE_FILES) GetErrorNullMsg.c
GetErrorZeroLength_SOURCES    = $(INCLUDE_FILES) GetErrorZeroLength.c
GetLastNullError_SOURCES      = $(INCLUDE_FILES) GetLastNullError.c
GetLastPass_SOURCES           = $(INCLUDE_FILES) GetLastPass.c
GetLengthNullError_SOURCES    = $(INCLUDE_FILES) GetLengthNullError.c
GetLengthPass_SOURCES         = $(INCLUDE_FILES) GetLengthPass.c
InitNoInitDefs_SOURCES        = $(INCLUDE_FILES) InitNoInitDefs.c
InitNullError_SOURCES         = $(INCLUDE_FILES) InitNullError.c
InitPass_SOURCES              = $(INCLUDE_FILES) InitPass.c
SetErrorInvalidSig_SOURCES    = $(INCLUDE_FILES) SetErrorInvalidSig.c
SetErrorInvalidValue_SOURCES  = $(INCLUDE_FILES) SetErrorInvalidValue.c
SetErrorNullError_SOURCES     = $(INCLUDE_FILES) SetErrorNullError.c
Tests_SOURCES                 = $(INCLUDE_FILES) Tests.c

EXTRA_DIST = 

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS = 

if COND_USE_DBC
TESTS +=                       \
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
         SetErrorNullError 
endif

if COND_USE_EXTREME_DBC
TESTS += 
endif

TESTS +=       \
         Tests  

XFAIL_TESTS =                       \
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

