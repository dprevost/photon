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

SUBDIRS = . Tests
 
bin_PROGRAMS = vdswd

noinst_LIBRARIES = libVerifyVDS.a

man_MANS = vdswd.1
info_TEXINFOS = vdswd.texi

if COND_USE_DOCBOOK2TEXI
vdswd.texi: vdswd.xml
	docbook2x-texi --encoding=iso-8859-1 vdswd.xml
endif
if COND_USE_DOCBOOK2MAN
vdswd.1: vdswd.xml
	docbook2x-man --encoding=iso-8859-1 vdswd.xml
endif

EXTRA_DIST =           \
        $(man_MANS)    \
        Makefile.win32 \
        Watchdog.dsp   

vdswd_SOURCES =           \
        Acceptor.cpp      \
        Acceptor.h        \
        LogMsg.cpp        \
        LogMsg.h          \
        MemoryManager.cpp \
        MemoryManager.h   \
        VDSHandler.cpp    \
        VDSHandler.h      \
        Watchdog.cpp      \
        Watchdog.h        \
        wd.cpp            

libVerifyVDS_a_SOURCES =      \
        VerifyCommon.h        \
        VerifyFolder.c        \
        VerifyHash.c          \
        VerifyHashMap.c       \
        VerifyList.c          \
        VerifyMemObj.c        \
        VerifyQueue.c         \
        VerifyVDS.c           \
        VerifyVDS.h           

vdswd_LDADD = $(top_builddir)/src/Engine/libEngine.la    \
              $(top_builddir)/src/Common/libCommon.la    \
              $(top_builddir)/src/Watchdog/libVerifyVDS.a 

