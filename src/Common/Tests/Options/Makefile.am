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

INCLUDE_FILES = PrintError.h

check_PROGRAMS =                            \
                 GetLongOptNullHandle       \
                 GetLongOptNullOpt          \
                 GetLongOptNullValue        \
                 GetLongOptPass             \
                 GetShortOptNullHandle      \
                 GetShortOptNullValue       \
                 GetShortOptPass            \
                 IsLongPresentNullHandle    \
                 IsLongPresentNullOpt       \
                 IsLongPresentPass          \
                 IsShortPresentNullHandle   \
                 IsShortPresentPass         \
                 SetOptionsNullHandle       \
                 SetOptionsNullOpts         \
                 SetOptionsNullEndedOpt     \
                 SetOptionsNullEndedComment \
                 SetOptionsNullEndedValue   \
                 SetOptionsNumOptsZero      \
                 SetOptionsPass             \
                 SetOptionsRepeatedLong     \
                 SetOptionsRepeatedShort    \
                 ShowUsageNullAddArgs       \
                 ShowUsageNullHandle        \
                 ShowUsageNullProgName      \
                 ShowUsagePass              \
                 Tests                      \
                 UnsetOptNullHandle         \
                 ValidateArgcZero           \
                 ValidateNullArgv           \
                 ValidateNULLArgvi          \
                 ValidateNullHandle         \
                 ValidatePass               

GetLongOptNullHandle_SOURCES       = $(INCLUDE_FILES) GetLongOptNullHandle.c
GetLongOptNullOpt_SOURCES          = $(INCLUDE_FILES) GetLongOptNullOpt.c
GetLongOptNullValue_SOURCES        = $(INCLUDE_FILES) GetLongOptNullValue.c
GetLongOptPass_SOURCES             = $(INCLUDE_FILES) GetLongOptPass.c
GetShortOptNullHandle_SOURCES      = $(INCLUDE_FILES) GetShortOptNullHandle.c
GetShortOptNullValue_SOURCES       = $(INCLUDE_FILES) GetShortOptNullValue.c
GetShortOptPass_SOURCES            = $(INCLUDE_FILES) GetShortOptPass.c
IsLongPresentNullHandle_SOURCES    = $(INCLUDE_FILES) IsLongPresentNullHandle.c
IsLongPresentNullOpt_SOURCES       = $(INCLUDE_FILES) IsLongPresentNullOpt.c
IsLongPresentPass_SOURCES          = $(INCLUDE_FILES) IsLongPresentPass.c
IsShortPresentNullHandle_SOURCES   = $(INCLUDE_FILES) IsShortPresentNullHandle.c
IsShortPresentPass_SOURCES         = $(INCLUDE_FILES) IsShortPresentPass.c
SetOptionsNullHandle_SOURCES       = $(INCLUDE_FILES) SetOptionsNullHandle.c
SetOptionsNullOpts_SOURCES         = $(INCLUDE_FILES) SetOptionsNullOpts.c
SetOptionsNullEndedOpt_SOURCES     = $(INCLUDE_FILES) SetOptionsNullEndedOpt.c
SetOptionsNullEndedComment_SOURCES = $(INCLUDE_FILES) SetOptionsNullEndedComment.c
SetOptionsNullEndedValue_SOURCES   = $(INCLUDE_FILES) SetOptionsNullEndedValue.c
SetOptionsNumOptsZero_SOURCES      = $(INCLUDE_FILES) SetOptionsNumOptsZero.c
SetOptionsPass_SOURCES             = $(INCLUDE_FILES) SetOptionsPass.c
SetOptionsRepeatedLong_SOURCES     = $(INCLUDE_FILES) SetOptionsRepeatedLong.c
SetOptionsRepeatedShort_SOURCES    = $(INCLUDE_FILES) SetOptionsRepeatedShort.c
ShowUsageNullAddArgs_SOURCES       = $(INCLUDE_FILES) ShowUsageNullAddArgs.c
ShowUsageNullHandle_SOURCES        = $(INCLUDE_FILES) ShowUsageNullHandle.c
ShowUsageNullProgName_SOURCES      = $(INCLUDE_FILES) ShowUsageNullProgName.c
ShowUsagePass_SOURCES              = $(INCLUDE_FILES) ShowUsagePass.c
Tests_SOURCES                      = $(INCLUDE_FILES) Tests.c
UnsetOptNullHandle_SOURCES         = $(INCLUDE_FILES) UnsetOptNullHandle.c
ValidateArgcZero_SOURCES           = $(INCLUDE_FILES) ValidateArgcZero.c
ValidateNullArgv_SOURCES           = $(INCLUDE_FILES) ValidateNullArgv.c
ValidateNULLArgvi_SOURCES          = $(INCLUDE_FILES) ValidateNULLArgvi.c
ValidateNullHandle_SOURCES         = $(INCLUDE_FILES) ValidateNullHandle.c
ValidatePass_SOURCES               = $(INCLUDE_FILES) ValidatePass.c

EXTRA_DIST = 

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)


TESTS = 

if COND_USE_DBC
TESTS +=                            \
         GetLongOptNullHandle       \
         GetLongOptNullOpt          \
         GetLongOptNullValue        \
         GetLongOptPass             \
         GetShortOptNullHandle      \
         GetShortOptNullValue       \
         GetShortOptPass            \
         IsLongPresentNullHandle    \
         IsLongPresentNullOpt       \
         IsLongPresentPass          \
         IsShortPresentNullHandle   \
         IsShortPresentPass         \
         SetOptionsNullHandle       \
         SetOptionsNullOpts         \
         SetOptionsNullEndedOpt     \
         SetOptionsNullEndedComment \
         SetOptionsNullEndedValue   \
         SetOptionsNumOptsZero      \
         SetOptionsPass             \
         SetOptionsRepeatedLong     \
         SetOptionsRepeatedShort    \
         ShowUsageNullAddArgs       \
         ShowUsageNullHandle        \
         ShowUsageNullProgName      \
         ShowUsagePass              \
         ValidateArgcZero           \
         ValidateNullArgv           \
         ValidateNULLArgvi          \
         ValidateNullHandle         \
         ValidatePass
endif

if COND_USE_EXTREME_DBC
TESTS += 
endif

TESTS += Tests  

XFAIL_TESTS =                       \
         GetLongOptNullHandle       \
         GetLongOptNullOpt          \
         GetLongOptNullValue        \
         GetShortOptNullHandle      \
         GetShortOptNullValue       \
         IsLongPresentNullHandle    \
         IsLongPresentNullOpt       \
         IsShortPresentNullHandle   \
         SetOptionsNullHandle       \
         SetOptionsNullOpts         \
         SetOptionsNullEndedOpt     \
         SetOptionsNullEndedComment \
         SetOptionsNullEndedValue   \
         SetOptionsNumOptsZero      \
         SetOptionsRepeatedLong     \
         SetOptionsRepeatedShort    \
         ShowUsageNullAddArgs       \
         ShowUsageNullHandle        \
         ShowUsageNullProgName      \
         ValidateArgcZero           \
         ValidateNullArgv           \
         ValidateNULLArgvi          \
         ValidateNullHandle         

LDADD = $(top_builddir)/src/Common/libCommon.la

