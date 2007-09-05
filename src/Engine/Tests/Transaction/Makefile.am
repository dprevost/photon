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
                 
COMMON_HEADER = txTest.h

AddInvalidSig_SOURCES         = $(COMMON_HEADER) AddInvalidSig.c
AddNullChildOffset_SOURCES    = $(COMMON_HEADER) AddNullChildOffset.c
AddNullContext_SOURCES        = $(COMMON_HEADER) AddNullContext.c
AddNullParentOffset_SOURCES   = $(COMMON_HEADER) AddNullParentOffset.c
AddNullTx_SOURCES             = $(COMMON_HEADER) AddNullTx.c
AddPass_SOURCES               = $(COMMON_HEADER) AddPass.c
CommitInvalidSig_SOURCES      = $(COMMON_HEADER) CommitInvalidSig.c
CommitNullContext_SOURCES     = $(COMMON_HEADER) CommitNullContext.c
CommitNullTx_SOURCES          = $(COMMON_HEADER) CommitNullTx.c
CommitPass_SOURCES            = $(COMMON_HEADER) CommitPass.c
FiniInvalidSig_SOURCES        = $(COMMON_HEADER) FiniInvalidSig.c
FiniNullContext_SOURCES       = $(COMMON_HEADER) FiniNullContext.c
FiniNullTx_SOURCES            = $(COMMON_HEADER) FiniNullTx.c
FiniPass_SOURCES              = $(COMMON_HEADER) FiniPass.c
InitNullContext_SOURCES       = $(COMMON_HEADER) InitNullContext.c
InitNullTx_SOURCES            = $(COMMON_HEADER) InitNullTx.c
InitPass_SOURCES              = $(COMMON_HEADER) InitPass.c
InitZeroBlocks_SOURCES        = $(COMMON_HEADER) InitZeroBlocks.c
RemoveLastInvalidSig_SOURCES  = $(COMMON_HEADER) RemoveLastInvalidSig.c
RemoveLastNullContext_SOURCES = $(COMMON_HEADER) RemoveLastNullContext.c
RemoveLastNullTx_SOURCES      = $(COMMON_HEADER) RemoveLastNullTx.c
RemoveLastPass_SOURCES        = $(COMMON_HEADER) RemoveLastPass.c
RollbackInvalidSig_SOURCES    = $(COMMON_HEADER) RollbackInvalidSig.c
RollbackNullContext_SOURCES   = $(COMMON_HEADER) RollbackNullContext.c
RollbackNullTx_SOURCES        = $(COMMON_HEADER) RollbackNullTx.c
RollbackPass_SOURCES          = $(COMMON_HEADER) RollbackPass.c
                 
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

if COND_USE_KERNEL_HEADERS
   AM_CPPFLAGS += -DCONFIG_KERNEL_HEADERS \
                  -I$(top_srcdir)/src/Locking/include \
                  -include $(top_srcdir)/src/Locking/include/mylinux/autoconf.h 
endif

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


