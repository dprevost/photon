# Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES        = *~

bin_PROGRAMS      = errorParser

errorParser_SOURCES =                  \
                      analyser.cpp     \
                      analyser.h       \
                      analyserTest.cpp \
                      analyserTest.h   \
                      errorParser.cpp  \
                      inputHeader.cpp  \
                      inputHeader.h    \
                      outputFiles.cpp  \
                      outputFiles.h   

AM_CPPFLAGS       = -I$(top_srcdir)/src/Common 

if COND_USE_DBC
   AM_CPPFLAGS += -DUSE_DBC
endif
if COND_USE_EXTREME_DBC
   AM_CPPFLAGS += -DUSE_EXTREME_DBC
endif

errorParser_LDADD = $(top_builddir)/src/Common/libCommon.la 

