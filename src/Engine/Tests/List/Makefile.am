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

# The two "real" regression tests are ListTestSmall and ListTestBig.
# I put them at the end to make sure that the interface itself (to 
# the list) is ok. I also put Small before Big since the smaller one
# is doing a lot less iterations but doing many more consistency checks
# (I could have bundled it all together but the test would take too
# much clock time/cpu time). 

check_PROGRAMS =                           \
                 FiniInvSig                \
                 FiniPreList               \
                 FiniPass                  \
                 GetFirstInvSig            \
                 GetFirstPreAlloc          \
                 GetFirstPreItem           \
                 GetFirstPreList           \
                 GetFirstPass              \
                 GetLastInvSig             \
                 GetLastPreAlloc           \
                 GetLastPreItem            \
                 GetLastPreList            \
                 GetLastPass               \
                 InitPreAlloc              \
                 InitPreList               \
                 InitPass                  \
                 IsValidInvSig             \
                 IsValidPreAlloc           \
                 IsValidPreList            \
                 IsValidPreUnknown         \
                 IsValidPass               \
                 PeakFirstInvSig           \
                 PeakFirstPreAlloc         \
                 PeakFirstPreItem          \
                 PeakFirstPreList          \
                 PeakFirstPass             \
                 PeakLastInvSig            \
                 PeakLastPreAlloc          \
                 PeakLastPreItem           \
                 PeakLastPreList           \
                 PeakLastPass              \
                 PeakNextInvSig            \
                 PeakNextPreAlloc          \
                 PeakNextPreCurrent        \
                 PeakNextPreCurrNextOffset \
                 PeakNextPreCurrPrevOffset \
                 PeakNextPreList           \
                 PeakNextPreNext           \
                 PeakNextPass              \
                 PeakPreviousInvSig        \
                 PeakPreviousPreAlloc      \
                 PeakPreviousPreCurrent    \
                 PeakPrevPreCurrNextOffset \
                 PeakPrevPreCurrPrevOffset \
                 PeakPreviousPreList       \
                 PeakPreviousPrePrevious   \
                 PeakPreviousPass          \
                 PutFirstInvSig            \
                 PutFirstPreAlloc          \
                 PutFirstPreList           \
                 PutFirstPreNewItem        \
                 PutFirstPass              \
                 PutLastInvSig             \
                 PutLastPreAlloc           \
                 PutLastPreList            \
                 PutLastPreNewItem         \
                 PutLastPass               \
                 RemoveInvSig              \
                 RemovePreNotEmpty         \
                 RemovePreAlloc            \
                 RemovePreItem             \
                 RemovePreItemNextOffset   \
                 RemovePreItemPrevOffset   \
                 RemovePreList             \
                 RemovePass                \
                 RemoveXPreWrongItem       \
                 ResetInvSig               \
                 ResetPreAlloc             \
                 ResetPreList              \
                 ResetPass                 \
                 TestSmall                 \
                 TestBig

LIST_H = ListTestCommon.h

FiniInvSig_SOURCES                = $(LIST_H) FiniInvSig.c
FiniPreList_SOURCES               = $(LIST_H) FiniPreList.c
FiniPass_SOURCES                  = $(LIST_H) FiniPass.c
GetFirstInvSig_SOURCES            = $(LIST_H) GetFirstInvSig.c
GetFirstPreAlloc_SOURCES          = $(LIST_H) GetFirstPreAlloc.c
GetFirstPreItem_SOURCES           = $(LIST_H) GetFirstPreItem.c
GetFirstPreList_SOURCES           = $(LIST_H) GetFirstPreList.c
GetFirstPass_SOURCES              = $(LIST_H) GetFirstPass.c
GetLastInvSig_SOURCES             = $(LIST_H) GetLastInvSig.c
GetLastPreAlloc_SOURCES           = $(LIST_H) GetLastPreAlloc.c
GetLastPreItem_SOURCES            = $(LIST_H) GetLastPreItem.c
GetLastPreList_SOURCES            = $(LIST_H) GetLastPreList.c
GetLastPass_SOURCES               = $(LIST_H) GetLastPass.c
InitPreAlloc_SOURCES              = $(LIST_H) InitPreAlloc.c
InitPreList_SOURCES               = $(LIST_H) InitPreList.c
InitPass_SOURCES                  = $(LIST_H) InitPass.c
IsValidInvSig_SOURCES             = $(LIST_H) IsValidInvSig.c
IsValidPreAlloc_SOURCES           = $(LIST_H) IsValidPreAlloc.c
IsValidPreList_SOURCES            = $(LIST_H) IsValidPreList.c
IsValidPreUnknown_SOURCES         = $(LIST_H) IsValidPreUnknown.c
IsValidPass_SOURCES               = $(LIST_H) IsValidPass.c
PeakFirstInvSig_SOURCES           = $(LIST_H) PeakFirstInvSig.c
PeakFirstPreAlloc_SOURCES         = $(LIST_H) PeakFirstPreAlloc.c
PeakFirstPreItem_SOURCES          = $(LIST_H) PeakFirstPreItem.c
PeakFirstPreList_SOURCES          = $(LIST_H) PeakFirstPreList.c
PeakFirstPass_SOURCES             = $(LIST_H) PeakFirstPass.c
PeakLastInvSig_SOURCES            = $(LIST_H) PeakLastInvSig.c
PeakLastPreAlloc_SOURCES          = $(LIST_H) PeakLastPreAlloc.c
PeakLastPreItem_SOURCES           = $(LIST_H) PeakLastPreItem.c
PeakLastPreList_SOURCES           = $(LIST_H) PeakLastPreList.c
PeakLastPass_SOURCES              = $(LIST_H) PeakLastPass.c
PeakNextInvSig_SOURCES            = $(LIST_H) PeakNextInvSig.c
PeakNextPreAlloc_SOURCES          = $(LIST_H) PeakNextPreAlloc.c
PeakNextPreCurrent_SOURCES        = $(LIST_H) PeakNextPreCurrent.c
PeakNextPreCurrNextOffset_SOURCES = $(LIST_H) PeakNextPreCurrNextOffset.c
PeakNextPreCurrPrevOffset_SOURCES = $(LIST_H) PeakNextPreCurrPrevOffset.c
PeakNextPreList_SOURCES           = $(LIST_H) PeakNextPreList.c
PeakNextPreNext_SOURCES           = $(LIST_H) PeakNextPreNext.c
PeakNextPass_SOURCES              = $(LIST_H) PeakNextPass.c
PeakPreviousInvSig_SOURCES        = $(LIST_H) PeakPreviousInvSig.c
PeakPreviousPreAlloc_SOURCES      = $(LIST_H) PeakPreviousPreAlloc.c
PeakPreviousPreCurrent_SOURCES    = $(LIST_H) PeakPreviousPreCurrent.c
PeakPrevPreCurrNextOffset_SOURCES = $(LIST_H) PeakPrevPreCurrNextOffset.c
PeakPrevPreCurrPrevOffset_SOURCES = $(LIST_H) PeakPrevPreCurrPrevOffset.c
PeakPreviousPreList_SOURCES       = $(LIST_H) PeakPreviousPreList.c
PeakPreviousPrePrevious_SOURCES   = $(LIST_H) PeakPreviousPrePrevious.c
PeakPreviousPass_SOURCES          = $(LIST_H) PeakPreviousPass.c
PutFirstInvSig_SOURCES            = $(LIST_H) PutFirstInvSig.c
PutFirstPreAlloc_SOURCES          = $(LIST_H) PutFirstPreAlloc.c
PutFirstPreList_SOURCES           = $(LIST_H) PutFirstPreList.c
PutFirstPreNewItem_SOURCES        = $(LIST_H) PutFirstPreNewItem.c
PutFirstPass_SOURCES              = $(LIST_H) PutFirstPass.c
PutLastInvSig_SOURCES             = $(LIST_H) PutLastInvSig.c
PutLastPreAlloc_SOURCES           = $(LIST_H) PutLastPreAlloc.c
PutLastPreList_SOURCES            = $(LIST_H) PutLastPreList.c
PutLastPreNewItem_SOURCES         = $(LIST_H) PutLastPreNewItem.c
PutLastPass_SOURCES               = $(LIST_H) PutLastPass.c
RemoveInvSig_SOURCES              = $(LIST_H) RemoveInvSig.c
RemovePreAlloc_SOURCES            = $(LIST_H) RemovePreAlloc.c
RemovePreItem_SOURCES             = $(LIST_H) RemovePreItem.c
RemovePreItemNextOffset_SOURCES   = $(LIST_H) RemovePreItemNextOffset.c
RemovePreItemPrevOffset_SOURCES   = $(LIST_H) RemovePreItemPrevOffset.c
RemovePreList_SOURCES             = $(LIST_H) RemovePreList.c
RemovePreNotEmpty_SOURCES         = $(LIST_H) RemovePreNotEmpty.c
RemoveXPreWrongItem_SOURCES       = $(LIST_H) RemoveXPreWrongItem.c
RemovePass_SOURCES                = $(LIST_H) RemovePass.c
ResetInvSig_SOURCES               = $(LIST_H) ResetInvSig.c
ResetPreAlloc_SOURCES             = $(LIST_H) ResetPreAlloc.c
ResetPreList_SOURCES              = $(LIST_H) ResetPreList.c
ResetPass_SOURCES                 = $(LIST_H) ResetPass.c
TestSmall_SOURCES                 = $(LIST_H) TestSmall.c
TestBig_SOURCES                   = $(LIST_H) TestBig.c

EXTRA_DIST = $(LIST_H)

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS = 

if COND_USE_DBC
TESTS +=                           \
         FiniInvSig                \
         FiniPreList               \
         FiniPass                  \
         GetFirstInvSig            \
         GetFirstPreAlloc          \
         GetFirstPreItem           \
         GetFirstPreList           \
         GetFirstPass              \
         GetLastInvSig             \
         GetLastPreAlloc           \
         GetLastPreItem            \
         GetLastPreList            \
         GetLastPass               \
         InitPreAlloc              \
         InitPreList               \
         InitPass                  \
         IsValidInvSig             \
         IsValidPreAlloc           \
         IsValidPreList            \
         IsValidPreUnknown         \
         IsValidPass               \
         PeakFirstInvSig           \
         PeakFirstPreAlloc         \
         PeakFirstPreItem          \
         PeakFirstPreList          \
         PeakFirstPass             \
         PeakLastInvSig            \
         PeakLastPreAlloc          \
         PeakLastPreItem           \
         PeakLastPreList           \
         PeakLastPass              \
         PeakNextInvSig            \
         PeakNextPreAlloc          \
         PeakNextPreCurrent        \
         PeakNextPreCurrNextOffset \
         PeakNextPreCurrPrevOffset \
         PeakNextPreList           \
         PeakNextPreNext           \
         PeakNextPass              \
         PeakPreviousInvSig        \
         PeakPreviousPreAlloc      \
         PeakPreviousPreCurrent    \
         PeakPrevPreCurrNextOffset \
         PeakPrevPreCurrPrevOffset \
         PeakPreviousPreList       \
         PeakPreviousPrePrevious   \
         PeakPreviousPass          \
         PutFirstInvSig            \
         PutFirstPreAlloc          \
         PutFirstPreList           \
         PutFirstPreNewItem        \
         PutFirstPass              \
         PutLastInvSig             \
         PutLastPreAlloc           \
         PutLastPreList            \
         PutLastPreNewItem         \
         PutLastPass               \
         RemoveInvSig              \
         RemovePreAlloc            \
         RemovePreItem             \
         RemovePreItemNextOffset   \
         RemovePreItemPrevOffset   \
         RemovePreList             \
         RemovePreNotEmpty         \
         RemovePass                \
         ResetInvSig               \
         ResetPreAlloc             \
         ResetPreList              \
         ResetPass 
endif

if COND_USE_EXTREME_DBC
TESTS +=                     \
         RemoveXPreWrongItem
endif

TESTS +=           \
         TestSmall \
         TestBig

XFAIL_TESTS =                           \
              FiniInvSig                \
              FiniPreList               \
              GetFirstInvSig            \
              GetFirstPreAlloc          \
              GetFirstPreItem           \
              GetFirstPreList           \
              GetLastInvSig             \
              GetLastPreAlloc           \
              GetLastPreItem            \
              GetLastPreList            \
              InitPreAlloc              \
              InitPreList               \
              IsValidInvSig             \
              IsValidPreAlloc           \
              IsValidPreList            \
              IsValidPreUnknown         \
              PeakFirstInvSig           \
              PeakFirstPreAlloc         \
              PeakFirstPreItem          \
              PeakFirstPreList          \
              PeakLastInvSig            \
              PeakLastPreAlloc          \
              PeakLastPreItem           \
              PeakLastPreList           \
              PeakNextInvSig            \
              PeakNextPreAlloc          \
              PeakNextPreCurrent        \
              PeakNextPreCurrNextOffset \
              PeakNextPreCurrPrevOffset \
              PeakNextPreList           \
              PeakNextPreNext           \
              PeakPreviousInvSig        \
              PeakPreviousPreAlloc      \
              PeakPreviousPreCurrent    \
              PeakPrevPreCurrNextOffset \
              PeakPrevPreCurrPrevOffset \
              PeakPreviousPreList       \
              PeakPreviousPrePrevious   \
              PutFirstInvSig            \
              PutFirstPreAlloc          \
              PutFirstPreList           \
              PutFirstPreNewItem        \
              PutLastInvSig             \
              PutLastPreAlloc           \
              PutLastPreList            \
              PutLastPreNewItem         \
              RemoveInvSig              \
              RemovePreAlloc            \
              RemovePreItem             \
              RemovePreItemNextOffset   \
              RemovePreItemPrevOffset   \
              RemovePreList             \
              RemovePreNotEmpty         \
              RemoveXPreWrongItem       \
              ResetInvSig               \
              ResetPreAlloc             \
              ResetPreList 

LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

AM_CPPFLAGS = -I$(top_srcdir)/src/Engine \
              -I$(top_srcdir)/src/Common \
              -I$(top_srcdir)/src/include

if COND_USE_DBC
   AM_CPPFLAGS += -DUSE_DBC
endif
if COND_USE_EXTREME_DBC
   AM_CPPFLAGS += -DUSE_EXTREME_DBC
endif

