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

# The two "real" regression tests are ListTestSmall and ListTestBig.
# I put them at the end to make sure that the interface itself (to 
# the list) is ok. I also put Small before Big since the smaller one
# is doing a lot less iterations but doing many more consistency checks
# (I could have bundled it all together but the test would take too
# much clock time/cpu time). 

check_PROGRAMS =                   \
        FiniInvSig                 \
        FiniNullList               \
        FiniPass                   \
        GetFirstInvSig             \
        GetFirstNullItem           \
        GetFirstNullList           \
        GetFirstPass               \
        GetLastInvSig              \
        GetLastNullItem            \
        GetLastNullList            \
        GetLastPass                \
        InitNullList               \
        InitPass                   \
        IsValidInvSig              \
        IsValidNullList            \
        IsValidNullUnknown         \
        IsValidPass                \
        PeakFirstInvSig            \
        PeakFirstNullItem          \
        PeakFirstNullList          \
        PeakFirstPass              \
        PeakLastInvSig             \
        PeakLastNullItem           \
        PeakLastNullList           \
        PeakLastPass               \
        PeakNextInvSig             \
        PeakNextNullCurrent        \
        PeakNextNullCurrNextOffset \
        PeakNextNullCurrPrevOffset \
        PeakNextNullList           \
        PeakNextNullNext           \
        PeakNextPass               \
        PeakPreviousInvSig         \
        PeakPreviousNullCurrent    \
        PeakPrevNullCurrNextOffset \
        PeakPrevNullCurrPrevOffset \
        PeakPreviousNullList       \
        PeakPreviousNullPrevious   \
        PeakPreviousPass           \
        PutFirstInvSig             \
        PutFirstNullList           \
        PutFirstNullNewItem        \
        PutFirstNullNewNext        \
        PutFirstNullNewPrevious    \
        PutFirstPass               \
        PutLastInvSig              \
        PutLastNullList            \
        PutLastNullNewItem         \
        PutLastNullNewNext         \
        PutLastNullNewPrevious     \
        PutLastPass                \
        RemoveInvSig               \
        RemoveAbsent               \
        RemoveNullItem             \
        RemoveNullNextOffset       \
        RemoveNullPrevOffset       \
        RemoveNullList             \
        RemovePass                 \
        RemoveWrongItem            \
        ReplaceInvSig              \
        ReplaceNullList            \
        ReplaceNullNew             \
        ReplaceNullNewNext         \
        ReplaceNullNewPrevious     \
        ReplaceNullOld             \
        ReplacePass                \
        ResetInvSig                \
        ResetNullList              \
        ResetPass                  \
        TestSmall                  \
        TestBig                    

LIST_H = ListTestCommon.h 

FiniInvSig_SOURCES                 = $(LIST_H) FiniInvSig.c
FiniNullList_SOURCES               = $(LIST_H) FiniNullList.c
FiniPass_SOURCES                   = $(LIST_H) FiniPass.c
GetFirstInvSig_SOURCES             = $(LIST_H) GetFirstInvSig.c
GetFirstNullItem_SOURCES           = $(LIST_H) GetFirstNullItem.c
GetFirstNullList_SOURCES           = $(LIST_H) GetFirstNullList.c
GetFirstPass_SOURCES               = $(LIST_H) GetFirstPass.c
GetLastInvSig_SOURCES              = $(LIST_H) GetLastInvSig.c
GetLastNullItem_SOURCES            = $(LIST_H) GetLastNullItem.c
GetLastNullList_SOURCES            = $(LIST_H) GetLastNullList.c
GetLastPass_SOURCES                = $(LIST_H) GetLastPass.c
InitNullList_SOURCES               = $(LIST_H) InitNullList.c
InitPass_SOURCES                   = $(LIST_H) InitPass.c
IsValidInvSig_SOURCES              = $(LIST_H) IsValidInvSig.c
IsValidNullList_SOURCES            = $(LIST_H) IsValidNullList.c
IsValidNullUnknown_SOURCES         = $(LIST_H) IsValidNullUnknown.c
IsValidPass_SOURCES                = $(LIST_H) IsValidPass.c
PeakFirstInvSig_SOURCES            = $(LIST_H) PeakFirstInvSig.c
PeakFirstNullItem_SOURCES          = $(LIST_H) PeakFirstNullItem.c
PeakFirstNullList_SOURCES          = $(LIST_H) PeakFirstNullList.c
PeakFirstPass_SOURCES              = $(LIST_H) PeakFirstPass.c
PeakLastInvSig_SOURCES             = $(LIST_H) PeakLastInvSig.c
PeakLastNullItem_SOURCES           = $(LIST_H) PeakLastNullItem.c
PeakLastNullList_SOURCES           = $(LIST_H) PeakLastNullList.c
PeakLastPass_SOURCES               = $(LIST_H) PeakLastPass.c
PeakNextInvSig_SOURCES             = $(LIST_H) PeakNextInvSig.c
PeakNextNullCurrent_SOURCES        = $(LIST_H) PeakNextNullCurrent.c
PeakNextNullCurrNextOffset_SOURCES = $(LIST_H) PeakNextNullCurrNextOffset.c
PeakNextNullCurrPrevOffset_SOURCES = $(LIST_H) PeakNextNullCurrPrevOffset.c
PeakNextNullList_SOURCES           = $(LIST_H) PeakNextNullList.c
PeakNextNullNext_SOURCES           = $(LIST_H) PeakNextNullNext.c
PeakNextPass_SOURCES               = $(LIST_H) PeakNextPass.c
PeakPreviousInvSig_SOURCES         = $(LIST_H) PeakPreviousInvSig.c
PeakPreviousNullCurrent_SOURCES    = $(LIST_H) PeakPreviousNullCurrent.c
PeakPrevNullCurrNextOffset_SOURCES = $(LIST_H) PeakPrevNullCurrNextOffset.c
PeakPrevNullCurrPrevOffset_SOURCES = $(LIST_H) PeakPrevNullCurrPrevOffset.c
PeakPreviousNullList_SOURCES       = $(LIST_H) PeakPreviousNullList.c
PeakPreviousNullPrevious_SOURCES   = $(LIST_H) PeakPreviousNullPrevious.c
PeakPreviousPass_SOURCES           = $(LIST_H) PeakPreviousPass.c
PutFirstInvSig_SOURCES             = $(LIST_H) PutFirstInvSig.c
PutFirstNullList_SOURCES           = $(LIST_H) PutFirstNullList.c
PutFirstNullNewItem_SOURCES        = $(LIST_H) PutFirstNullNewItem.c
PutFirstNullNewNext_SOURCES        = $(LIST_H) PutFirstNullNewNext.c
PutFirstNullNewPrevious_SOURCES    = $(LIST_H) PutFirstNullNewPrevious.c
PutFirstPass_SOURCES               = $(LIST_H) PutFirstPass.c
PutLastInvSig_SOURCES              = $(LIST_H) PutLastInvSig.c
PutLastNullList_SOURCES            = $(LIST_H) PutLastNullList.c
PutLastNullNewItem_SOURCES         = $(LIST_H) PutLastNullNewItem.c
PutLastNullNewNext_SOURCES         = $(LIST_H) PutLastNullNewNext.c
PutLastNullNewPrevious_SOURCES     = $(LIST_H) PutLastNullNewPrevious.c
PutLastPass_SOURCES                = $(LIST_H) PutLastPass.c
RemoveInvSig_SOURCES               = $(LIST_H) RemoveInvSig.c
RemoveNullItem_SOURCES             = $(LIST_H) RemoveNullItem.c
RemoveNullNextOffset_SOURCES       = $(LIST_H) RemoveNullNextOffset.c
RemoveNullPrevOffset_SOURCES       = $(LIST_H) RemoveNullPrevOffset.c
RemoveNullList_SOURCES             = $(LIST_H) RemoveNullList.c
RemoveAbsent_SOURCES               = $(LIST_H) RemoveAbsent.c
RemoveWrongItem_SOURCES            = $(LIST_H) RemoveWrongItem.c
RemovePass_SOURCES                 = $(LIST_H) RemovePass.c
ReplaceInvSig_SOURCES              = $(LIST_H) ReplaceInvSig.c
ReplaceNullList_SOURCES            = $(LIST_H) ReplaceNullList.c
ReplaceNullNew_SOURCES             = $(LIST_H) ReplaceNullNew.c
ReplaceNullNewNext_SOURCES         = $(LIST_H) ReplaceNullNewNext.c
ReplaceNullNewPrevious_SOURCES     = $(LIST_H) ReplaceNullNewPrevious.c
ReplaceNullOld_SOURCES             = $(LIST_H) ReplaceNullOld.c
ReplacePass_SOURCES                = $(LIST_H) ReplacePass.c
ResetInvSig_SOURCES                = $(LIST_H) ResetInvSig.c
ResetNullList_SOURCES              = $(LIST_H) ResetNullList.c
ResetPass_SOURCES                  = $(LIST_H) ResetPass.c
TestSmall_SOURCES                  = $(LIST_H) TestSmall.c
TestBig_SOURCES                    = $(LIST_H) TestBig.c

EXTRA_DIST =           \
        $(LIST_H)      \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                            \
        FiniInvSig                 \
        FiniNullList               \
        FiniPass                   \
        GetFirstInvSig             \
        GetFirstNullItem           \
        GetFirstNullList           \
        GetFirstPass               \
        GetLastInvSig              \
        GetLastNullItem            \
        GetLastNullList            \
        GetLastPass                \
        InitNullList               \
        InitPass                   \
        IsValidInvSig              \
        IsValidNullList            \
        IsValidNullUnknown         \
        IsValidPass                \
        PeakFirstInvSig            \
        PeakFirstNullItem          \
        PeakFirstNullList          \
        PeakFirstPass              \
        PeakLastInvSig             \
        PeakLastNullItem           \
        PeakLastNullList           \
        PeakLastPass               \
        PeakNextInvSig             \
        PeakNextNullCurrent        \
        PeakNextNullCurrNextOffset \
        PeakNextNullCurrPrevOffset \
        PeakNextNullList           \
        PeakNextNullNext           \
        PeakNextPass               \
        PeakPreviousInvSig         \
        PeakPreviousNullCurrent    \
        PeakPrevNullCurrNextOffset \
        PeakPrevNullCurrPrevOffset \
        PeakPreviousNullList       \
        PeakPreviousNullPrevious   \
        PeakPreviousPass           \
        PutFirstInvSig             \
        PutFirstNullList           \
        PutFirstNullNewItem        \
        PutFirstNullNewNext        \
        PutFirstNullNewPrevious    \
        PutFirstPass               \
        PutLastInvSig              \
        PutLastNullList            \
        PutLastNullNewItem         \
        PutLastNullNewNext         \
        PutLastNullNewPrevious     \
        PutLastPass                \
        RemoveInvSig               \
        RemoveNullItem             \
        RemoveNullNextOffset       \
        RemoveNullPrevOffset       \
        RemoveNullList             \
        RemoveAbsent               \
        RemovePass                 \
        RemoveWrongItem            \
        ReplaceInvSig              \
        ReplaceNullList            \
        ReplaceNullNew             \
        ReplaceNullNewNext         \
        ReplaceNullNewPrevious     \
        ReplaceNullOld             \
        ReplacePass                \
        ResetInvSig                \
        ResetNullList              \
        ResetPass                  \
        TestSmall                  \
        TestBig                    

XFAIL_TESTS =                      \
        FiniInvSig                 \
        FiniNullList               \
        GetFirstInvSig             \
        GetFirstNullItem           \
        GetFirstNullList           \
        GetLastInvSig              \
        GetLastNullItem            \
        GetLastNullList            \
        InitNullList               \
        IsValidInvSig              \
        IsValidNullList            \
        IsValidNullUnknown         \
        PeakFirstInvSig            \
        PeakFirstNullItem          \
        PeakFirstNullList          \
        PeakLastInvSig             \
        PeakLastNullItem           \
        PeakLastNullList           \
        PeakNextInvSig             \
        PeakNextNullCurrent        \
        PeakNextNullCurrNextOffset \
        PeakNextNullCurrPrevOffset \
        PeakNextNullList           \
        PeakNextNullNext           \
        PeakPreviousInvSig         \
        PeakPreviousNullCurrent    \
        PeakPrevNullCurrNextOffset \
        PeakPrevNullCurrPrevOffset \
        PeakPreviousNullList       \
        PeakPreviousNullPrevious   \
        PutFirstInvSig             \
        PutFirstNullList           \
        PutFirstNullNewItem        \
        PutFirstNullNewNext        \
        PutFirstNullNewPrevious    \
        PutLastInvSig              \
        PutLastNullList            \
        PutLastNullNewItem         \
        PutLastNullNewNext         \
        PutLastNullNewPrevious     \
        RemoveInvSig               \
        RemoveNullItem             \
        RemoveNullNextOffset       \
        RemoveNullPrevOffset       \
        RemoveNullList             \
        RemoveAbsent               \
        RemoveWrongItem            \
        ReplaceInvSig              \
        ReplaceNullList            \
        ReplaceNullNew             \
        ReplaceNullNewNext         \
        ReplaceNullNewPrevious     \
        ReplaceNullOld             \
        ResetInvSig                \
        ResetNullList 

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

