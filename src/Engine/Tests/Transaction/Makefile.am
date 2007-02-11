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
                 AddInvalidSig         \
                 AddNullChildOffset    \
                 AddNullContext        \
                 AddNullParentOffset   \
                 AddNullTx             \
                 AddPass               \
                 CommitInvalidSig      \
                 CommitNullContext     \
                 CommitNullTx          \
                 CommitPass            \
                 FiniInvalidSig        \
                 FiniNullContext       \
                 FiniNullTx            \
                 FiniPass              \
                 InitNullContext       \
                 InitNullTx            \
                 InitPass              \
                 InitZeroBlocks        \
                 RemoveLastInvalidSig  \
                 RemoveLastNullContext \
                 RemoveLastNullTx      \
                 RemoveLastPass        \
                 RollbackInvalidSig    \
                 RollbackNullContext   \
                 RollbackNullTx        \
                 RollbackPass          
                 
AddInvalidSig_SOURCES         = AddInvalidSig.c
AddNullChildOffset_SOURCES    = AddNullChildOffset.c
AddNullContext_SOURCES        = AddNullContext.c
AddNullParentOffset_SOURCES   = AddNullParentOffset.c
AddNullTx_SOURCES             = AddNullTx.c
AddPass_SOURCES               = AddPass.c
CommitInvalidSig_SOURCES      = CommitInvalidSig.c
CommitNullContext_SOURCES     = CommitNullContext.c
CommitNullTx_SOURCES          = CommitNullTx.c
CommitPass_SOURCES            = CommitPass.c
FiniInvalidSig_SOURCES        = FiniInvalidSig.c
FiniNullContext_SOURCES       = FiniNullContext.c
FiniNullTx_SOURCES            = FiniNullTx.c
FiniPass_SOURCES              = FiniPass.c
InitNullContext_SOURCES       = InitNullContext.c
InitNullTx_SOURCES            = InitNullTx.c
InitPass_SOURCES              = InitPass.c
InitZeroBlocks_SOURCES        = InitZeroBlocks.c
RemoveLastInvalidSig_SOURCES  = RemoveLastInvalidSig.c
RemoveLastNullContext_SOURCES = RemoveLastNullContext.c
RemoveLastNullTx_SOURCES      = RemoveLastNullTx.c
RemoveLastPass_SOURCES        = RemoveLastPass.c
RollbackInvalidSig_SOURCES    = RollbackInvalidSig.c
RollbackNullContext_SOURCES   = RollbackNullContext.c
RollbackNullTx_SOURCES        = RollbackNullTx.c
RollbackPass_SOURCES          = RollbackPass.c
                 
EXTRA_DIST =         

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS =                       \
        AddInvalidSig         \
        AddNullChildOffset    \
        AddNullContext        \
        AddNullParentOffset   \
        AddNullTx             \
        AddPass               \
        CommitInvalidSig      \
        CommitNullContext     \
        CommitNullTx          \
        CommitPass            \
        FiniInvalidSig        \
        FiniNullContext       \
        FiniNullTx            \
        FiniPass              \
        InitNullContext       \
        InitNullTx            \
        InitPass              \
        InitZeroBlocks        \
        RemoveLastInvalidSig  \
        RemoveLastNullContext \
        RemoveLastNullTx      \
        RemoveLastPass        \
        RollbackInvalidSig    \
        RollbackNullContext   \
        RollbackNullTx        \
        RollbackPass          
                 
XFAIL_TESTS =                       \
              AddInvalidSig         \
              AddNullChildOffset    \
              AddNullContext        \
              AddNullParentOffset   \
              AddNullTx             \
              CommitInvalidSig      \
              CommitNullContext     \
              CommitNullTx          \
              FiniInvalidSig        \
              FiniNullContext       \
              FiniNullTx            \
              InitNullContext       \
              InitNullTx            \
              InitZeroBlocks        \
              RemoveLastInvalidSig  \
              RemoveLastNullContext \
              RemoveLastNullTx      \
              RollbackInvalidSig    \
              RollbackNullContext   \
              RollbackNullTx        

LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

AM_CPPFLAGS = -I$(top_srcdir)/src/Engine       \
              -I$(top_srcdir)/src/Engine/Tests \
              -I$(top_srcdir)/src/Common       \
              -I$(top_srcdir)/src/Tests        \
              -I$(top_srcdir)/src/include

if COND_USE_ASSERT
   AM_CPPFLAGS += -DUSE_ASSERT
endif
if COND_USE_TRAP_ERRORS
   AM_CPPFLAGS += -DUSE_TRAP_ERRORS
endif

if COND_USE_DBC
   AM_CPPFLAGS += -DUSE_DBC
endif
if COND_USE_EXTREME_DBC
   AM_CPPFLAGS += -DUSE_EXTREME_DBC
endif


