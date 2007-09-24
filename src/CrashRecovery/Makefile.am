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

CLEANFILES = *~

#bin_PROGRAMS = vdscr

#vdscr_SOURCES =                         \
                CrashRecovery.cpp       \
                MemoryRecovery.cpp      \
                MemoryRecovery.h        \
                Report.cpp              \
                Report.h                \
                TransactionRecovery.cpp \
                TransactionRecovery.h   \
                TreeRecovery.cpp        \
                TreeRecovery.h          

vdscr_LDADD = \
   $(top_builddir)/src/API/libvdsf.la 

###   $(top_builddir)/src/Wrappers/libCommon.la
