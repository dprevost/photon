# Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

noinst_LIBRARIES = libVerifyVDS.a

man_MANS = quasar.1
info_TEXINFOS = quasar.texi

OUTPUT_FILES =               \
        quasarErrors.h       \
        quasarErrorHandler.c \
        quasarErrorHandler.h 

if COND_USE_DOCBOOK2TEXI
quasar.texi: quasar.xml
	docbook2x-texi --encoding=iso-8859-1 quasar.xml
endif
if COND_USE_DOCBOOK2MAN
quasar.1: quasar.xml
	docbook2x-man --encoding=iso-8859-1 quasar.xml
endif

if COND_USE_ERRORPARSER
##all-local: $(OUTPUT_FILES)

$(OUTPUT_FILES): quasarOptions.xml quasarErrors.xml
	errorParser --options quasarOptions.xml quasarErrors.xml
endif

EXTRA_DIST =              \
        $(man_MANS)       \
        Makefile.win32    \
        quasar.xml        \
        Watchdog.dsp      \
        quasarErrors.xml  \
        quasarOptions.xml 

quasar_SOURCES =             \
        quasarErrorHandler.c \
        quasarErrorHandler.h \
        quasarErrors.h       \
        Acceptor.c           \
        Acceptor.h           \
        Config.h             \
        Config.c             \
        LogMsg.c             \
        LogMsg.h             \
        MemoryManager.c      \
        MemoryManager.h      \
        VDSHandler.c         \
        VDSHandler.h         \
        Watchdog.c           \
        Watchdog.h           \
        wd.c                 

libVerifyVDS_a_SOURCES = \
        VerifyCommon.h   \
        VerifyFastMap.c  \
        VerifyFolder.c   \
        VerifyHash.c     \
        VerifyHashMap.c  \
        VerifyList.c     \
        VerifyMemObj.c   \
        VerifyQueue.c    \
        VerifyVDS.c      \
        VerifyVDS.h      

quasar_LDADD = $(top_builddir)/src/Nucleus/libNucleus.la  \
              $(top_builddir)/src/Common/libCommon.la     \
              $(top_builddir)/src/Quasar/libVerifyVDS.a 

