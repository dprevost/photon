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

check_PROGRAMS =                      \
                 DeleteInvalidSig     \
                 DeleteNullContext    \
                 DeleteNullFolder     \
                 DeleteNullName       \
                 DeletePass           \
                 DeleteZeroNameLength \
                 FiniInvalidSig       \
                 FiniNullContext      \
                 FiniNullFolder       \
                 FiniPass             \
                 GetInvalidSig        \
                 GetNullContext       \
                 GetNullDescriptor    \
                 GetNullFolder        \
                 GetNullName          \
                 GetPass              \
                 GetZeroNameLength    \
                 InitNullContext      \
                 InitNullFolder       \
                 InitNullName         \
                 InitNullParentOffset \
                 InitPass             \
                 InitZeroBlocks       \
                 InitZeroNameLength   \
                 InsertInvalidSig     \
                 InsertNullContext    \
                 InsertNullFolder     \
                 InsertNullName       \
                 InsertNullOrigName   \
                 InsertPass           \
                 InsertZeroBlocks     \
                 InsertZeroNameLength \
                 Tests

COMMON_HEADER = folderTest.h

FiniInvalidSig_SOURCES       = $(COMMON_HEADER) FiniInvalidSig.c
FiniNullContext_SOURCES      = $(COMMON_HEADER) FiniNullContext.c
FiniNullFolder_SOURCES       = $(COMMON_HEADER) FiniNullFolder.c
FiniPass_SOURCES             = $(COMMON_HEADER) FiniPass.c
GetInvalidSig_SOURCES        = $(COMMON_HEADER) GetInvalidSig.c
GetNullContext_SOURCES       = $(COMMON_HEADER) GetNullContext.c
GetNullDescriptor_SOURCES    = $(COMMON_HEADER) GetNullDescriptor.c
GetNullFolder_SOURCES        = $(COMMON_HEADER) GetNullFolder.c
GetNullName_SOURCES          = $(COMMON_HEADER) GetNullName.c
GetPass_SOURCES              = $(COMMON_HEADER) GetPass.c
GetZeroNameLength_SOURCES    = $(COMMON_HEADER) GetZeroNameLength.c
InitNullContext_SOURCES      = $(COMMON_HEADER) InitNullContext.c
InitNullFolder_SOURCES       = $(COMMON_HEADER) InitNullFolder.c
InitNullName_SOURCES         = $(COMMON_HEADER) InitNullName.c
InitNullParentOffset_SOURCES = $(COMMON_HEADER) InitNullParentOffset.c
InitPass_SOURCES             = $(COMMON_HEADER) InitPass.c
InitZeroBlocks_SOURCES       = $(COMMON_HEADER) InitZeroBlocks.c
InitZeroNameLength_SOURCES   = $(COMMON_HEADER) InitZeroNameLength.c
InsertInvalidSig_SOURCES     = $(COMMON_HEADER) InsertInvalidSig.c
InsertNullContext_SOURCES    = $(COMMON_HEADER) InsertNullContext.c
InsertNullFolder_SOURCES     = $(COMMON_HEADER) InsertNullFolder.c
InsertNullName_SOURCES       = $(COMMON_HEADER) InsertNullName.c
InsertNullOrigName_SOURCES   = $(COMMON_HEADER) InsertNullOrigName.c
InsertPass_SOURCES           = $(COMMON_HEADER) InsertPass.c
InsertZeroBlocks_SOURCES     = $(COMMON_HEADER) InsertZeroBlocks.c
InsertZeroNameLength_SOURCES = $(COMMON_HEADER) InsertZeroNameLength.c

EXTRA_DIST = 

TESTS_ENVIRONMENT = srcdir=$(srcdir) top_builddir=$(top_builddir) verbose=1 PYTHON=$(PYTHON)

TESTS =                      \
        DeleteInvalidSig     \
        DeleteNullContext    \
        DeleteNullFolder     \
        DeleteNullName       \
        DeletePass           \
        DeleteZeroNameLength \
        FiniInvalidSig       \
        FiniNullContext      \
        FiniNullFolder       \
        FiniPass             \
        GetInvalidSig        \
        GetNullContext       \
        GetNullDescriptor    \
        GetNullFolder        \
        GetNullName          \
        GetPass              \
        GetZeroNameLength    \
        InitNullContext      \
        InitNullFolder       \
        InitNullName         \
        InitNullParentOffset \
        InitPass             \
        InitZeroBlocks       \
        InitZeroNameLength   \
        InsertInvalidSig     \
        InsertNullContext    \
        InsertNullFolder     \
        InsertNullName       \
        InsertNullOrigName   \
        InsertPass           \
        InsertZeroBlocks     \
        InsertZeroNameLength \
        Tests
        
XFAIL_TESTS =                      \
              DeleteInvalidSig     \
              DeleteNullContext    \
              DeleteNullFolder     \
              DeleteNullName       \
              DeleteZeroNameLength \
              FiniInvalidSig       \
              FiniNullContext      \
              FiniNullFolder       \
              GetInvalidSig        \
              GetNullContext       \
              GetNullDescriptor    \
              GetNullFolder        \
              GetNullName          \
              GetZeroNameLength    \
              InitNullContext      \
              InitNullFolder       \
              InitNullName         \
              InitNullParentOffset \
              InitZeroBlocks       \
              InitZeroNameLength   \
              InsertInvalidSig     \
              InsertNullContext    \
              InsertNullFolder     \
              InsertNullName       \
              InsertNullOrigName   \
              InsertZeroBlocks     \
              InsertZeroNameLength 

LDADD = $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

