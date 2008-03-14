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

CLEANFILES = *~ core

EXTRA_DIST =             \
   Makefile.Win32        \
   PrintError.h          \
   OutOfMemory.sh        \
   RunTests.vbs          \
   TestWithWatchdog.py   \
   TestWithWatchdog.sh   \
   TransactionHashMap.sh \
   TransactionQueue.sh   

check_PROGRAMS =      \
   TransactionHashMap \
   TransactionQueue   \
   InjectErrorsVDS    \
   OutOfMemory        

InjectErrorsVDS_SOURCES    = InjectErrorsVDS.cpp
OutOfMemory_SOURCES        = OutOfMemory.cpp
TransactionHashMap_SOURCES = TransactionHashMap.c
TransactionQueue_SOURCES   = TransactionQueue.c

TESTS_ENVIRONMENT =                              \
                    tcp_port=10701               \
                    top_srcdir=$(top_srcdir)     \
                    srcdir=$(srcdir)             \
                    top_builddir=$(top_builddir) \
                    verbose=1                    \
                    PYTHON=$(PYTHON)

TESTS =                  \
   TransactionHashMap.sh \
   TransactionQueue.sh   \
   OutOfMemory.sh        

LDADD = $(top_builddir)/src/API/libvdsf.la      \
        $(top_builddir)/src/Engine/libEngine.la \
        $(top_builddir)/src/Common/libCommon.la

InjectErrorsVDS_LDADD = $(top_builddir)/src/C++/libvdsfcpp.la 
OutOfMemory_LDADD = $(top_builddir)/src/C++/libvdsfcpp.la 

