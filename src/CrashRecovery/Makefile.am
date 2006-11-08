# Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~

bin_PROGRAMS = vdscr

vdscr_SOURCES =                         \
                CrashRecovery.cpp       \
                MemoryRecovery.cpp      \
                MemoryRecovery.h        \
                Report.cpp              \
                Report.h                \
                TransactionRecovery.cpp \
                TransactionRecovery.h   \
                TreeRecovery.cpp        \
                TreeRecovery.h          

AM_CPPFLAGS = -I$(top_srcdir)/src/Common     \
              -I$(top_srcdir)/src/include    \
              -I$(top_srcdir)/src/Engine \
              -I$(top_srcdir)/src/API

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

vdscr_LDADD = \
   $(top_builddir)/src/API/libvdsf.la 

###   $(top_builddir)/src/Wrappers/libCommon.la
