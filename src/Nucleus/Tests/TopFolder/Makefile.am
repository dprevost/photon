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

check_PROGRAMS =                   \
        TopCloseNullContext        \
        TopCloseNullItem           \
        TopClosePass               \
        TopCreateInvalidNameLength \
        TopCreateNullContext       \
        TopCreateNullDefinition    \
        TopCreateNullFolder        \
        TopCreateNullName          \
        TopCreatePass              \
        TopCreateWrongType         \
        TopDestroyNullContext      \
        TopDestroyNullFolder       \
        TopDestroyNullName         \
        TopDestroyPass             \
        TopEditNullContext         \
        TopEditNullFolder          \
        TopEditNullItem            \
        TopEditNullName            \
        TopEditPass                \
        TopEditWrongLength         \
        TopEditWrongType           \
        TopEditZeroLength          \
        TopOpenNullContext         \
        TopOpenNullItem            \
        TopOpenNullFolder          \
        TopOpenNullName            \
        TopOpenPass                \
        TopOpenWrongLength         \
        TopOpenWrongType           \
        TopOpenZeroLength          \
        TopStatusNullContext       \
        TopStatusNullFolder        \
        TopStatusNullName          \
        TopStatusNullStatus        \
        TopStatusPass              

COMMON_HEADER = ../Folder/folderTest.h

TopCloseNullContext_SOURCES        = $(COMMON_HEADER) TopCloseNullContext.c
TopCloseNullItem_SOURCES           = $(COMMON_HEADER) TopCloseNullItem.c
TopClosePass_SOURCES               = $(COMMON_HEADER) TopClosePass.c
TopCreateInvalidNameLength_SOURCES = $(COMMON_HEADER) TopCreateInvalidNameLength.c
TopCreateNullContext_SOURCES       = $(COMMON_HEADER) TopCreateNullContext.c
TopCreateNullDefinition_SOURCES    = $(COMMON_HEADER) TopCreateNullDefinition.c
TopCreateNullFolder_SOURCES        = $(COMMON_HEADER) TopCreateNullFolder.c
TopCreateNullName_SOURCES          = $(COMMON_HEADER) TopCreateNullName.c
TopCreatePass_SOURCES              = $(COMMON_HEADER) TopCreatePass.c
TopCreateWrongType_SOURCES         = $(COMMON_HEADER) TopCreateWrongType.c
TopDestroyNullContext_SOURCES      = $(COMMON_HEADER) TopDestroyNullContext.c
TopDestroyNullFolder_SOURCES       = $(COMMON_HEADER) TopDestroyNullFolder.c
TopDestroyNullName_SOURCES         = $(COMMON_HEADER) TopDestroyNullName.c
TopDestroyPass_SOURCES             = $(COMMON_HEADER) TopDestroyPass.c
TopEditNullContext_SOURCES         = $(COMMON_HEADER) TopEditNullContext.c
TopEditNullFolder_SOURCES          = $(COMMON_HEADER) TopEditNullFolder.c
TopEditNullItem_SOURCES            = $(COMMON_HEADER) TopEditNullItem.c
TopEditNullName_SOURCES            = $(COMMON_HEADER) TopEditNullName.c
TopEditPass_SOURCES                = $(COMMON_HEADER) TopEditPass.c
TopEditWrongLength_SOURCES         = $(COMMON_HEADER) TopEditWrongLength.c
TopEditWrongType_SOURCES           = $(COMMON_HEADER) TopEditWrongType.c
TopEditZeroLength_SOURCES          = $(COMMON_HEADER) TopEditZeroLength.c
TopOpenNullContext_SOURCES         = $(COMMON_HEADER) TopOpenNullContext.c
TopOpenNullItem_SOURCES            = $(COMMON_HEADER) TopOpenNullItem.c
TopOpenNullFolder_SOURCES          = $(COMMON_HEADER) TopOpenNullFolder.c
TopOpenNullName_SOURCES            = $(COMMON_HEADER) TopOpenNullName.c
TopOpenPass_SOURCES                = $(COMMON_HEADER) TopOpenPass.c
TopOpenWrongLength_SOURCES         = $(COMMON_HEADER) TopOpenWrongLength.c
TopOpenWrongType_SOURCES           = $(COMMON_HEADER) TopOpenWrongType.c
TopOpenZeroLength_SOURCES          = $(COMMON_HEADER) TopOpenZeroLength.c
TopStatusNullContext_SOURCES       = $(COMMON_HEADER) TopStatusNullContext.c
TopStatusNullFolder_SOURCES        = $(COMMON_HEADER) TopStatusNullFolder.c
TopStatusNullName_SOURCES          = $(COMMON_HEADER) TopStatusNullName.c
TopStatusNullStatus_SOURCES        = $(COMMON_HEADER) TopStatusNullStatus.c
TopStatusPass_SOURCES              = $(COMMON_HEADER) TopStatusPass.c

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                            \
        TopCloseNullContext        \
        TopCloseNullItem           \
        TopClosePass               \
        TopCreateInvalidNameLength \
        TopCreateNullContext       \
        TopCreateNullDefinition    \
        TopCreateNullFolder        \
        TopCreateNullName          \
        TopCreatePass              \
        TopCreateWrongType         \
        TopDestroyNullContext      \
        TopDestroyNullFolder       \
        TopDestroyNullName         \
        TopDestroyPass             \
        TopEditNullContext         \
        TopEditNullFolder          \
        TopEditNullItem            \
        TopEditNullName            \
        TopEditPass                \
        TopEditWrongLength         \
        TopEditWrongType           \
        TopEditZeroLength          \
        TopOpenNullContext         \
        TopOpenNullItem            \
        TopOpenNullFolder          \
        TopOpenNullName            \
        TopOpenPass                \
        TopOpenWrongLength         \
        TopOpenWrongType           \
        TopOpenZeroLength          \
        TopStatusNullContext       \
        TopStatusNullFolder        \
        TopStatusNullName          \
        TopStatusNullStatus        \
        TopStatusPass              

XFAIL_TESTS =                      \
        TopCloseNullContext        \
        TopCloseNullItem           \
        TopCreateNullContext       \
        TopCreateNullDefinition    \
        TopCreateNullFolder        \
        TopCreateNullName          \
        TopCreateWrongType         \
        TopDestroyNullContext      \
        TopDestroyNullFolder       \
        TopDestroyNullName         \
        TopEditNullContext         \
        TopEditNullFolder          \
        TopEditNullItem            \
        TopEditNullName            \
        TopEditWrongType           \
        TopOpenNullContext         \
        TopOpenNullItem            \
        TopOpenNullFolder          \
        TopOpenNullName            \
        TopOpenWrongType           \
        TopStatusNullContext       \
        TopStatusNullFolder        \
        TopStatusNullName          \
        TopStatusNullStatus        

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

