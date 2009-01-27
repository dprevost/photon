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

CLEANFILES = *~

#noinst_PROGRAMS =     \
        InitCrashDemo \
        CrashClient   \
        TMDemo        \
        TMReadLoop    


#InitCrashDemo_SOURCES = InitCrashDemo.cpp demo.h
#CrashClient_SOURCES   = CrashClient.cpp demo.h
#TMDemo_SOURCES        = TMDemo.cpp
#TMReadLoop_SOURCES    = TMReadLoop.cpp

#EXTRA_DIST =               \
        CrashClient.sh     \
        StartCrashDemo.sh  \
        StartTMDemo.sh     \
        TMDemo.sh          \
        TMReadLoop.sh 


LDADD = $(top_builddir)/src/API/libphoton.la

#distclean-local:
#	-rm -rf $(testsubdir)

