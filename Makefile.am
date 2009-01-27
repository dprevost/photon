# Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~

EXTRA_DIST =                 \
        Makefile.win32       

# The 'doc' subdir must be placed after 'src' since some of the 
# documentation and man pages are generated automatically when
# using "make dist" (otherwise, you may have missing files or
# outdated documentation).

SUBDIRS = m4 src doc

ACLOCAL_AMFLAGS = -I m4

install-data-local:
	-mkdir -p $(DESTDIR)$(datadir)/doc/photon
	$(install_sh_DATA) $(top_srcdir)/README  $(DESTDIR)$(datadir)/doc/photon/README
	$(install_sh_DATA) $(top_srcdir)/COPYING $(DESTDIR)$(datadir)/doc/photon/COPYING

uninstall-local:
	-rm -f $(DESTDIR)$(datadir)/doc/photon/README
	-rm -f $(DESTDIR)$(datadir)/doc/photon/COPYING
	-rmdir $(DESTDIR)$(datadir)/doc/photon
	-rmdir $(DESTDIR)$(datadir)/doc

