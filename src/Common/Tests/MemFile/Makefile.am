# Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~ core MemFile.mem

check_PROGRAMS =                         \
                 BackStatusInvalidSig    \
                 BackStatusMemNull       \
                 BackStatusNameEmpty     \
                 BackStatusPass          \
                 BackStatusStatusNull    \
                 CloseNullError          \
                 CloseInvalidSig         \
                 CloseMemNull            \
                 ClosePass               \
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


BackStatusInvalidSig_SOURCES    = BackStatusInvalidSig.c
BackStatusMemNull_SOURCES       = BackStatusMemNull.c
BackStatusNameEmpty_SOURCES     = BackStatusNameEmpty.c
BackStatusPass_SOURCES          = BackStatusPass.c
BackStatusStatusNull_SOURCES    = BackStatusStatusNull.c
CloseNullError_SOURCES          = CloseNullError.c
CloseInvalidSig_SOURCES         = CloseInvalidSig.c
CloseMemNull_SOURCES            = CloseMemNull.c
ClosePass_SOURCES               = ClosePass.c
CreateBackNullError_SOURCES     = CreateBackNullError.c
CreateBackInvalidSig_SOURCES    = CreateBackInvalidSig.c
CreateBackInvLength_SOURCES     = CreateBackInvLength.c
CreateBackMemNull_SOURCES       = CreateBackMemNull.c
CreateBackNameEmpty_SOURCES     = CreateBackNameEmpty.c
CreateBackPass_SOURCES          = CreateBackPass.c
CreateBackPerm0000_SOURCES      = CreateBackPerm0000.c
CreateBackPerm0200_SOURCES      = CreateBackPerm0200.c
CreateBackPerm0400_SOURCES      = CreateBackPerm0400.c
CreateBackPerm0500_SOURCES      = CreateBackPerm0500.c
FiniInvalidSig_SOURCES          = FiniInvalidSig.c
FiniNullMem_SOURCES             = FiniNullMem.c
FiniPass_SOURCES                = FiniPass.c
InitLengthPositive_SOURCES      = InitLengthPositive.c
InitMemNull_SOURCES             = InitMemNull.c
InitNameEmpty_SOURCES           = InitNameEmpty.c
InitNameNull_SOURCES            = InitNameNull.c
InitPass_SOURCES                = InitPass.c
OpenAddrNull_SOURCES            = OpenAddrNull.c
OpenNullError_SOURCES           = OpenNullError.c
OpenInvalidLength_SOURCES       = OpenInvalidLength.c
OpenInvalidSig_SOURCES          = OpenInvalidSig.c
OpenMemNull_SOURCES             = OpenMemNull.c
OpenNameEmpty_SOURCES           = OpenNameEmpty.c
OpenPass_SOURCES                = OpenPass.c
ReadOnlyNullError_SOURCES       = ReadOnlyNullError.c
ReadOnlyInvalidBaseAddr_SOURCES = ReadOnlyInvalidBaseAddr.c
ReadOnlyInvalidSig_SOURCES      = ReadOnlyInvalidSig.c
ReadOnlyMemNull_SOURCES         = ReadOnlyMemNull.c
ReadOnlyPass_SOURCES            = ReadOnlyPass.c
ReadOnlyTryWrite_SOURCES        = ReadOnlyTryWrite.c
ReadWriteNullError_SOURCES      = ReadWriteNullError.c
ReadWriteInvalidSig_SOURCES     = ReadWriteInvalidSig.c
ReadWriteInvBaseAddr_SOURCES    = ReadWriteInvBaseAddr.c
ReadWriteMemNull_SOURCES        = ReadWriteMemNull.c
ReadWritePass_SOURCES           = ReadWritePass.c
SynchNullError_SOURCES          = SynchNullError.c
SynchInvalidBaseAddr_SOURCES    = SynchInvalidBaseAddr.c
SynchInvalidSig_SOURCES         = SynchInvalidSig.c
SynchMemNull_SOURCES            = SynchMemNull.c
SynchPass_SOURCES               = SynchPass.c
Tests_SOURCES                   = Tests.c


EXTRA_DIST =                     

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=0 PYTHON=$(PYTHON)

TESTS = 

if COND_USE_DBC
TESTS +=                         \
         BackStatusInvalidSig    \
         BackStatusMemNull       \
         BackStatusNameEmpty     \
         BackStatusPass          \
         BackStatusStatusNull    \
         CloseNullError          \
         CloseInvalidSig         \
         CloseMemNull            \
         ClosePass               \
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
         SynchPass               
endif

if COND_USE_EXTREME_DBC
TESTS += 
endif

TESTS += Tests 

XFAIL_TESTS =                         \
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
              ReadOnlyTryWrite        \
              ReadWriteNullError      \
              ReadWriteInvalidSig     \
              ReadWriteInvBaseAddr    \
              ReadWriteMemNull        \
              SynchNullError          \
              SynchInvalidBaseAddr    \
              SynchInvalidSig         \
              SynchMemNull             

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

