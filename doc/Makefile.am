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

EXTRA_DIST =  doxyfile.photon_c doxyfile.photon_cpp refman_c.pdf refman_c++.pdf

#
# Only rebuild the ref. manual if both doxygen and latex are present.
#
if COND_USE_DOXYGEN 
if COND_USE_LATEX

all-local: refman_c.pdf refman_c++.pdf

HEADER_FILES =                                      \
        $(top_srcdir)/src/include/photon/photon.h        \
        $(top_srcdir)/src/include/photon/psoCommon.h  \
        $(top_srcdir)/src/include/photon/psoFolder.h  \
        $(top_srcdir)/src/include/photon/psoHashMap.h \
        $(top_srcdir)/src/include/photon/psoQueue.h   \
        $(top_srcdir)/src/include/photon/psoProcess.h \
        $(top_srcdir)/src/include/photon/psoSession.h 

CPP_HEADER_FILES =                                  \
        $(top_srcdir)/src/include/photon/photon          \
        $(top_srcdir)/src/include/photon/psoException \
        $(top_srcdir)/src/include/photon/psoFolder    \
        $(top_srcdir)/src/include/photon/psoHashMap   \
        $(top_srcdir)/src/include/photon/psoQueue     \
        $(top_srcdir)/src/include/photon/psoProcess   \
        $(top_srcdir)/src/include/photon/psoSession   

refman_c.pdf: $(HEADER_FILES)
	-rm -rf c
	doxygen doxyfile.photon_c $(VERBOSE)
	cd c/latex; make refman.pdf; cd ../..
	cd c/latex; make refman.pdf; cd ../..
	cp c/latex/refman.pdf refman_c.pdf

refman_c++.pdf: $(CPP_HEADER_FILES)
	-rm -rf c++
	doxygen doxyfile.photon_cpp $(VERBOSE)
	cd c++/latex; make refman.pdf; cd ../..
	cd c++/latex; make refman.pdf; cd ../..
	cp c++/latex/refman.pdf refman_c++.pdf

endif
endif

install-data-local:
	-mkdir -p $(DESTDIR)$(datadir)/doc/photon
	$(install_sh_DATA) $(top_srcdir)/doc/refman_c.pdf $(DESTDIR)$(datadir)/doc/photon/refman_c.pdf
	$(install_sh_DATA) $(top_srcdir)/doc/refman_c++.pdf $(DESTDIR)$(datadir)/doc/photon/refman_c++.pdf

uninstall-local:
	-rm -f $(DESTDIR)$(datadir)/doc/photon/refman_c.pdf
	-rm -f $(DESTDIR)$(datadir)/doc/photon/refman_c++.pdf
	-rmdir $(DESTDIR)$(datadir)/doc/photon
	-rmdir $(DESTDIR)$(datadir)/doc

