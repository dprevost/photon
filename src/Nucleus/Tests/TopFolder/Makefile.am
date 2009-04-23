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

check_PROGRAMS =                \
        CloseNullContext        \
        CloseNullItem           \
        ClosePass               \
        CreateFolderInvLength   \
        CreateFolderNullContext \
        CreateFolderNullFolder  \
        CreateFolderNullName    \
        CreateFolderPass        \
        CreateInvalidNameLength \
        CreateNullContext       \
        CreateNullDefinition    \
        CreateNullFolder        \
        CreateNullName          \
        CreatePass              \
        CreateWrongType         \
        DestroyNullContext      \
        DestroyNullFolder       \
        DestroyNullName         \
        DestroyPass             \
        EditNullContext         \
        EditNullFolder          \
        EditNullItem            \
        EditNullName            \
        EditPass                \
        EditWrongLength         \
        EditWrongType           \
        EditZeroLength          \
        GetDefNullContext       \
        GetDefNullDataDef       \
        GetDefNullDef           \
        GetDefNullFolder        \
        GetDefNullKeyDef        \
        GetDefNullName          \
        GetDefPass              \
        OpenNullContext         \
        OpenNullItem            \
        OpenNullFolder          \
        OpenNullName            \
        OpenPass                \
        OpenWrongLength         \
        OpenWrongType           \
        OpenZeroLength          \
        StatusNullContext       \
        StatusNullFolder        \
        StatusNullName          \
        StatusNullStatus        \
        StatusPass              

COMMON_HEADER = folderTest.h

CloseNullContext_SOURCES        = $(COMMON_HEADER) CloseNullContext.c
CloseNullItem_SOURCES           = $(COMMON_HEADER) CloseNullItem.c
ClosePass_SOURCES               = $(COMMON_HEADER) ClosePass.c
CreateFolderInvLength_SOURCES   = $(COMMON_HEADER) CreateFolderInvLength.c
CreateFolderNullContext_SOURCES = $(COMMON_HEADER) CreateFolderNullContext.c
CreateFolderNullFolder_SOURCES  = $(COMMON_HEADER) CreateFolderNullFolder.c
CreateFolderNullName_SOURCES    = $(COMMON_HEADER) CreateFolderNullName.c
CreateFolderPass_SOURCES        = $(COMMON_HEADER) CreateFolderPass.c
CreateInvalidNameLength_SOURCES = $(COMMON_HEADER) CreateInvalidNameLength.c
CreateNullContext_SOURCES       = $(COMMON_HEADER) CreateNullContext.c
CreateNullDefinition_SOURCES    = $(COMMON_HEADER) CreateNullDefinition.c
CreateNullFolder_SOURCES        = $(COMMON_HEADER) CreateNullFolder.c
CreateNullName_SOURCES          = $(COMMON_HEADER) CreateNullName.c
CreatePass_SOURCES              = $(COMMON_HEADER) CreatePass.c
CreateWrongType_SOURCES         = $(COMMON_HEADER) CreateWrongType.c
DestroyNullContext_SOURCES      = $(COMMON_HEADER) DestroyNullContext.c
DestroyNullFolder_SOURCES       = $(COMMON_HEADER) DestroyNullFolder.c
DestroyNullName_SOURCES         = $(COMMON_HEADER) DestroyNullName.c
DestroyPass_SOURCES             = $(COMMON_HEADER) DestroyPass.c
EditNullContext_SOURCES         = $(COMMON_HEADER) EditNullContext.c
EditNullFolder_SOURCES          = $(COMMON_HEADER) EditNullFolder.c
EditNullItem_SOURCES            = $(COMMON_HEADER) EditNullItem.c
EditNullName_SOURCES            = $(COMMON_HEADER) EditNullName.c
EditPass_SOURCES                = $(COMMON_HEADER) EditPass.c
EditWrongLength_SOURCES         = $(COMMON_HEADER) EditWrongLength.c
EditWrongType_SOURCES           = $(COMMON_HEADER) EditWrongType.c
EditZeroLength_SOURCES          = $(COMMON_HEADER) EditZeroLength.c
GetDefNullContext_SOURCES       = $(COMMON_HEADER) GetDefNullContext.c
GetDefNullDataDef_SOURCES       = $(COMMON_HEADER) GetDefNullDataDef.c
GetDefNullDef_SOURCES           = $(COMMON_HEADER) GetDefNullDef.c
GetDefNullFolder_SOURCES        = $(COMMON_HEADER) GetDefNullFolder.c
GetDefNullKeyDef_SOURCES        = $(COMMON_HEADER) GetDefNullKeyDef.c
GetDefNullName_SOURCES          = $(COMMON_HEADER) GetDefNullName.c
GetDefPass_SOURCES              = $(COMMON_HEADER) GetDefPass.c
OpenNullContext_SOURCES         = $(COMMON_HEADER) OpenNullContext.c
OpenNullItem_SOURCES            = $(COMMON_HEADER) OpenNullItem.c
OpenNullFolder_SOURCES          = $(COMMON_HEADER) OpenNullFolder.c
OpenNullName_SOURCES            = $(COMMON_HEADER) OpenNullName.c
OpenPass_SOURCES                = $(COMMON_HEADER) OpenPass.c
OpenWrongLength_SOURCES         = $(COMMON_HEADER) OpenWrongLength.c
OpenWrongType_SOURCES           = $(COMMON_HEADER) OpenWrongType.c
OpenZeroLength_SOURCES          = $(COMMON_HEADER) OpenZeroLength.c
StatusNullContext_SOURCES       = $(COMMON_HEADER) StatusNullContext.c
StatusNullFolder_SOURCES        = $(COMMON_HEADER) StatusNullFolder.c
StatusNullName_SOURCES          = $(COMMON_HEADER) StatusNullName.c
StatusNullStatus_SOURCES        = $(COMMON_HEADER) StatusNullStatus.c
StatusPass_SOURCES              = $(COMMON_HEADER) StatusPass.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                         \
        CloseNullContext        \
        CloseNullItem           \
        ClosePass               \
        CreateFolderInvLength   \
        CreateFolderNullContext \
        CreateFolderNullFolder  \
        CreateFolderNullName    \
        CreateFolderPass        \
        CreateInvalidNameLength \
        CreateNullContext       \
        CreateNullDefinition    \
        CreateNullFolder        \
        CreateNullName          \
        CreatePass              \
        CreateWrongType         \
        DestroyNullContext      \
        DestroyNullFolder       \
        DestroyNullName         \
        DestroyPass             \
        EditNullContext         \
        EditNullFolder          \
        EditNullItem            \
        EditNullName            \
        EditPass                \
        EditWrongLength         \
        EditWrongType           \
        EditZeroLength          \
        GetDefNullContext       \
        GetDefNullDataDef       \
        GetDefNullDef           \
        GetDefNullFolder        \
        GetDefNullKeyDef        \
        GetDefNullName          \
        GetDefPass              \
        OpenNullContext         \
        OpenNullItem            \
        OpenNullFolder          \
        OpenNullName            \
        OpenPass                \
        OpenWrongLength         \
        OpenWrongType           \
        OpenZeroLength          \
        StatusNullContext       \
        StatusNullFolder        \
        StatusNullName          \
        StatusNullStatus        \
        StatusPass              

XFAIL_TESTS =                   \
        CloseNullContext        \
        CloseNullItem           \
        CreateFolderNullContext \
        CreateFolderNullFolder  \
        CreateFolderNullName    \
        CreateNullContext       \
        CreateNullDefinition    \
        CreateNullFolder        \
        CreateNullName          \
        CreateWrongType         \
        DestroyNullContext      \
        DestroyNullFolder       \
        DestroyNullName         \
        EditNullContext         \
        EditNullFolder          \
        EditNullItem            \
        EditNullName            \
        EditWrongType           \
        GetDefNullContext       \
        GetDefNullDataDef       \
        GetDefNullDef           \
        GetDefNullFolder        \
        GetDefNullKeyDef        \
        GetDefNullName          \
        OpenNullContext         \
        OpenNullItem            \
        OpenNullFolder          \
        OpenNullName            \
        OpenWrongType           \
        StatusNullContext       \
        StatusNullFolder        \
        StatusNullName          \
        StatusNullStatus        

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

