# Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~ Packages\*~

# The code have to be build following a certain order:
#   1-    Errors
#   2-    Common
#   3-    Nucleus
#   4-    Quasar
#   5-    API
#   n-    ...
#   Last- Tests

if COND_USE_JAVA
  MAYBE_Java = Java
endif

SUBDIRS =             \
        Errors        \
        Common        \
        Nucleus       \
        Quasar        \
        API           \
        C++           \
        $(MAYBE_Java) \
        include       \
        XML           \
        Tests         \
        Tools         

# To be added after the C++ subdir
# $(MAYBE_Java) 

EXAMPLES_FILES =                                   \
        Examples/iso_3166.tab                      \
        Examples/README.txt                        \
        Examples/quasar-config.example              \
        Examples/C/Examples.dsw                    \
        Examples/C/HashMap.c                       \
        Examples/C/HashMapLoop.c                   \
        Examples/C/iso_3166.h                      \
        Examples/C/Makefile                        \
        Examples/C/Queue.h                         \
        Examples/C/QueueIn.c                       \
        Examples/C/QueueOut.c                      \
        Examples/C/QueueWork.c                     \
        Examples/C/README.txt                      \
        Examples/C/RunQueue.sh                     \
        Examples/C/RunQueue.vbs                    \
        Examples/C/Project_Files/HashMap.dsp       \
        Examples/C/Project_Files/HashMapLoop.dsp   \
        Examples/C/Project_Files/QueueIn.dsp       \
        Examples/C/Project_Files/QueueOut.dsp      \
        Examples/C/Project_Files/QueueWork.dsp     \
        Examples/C++/Examples.dsw                  \
        Examples/C++/HashMap.cpp                   \
        Examples/C++/HashMapLoop.cpp               \
        Examples/C++/iso_3166.h                    \
        Examples/C++/Makefile                      \
        Examples/C++/Queue.h                       \
        Examples/C++/QueueIn.cpp                   \
        Examples/C++/QueueOut.cpp                  \
        Examples/C++/QueueWork.cpp                 \
        Examples/C++/README.txt                    \
        Examples/C++/RunQueue.sh                   \
        Examples/C++/RunQueue.vbs                  \
        Examples/C++/Project_Files/HashMap.dsp     \
        Examples/C++/Project_Files/HashMapLoop.dsp \
        Examples/C++/Project_Files/QueueIn.dsp     \
        Examples/C++/Project_Files/QueueOut.dsp    \
        Examples/C++/Project_Files/QueueWork.dsp   

PACKAGES_FILES =                \
        Packages/photon.iss     \
        Packages/photon.spec.in 

EXTRA_DIST =               \
        $(EXAMPLES_FILES)  \
        config-win32.h     \
        Makefile.win32     \
        photon.dsw         \
        $(PACKAGES_FILES)  \
        XML/wd_config.xsd  

install-data-local:
	-mkdir -p $(DESTDIR)$(datadir)/doc/photon/Examples
	for f in $(EXAMPLES_FILES); do \
		$(install_sh_DATA) $(top_srcdir)/src/$$f $(DESTDIR)$(datadir)/doc/photon/$$f ; \
	done

uninstall-local:
	-for f in $(EXAMPLES_FILES); do \
		rm -f $(DESTDIR)$(datadir)/doc/photon/$$f ; \
	done
	-rmdir $(DESTDIR)$(datadir)/doc/photon
	-rmdir $(DESTDIR)$(datadir)/doc

