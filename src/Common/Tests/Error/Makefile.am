# Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~ core

check_PROGRAMS =                       \
                 AddHandlerNullDef     \
                 AddHandlerNullHandler \
                 AddHandlerNullName    \
                 AddHandlerPass        \
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
InitNoInitDefs_SOURCES        = InitNoInitDefs.c
InitNullError_SOURCES         = InitNullError.c
InitPass_SOURCES              = InitPass.c
SetErrorInvalidSig_SOURCES    = SetErrorInvalidSig.c
SetErrorInvalidValue_SOURCES  = SetErrorInvalidValue.c
SetErrorNullError_SOURCES     = SetErrorNullError.c
Tests_SOURCES                 = Tests.c

EXTRA_DIST = 

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS = 

if COND_USE_DBC
TESTS +=                       \
         AddHandlerNullDef     \
         AddHandlerNullHandler \
         AddHandlerNullName    \
         AddHandlerPass        \
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
              InitNoInitDefs        \
              InitNullError         \
              SetErrorInvalidSig    \
              SetErrorInvalidValue  \
              SetErrorNullError 

LDADD = $(top_builddir)/src/Common/libCommon.la

AM_CPPFLAGS = -I$(top_srcdir)/src/Common \
              -I$(top_srcdir)/src/include

if COND_USE_DBC
   AM_CPPFLAGS += -DUSE_DBC
endif
if COND_USE_EXTREME_DBC
   AM_CPPFLAGS += -DUSE_EXTREME_DBC
endif


