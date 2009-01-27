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

CLEANFILES = *~ core MemFile.mem

INCLUDE_FILES =

check_PROGRAMS =                \
        BackStatusInvalidSig    \
        BackStatusMemNull       \
        BackStatusNameEmpty     \
        BackStatusPass          \
        BackStatusStatusNull    \
        CloseNullError          \
        CloseInvalidSig         \
        CloseMemNull            \
        ClosePass               \
        CopyPass                \
        CreateBackNullError     \
        CreateBackInvalidSig    \
        CreateBackInvLength     \
        CreateBackMemNull       \
        CreateBackNameEmpty     \
        CreateBackPass          \
        CreateBackPerm0000      \
        CreateBackPerm0200      \
        CreateBackPerm0400      \
        CreateBackPerm0500      \
        FiniInvalidSig          \
        FiniNullMem             \
        FiniPass                \
        InitLengthPositive      \
        InitMemNull             \
        InitNameEmpty           \
        InitNameNull            \
        InitPass                \
        OpenAddrNull            \
        OpenNullError           \
        OpenInvalidLength       \
        OpenInvalidSig          \
        OpenMemNull             \
        OpenNameEmpty           \
        OpenPass                \
        ReadOnlyNullError       \
        ReadOnlyInvalidBaseAddr \
        ReadOnlyInvalidSig      \
        ReadOnlyMemNull         \
        ReadOnlyPass            \
        ReadOnlyTryWrite        \
        ReadWriteNullError      \
        ReadWriteInvalidSig     \
        ReadWriteInvBaseAddr    \
        ReadWriteMemNull        \
        ReadWritePass           \
        SynchNullError          \
        SynchInvalidBaseAddr    \
        SynchInvalidSig         \
        SynchMemNull            \
        SynchPass               \
        Tests                   


BackStatusInvalidSig_SOURCES    = $(INCLUDE_FILES) BackStatusInvalidSig.c
BackStatusMemNull_SOURCES       = $(INCLUDE_FILES) BackStatusMemNull.c
BackStatusNameEmpty_SOURCES     = $(INCLUDE_FILES) BackStatusNameEmpty.c
BackStatusPass_SOURCES          = $(INCLUDE_FILES) BackStatusPass.c
BackStatusStatusNull_SOURCES    = $(INCLUDE_FILES) BackStatusStatusNull.c
CloseNullError_SOURCES          = $(INCLUDE_FILES) CloseNullError.c
CloseInvalidSig_SOURCES         = $(INCLUDE_FILES) CloseInvalidSig.c
CloseMemNull_SOURCES            = $(INCLUDE_FILES) CloseMemNull.c
ClosePass_SOURCES               = $(INCLUDE_FILES) ClosePass.c
CopyPass_SOURCES                = $(INCLUDE_FILES) CopyPass.c
CreateBackNullError_SOURCES     = $(INCLUDE_FILES) CreateBackNullError.c
CreateBackInvalidSig_SOURCES    = $(INCLUDE_FILES) CreateBackInvalidSig.c
CreateBackInvLength_SOURCES     = $(INCLUDE_FILES) CreateBackInvLength.c
CreateBackMemNull_SOURCES       = $(INCLUDE_FILES) CreateBackMemNull.c
CreateBackNameEmpty_SOURCES     = $(INCLUDE_FILES) CreateBackNameEmpty.c
CreateBackPass_SOURCES          = $(INCLUDE_FILES) CreateBackPass.c
CreateBackPerm0000_SOURCES      = $(INCLUDE_FILES) CreateBackPerm0000.c
CreateBackPerm0200_SOURCES      = $(INCLUDE_FILES) CreateBackPerm0200.c
CreateBackPerm0400_SOURCES      = $(INCLUDE_FILES) CreateBackPerm0400.c
CreateBackPerm0500_SOURCES      = $(INCLUDE_FILES) CreateBackPerm0500.c
FiniInvalidSig_SOURCES          = $(INCLUDE_FILES) FiniInvalidSig.c
FiniNullMem_SOURCES             = $(INCLUDE_FILES) FiniNullMem.c
FiniPass_SOURCES                = $(INCLUDE_FILES) FiniPass.c
InitLengthPositive_SOURCES      = $(INCLUDE_FILES) InitLengthPositive.c
InitMemNull_SOURCES             = $(INCLUDE_FILES) InitMemNull.c
InitNameEmpty_SOURCES           = $(INCLUDE_FILES) InitNameEmpty.c
InitNameNull_SOURCES            = $(INCLUDE_FILES) InitNameNull.c
InitPass_SOURCES                = $(INCLUDE_FILES) InitPass.c
OpenAddrNull_SOURCES            = $(INCLUDE_FILES) OpenAddrNull.c
OpenNullError_SOURCES           = $(INCLUDE_FILES) OpenNullError.c
OpenInvalidLength_SOURCES       = $(INCLUDE_FILES) OpenInvalidLength.c
OpenInvalidSig_SOURCES          = $(INCLUDE_FILES) OpenInvalidSig.c
OpenMemNull_SOURCES             = $(INCLUDE_FILES) OpenMemNull.c
OpenNameEmpty_SOURCES           = $(INCLUDE_FILES) OpenNameEmpty.c
OpenPass_SOURCES                = $(INCLUDE_FILES) OpenPass.c
ReadOnlyNullError_SOURCES       = $(INCLUDE_FILES) ReadOnlyNullError.c
ReadOnlyInvalidBaseAddr_SOURCES = $(INCLUDE_FILES) ReadOnlyInvalidBaseAddr.c
ReadOnlyInvalidSig_SOURCES      = $(INCLUDE_FILES) ReadOnlyInvalidSig.c
ReadOnlyMemNull_SOURCES         = $(INCLUDE_FILES) ReadOnlyMemNull.c
ReadOnlyPass_SOURCES            = $(INCLUDE_FILES) ReadOnlyPass.c
ReadOnlyTryWrite_SOURCES        = $(INCLUDE_FILES) ReadOnlyTryWrite.c
ReadWriteNullError_SOURCES      = $(INCLUDE_FILES) ReadWriteNullError.c
ReadWriteInvalidSig_SOURCES     = $(INCLUDE_FILES) ReadWriteInvalidSig.c
ReadWriteInvBaseAddr_SOURCES    = $(INCLUDE_FILES) ReadWriteInvBaseAddr.c
ReadWriteMemNull_SOURCES        = $(INCLUDE_FILES) ReadWriteMemNull.c
ReadWritePass_SOURCES           = $(INCLUDE_FILES) ReadWritePass.c
SynchNullError_SOURCES          = $(INCLUDE_FILES) SynchNullError.c
SynchInvalidBaseAddr_SOURCES    = $(INCLUDE_FILES) SynchInvalidBaseAddr.c
SynchInvalidSig_SOURCES         = $(INCLUDE_FILES) SynchInvalidSig.c
SynchMemNull_SOURCES            = $(INCLUDE_FILES) SynchMemNull.c
SynchPass_SOURCES               = $(INCLUDE_FILES) SynchPass.c
Tests_SOURCES                   = $(INCLUDE_FILES) Tests.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=0                    \
        PYTHON=$(PYTHON)             

TESTS =                         \
        BackStatusInvalidSig    \
        BackStatusMemNull       \
        BackStatusNameEmpty     \
        BackStatusPass          \
        BackStatusStatusNull    \
        CloseNullError          \
        CloseInvalidSig         \
        CloseMemNull            \
        ClosePass               \
        CopyPass                \
        CreateBackNullError     \
        CreateBackInvalidSig    \
        CreateBackInvLength     \
        CreateBackMemNull       \
        CreateBackNameEmpty     \
        CreateBackPass          \
        CreateBackPerm0000      \
        CreateBackPerm0200      \
        CreateBackPerm0400      \
        CreateBackPerm0500      \
        FiniInvalidSig          \
        FiniNullMem             \
        FiniPass                \
        InitLengthPositive      \
        InitMemNull             \
        InitNameEmpty           \
        InitNameNull            \
        InitPass                \
        OpenAddrNull            \
        OpenNullError           \
        OpenInvalidLength       \
        OpenInvalidSig          \
        OpenMemNull             \
        OpenNameEmpty           \
        OpenPass                \
        ReadOnlyNullError       \
        ReadOnlyInvalidBaseAddr \
        ReadOnlyInvalidSig      \
        ReadOnlyMemNull         \
        ReadOnlyPass            \
        ReadOnlyTryWrite        \
        ReadWriteNullError      \
        ReadWriteInvalidSig     \
        ReadWriteInvBaseAddr    \
        ReadWriteMemNull        \
        ReadWritePass           \
        SynchNullError          \
        SynchInvalidBaseAddr    \
        SynchInvalidSig         \
        SynchMemNull            \
        SynchPass               \
        Tests                   

XFAIL_TESTS =                   \
        BackStatusInvalidSig    \
        BackStatusMemNull       \
        BackStatusNameEmpty     \
        BackStatusStatusNull    \
        CloseNullError          \
        CloseInvalidSig         \
        CloseMemNull            \
        CreateBackNullError     \
        CreateBackInvalidSig    \
        CreateBackInvLength     \
        CreateBackMemNull       \
        CreateBackNameEmpty     \
        CreateBackPerm0000      \
        CreateBackPerm0200      \
        CreateBackPerm0400      \
        CreateBackPerm0500      \
        FiniInvalidSig          \
        FiniNullMem             \
        InitLengthPositive      \
        InitMemNull             \
        InitNameEmpty           \
        InitNameNull            \
        OpenAddrNull            \
        OpenNullError           \
        OpenInvalidLength       \
        OpenInvalidSig          \
        OpenMemNull             \
        OpenNameEmpty           \
        ReadOnlyNullError       \
        ReadOnlyInvalidBaseAddr \
        ReadOnlyInvalidSig      \
        ReadOnlyMemNull         \
        ReadWriteNullError      \
        ReadWriteInvalidSig     \
        ReadWriteInvBaseAddr    \
        ReadWriteMemNull        \
        SynchNullError          \
        SynchInvalidBaseAddr    \
        SynchInvalidSig         \
        SynchMemNull            

LDADD = $(top_builddir)/src/Common/libCommon.la

