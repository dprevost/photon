# Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

CLEANFILES        = *~

AM_CPPFLAGS += @LIBXML2_CFLAGS@
quasar_LDFLAGS = @LIBXML2_LIBS@

SUBDIRS = . Tests
 
bin_PROGRAMS = quasar

man_MANS = quasar.1

OUTPUT_FILES =               \
        quasarErrors.h       \
        quasarErrorHandler.c \
        quasarErrorHandler.h 

if COND_USE_DOCBOOK2XMAN
quasar.1: quasar.xml
	docbook2x-man --encoding=iso-8859-1 quasar.xml
endif
if COND_USE_DOCBOOK2MAN
quasar.1: quasar.xml
	db2x_docbook2man --encoding=iso-8859-1 quasar.xml
endif

if COND_USE_ERRORPARSER
$(OUTPUT_FILES): quasarOptions.xml quasarErrors.xml
	errorParser --options quasarOptions.xml quasarErrors.xml
endif

EXTRA_DIST =              \
        $(man_MANS)       \
        Makefile.win32    \
        quasar.xml        \
        quasar.vcproj     \
        quasarErrors.xml  \
        quasarOptions.xml 

quasar_SOURCES =             \
        quasarErrorHandler.c \
        quasarErrorHandler.h \
        quasarErrors.h       \
        Acceptor.c           \
        Acceptor.h           \
        Config.c             \
        Config.h             \
        LogMsg.c             \
        LogMsg.h             \
        MemoryManager.c      \
        MemoryManager.h      \
        qsrMain.c            \
        Quasar.c             \
        Quasar.h             \
        ShMemHandler.c       \
        ShMemHandler.h       \
        VerifyCommon.h       \
        VerifyFastMap.c      \
        VerifyFolder.c       \
        VerifyHash.c         \
        VerifyHashMap.c      \
        VerifyHashTx.c       \
        VerifyList.c         \
        VerifyMemObj.c       \
        VerifyQueue.c        \
        VerifyShMem.c        \
        VerifyShMem.h        

quasar_LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
               $(top_builddir)/src/Common/libCommon.la   

