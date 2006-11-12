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

GetLongOptNullHandle_SOURCES       = GetLongOptNullHandle.c
GetLongOptNullOpt_SOURCES          = GetLongOptNullOpt.c
GetLongOptNullValue_SOURCES        = GetLongOptNullValue.c
GetLongOptPass_SOURCES             = GetLongOptPass.c
GetShortOptNullHandle_SOURCES      = GetShortOptNullHandle.c
GetShortOptNullValue_SOURCES       = GetShortOptNullValue.c
GetShortOptPass_SOURCES            = GetShortOptPass.c
IsLongPresentNullHandle_SOURCES    = IsLongPresentNullHandle.c
IsLongPresentNullOpt_SOURCES       = IsLongPresentNullOpt.c
IsLongPresentPass_SOURCES          = IsLongPresentPass.c
IsShortPresentNullHandle_SOURCES   = IsShortPresentNullHandle.c
IsShortPresentPass_SOURCES         = IsShortPresentPass.c
SetOptionsNullHandle_SOURCES       = SetOptionsNullHandle.c
SetOptionsNullOpts_SOURCES         = SetOptionsNullOpts.c
SetOptionsNullEndedOpt_SOURCES     = SetOptionsNullEndedOpt.c
SetOptionsNullEndedComment_SOURCES = SetOptionsNullEndedComment.c
SetOptionsNullEndedValue_SOURCES   = SetOptionsNullEndedValue.c
SetOptionsNumOptsZero_SOURCES      = SetOptionsNumOptsZero.c
SetOptionsPass_SOURCES             = SetOptionsPass.c
SetOptionsRepeatedLong_SOURCES     = SetOptionsRepeatedLong.c
SetOptionsRepeatedShort_SOURCES    = SetOptionsRepeatedShort.c
ShowUsageNullAddArgs_SOURCES       = ShowUsageNullAddArgs.c
ShowUsageNullHandle_SOURCES        = ShowUsageNullHandle.c
ShowUsageNullProgName_SOURCES      = ShowUsageNullProgName.c
ShowUsagePass_SOURCES              = ShowUsagePass.c
Tests_SOURCES                      = Tests.c
UnsetOptNullHandle_SOURCES         = UnsetOptNullHandle.c
ValidateArgcZero_SOURCES           = ValidateArgcZero.c
ValidateNullArgv_SOURCES           = ValidateNullArgv.c
ValidateNULLArgvi_SOURCES          = ValidateNULLArgvi.c
ValidateNullHandle_SOURCES         = ValidateNullHandle.c
ValidatePass_SOURCES               = ValidatePass.c

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

AM_CPPFLAGS = -I$(top_srcdir)/src/Common       \
              -I$(top_srcdir)/src/Common/Tests \
              -I$(top_srcdir)/src/include

if COND_USE_DBC
   AM_CPPFLAGS += -DUSE_DBC
endif
if COND_USE_EXTREME_DBC
   AM_CPPFLAGS += -DUSE_EXTREME_DBC
endif


