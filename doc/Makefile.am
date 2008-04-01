# Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

EXTRA_DIST =  doxyfile.vds_c refman_c.pdf refman_c++.pdf

#
# Only rebuild the ref. manual if both doxygen and latex are present.
#
if COND_USE_DOXYGEN 
if COND_USE_LATEX

all-local: refman_c.pdf refman_c++.pdf

HEADER_FILES =                                      \
        $(top_srcdir)/src/include/vdsf/vds.h        \
        $(top_srcdir)/src/include/vdsf/vdsCommon.h  \
        $(top_srcdir)/src/include/vdsf/vdsFolder.h  \
        $(top_srcdir)/src/include/vdsf/vdsHashMap.h \
        $(top_srcdir)/src/include/vdsf/vdsQueue.h   \
        $(top_srcdir)/src/include/vdsf/vdsProcess.h \
        $(top_srcdir)/src/include/vdsf/vdsSession.h 

CPP_HEADER_FILES =                                  \
        $(top_srcdir)/src/include/vdsf/vds          \
        $(top_srcdir)/src/include/vdsf/vdsException \
        $(top_srcdir)/src/include/vdsf/vdsFolder    \
        $(top_srcdir)/src/include/vdsf/vdsHashMap   \
        $(top_srcdir)/src/include/vdsf/vdsQueue     \
        $(top_srcdir)/src/include/vdsf/vdsProcess   \
        $(top_srcdir)/src/include/vdsf/vdsSession   

refman_c.pdf: $(HEADER_FILES)
	-rm -rf c
	doxygen doxyfile.vds_c $(VERBOSE)
	cd c/latex; make refman.pdf; cd ../..
	cd c/latex; make refman.pdf; cd ../..
	cp c/latex/refman.pdf refman_c.pdf

refman_c++.pdf: $(CPP_HEADER_FILES)
	-rm -rf c++
	doxygen doxyfile.vds_cpp $(VERBOSE)
	cd c++/latex; make refman.pdf; cd ../..
	cd c++/latex; make refman.pdf; cd ../..
	cp c++/latex/refman.pdf refman_c++.pdf

endif
endif

install-data-local:
	-mkdir -p $(DESTDIR)$(datadir)/doc/vdsf
	$(install_sh_DATA) $(top_srcdir)/doc/refman_c.pdf $(DESTDIR)$(datadir)/doc/vdsf/refman_c.pdf
	$(install_sh_DATA) $(top_srcdir)/doc/refman_c++.pdf $(DESTDIR)$(datadir)/doc/vdsf/refman_c++.pdf

uninstall-local:
	-rm -f $(DESTDIR)$(datadir)/doc/vdsf/refman_c.pdf
	-rm -f $(DESTDIR)$(datadir)/doc/vdsf/refman_c++.pdf
	-rmdir $(DESTDIR)$(datadir)/doc/vdsf
	-rmdir $(DESTDIR)$(datadir)/doc

