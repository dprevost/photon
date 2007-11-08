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

# To debug, uncomment the next line and comment the next after that.
#VERBOSE =
VERBOSE = > /dev/null 2>&1
# A note on VERBOSE: it might be easier to debug by running each program
# manually... YMMV

#
# Only rebuild the man pages if doxygen is present.
#
if COND_USE_DOXYGEN

all-local: refman.pdf

#EXTRA_DIST = refman.pdf

HEADER_FILES =                                 \
   $(top_srcdir)/src/include/vdsf/vds.h        \
   $(top_srcdir)/src/include/vdsf/vdsCommon.h  \
   $(top_srcdir)/src/include/vdsf/vdsFolder.h  \
   $(top_srcdir)/src/include/vdsf/vdsHashMap.h \
   $(top_srcdir)/src/include/vdsf/vdsQueue.h   \
   $(top_srcdir)/src/include/vdsf/vdsProcess.h \
   $(top_srcdir)/src/include/vdsf/vdsSession.h 

refman.pdf: $(HEADER_FILES)
	doxygen Doxyfile.vds_c $(VERBOSE)
	cd latex; latex refman.tex $(VERBOSE); cd ..
	cd latex; latex refman.tex $(VERBOSE); cd ..
	cd latex; dvipdfm refman.dvi $(VERBOSE); cd ..
	cp latex/refman.pdf .

endif

install-data-local:
