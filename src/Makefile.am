# Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#  MAYBE_Java = Java
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

#        Proto         

EXAMPLES_FILES =                                      \
        Examples/iso_3166.tab                         \
        Examples/README.txt                           \
        Examples/quasar-config.example.xml            \
        Examples/C/Examples.sln                       \
        Examples/C/HashMap.c                          \
        Examples/C/HashMapLoop.c                      \
        Examples/C/iso_3166.h                         \
        Examples/C/Makefile                           \
        Examples/C/Queue.h                            \
        Examples/C/QueueIn.c                          \
        Examples/C/QueueOut.c                         \
        Examples/C/QueueWork.c                        \
        Examples/C/README.txt                         \
        Examples/C/RunQueue.sh                        \
        Examples/C/RunQueue.vbs                       \
        Examples/C/Project_Files/HashMap.vcproj       \
        Examples/C/Project_Files/HashMapLoop.vcproj   \
        Examples/C/Project_Files/QueueIn.vcproj       \
        Examples/C/Project_Files/QueueOut.vcproj      \
        Examples/C/Project_Files/QueueWork.vcproj     \
        Examples/C++/Examples.sln                     \
        Examples/C++/HashMap.cpp                      \
        Examples/C++/HashMapLoop.cpp                  \
        Examples/C++/iso_3166.h                       \
        Examples/C++/Makefile                         \
        Examples/C++/Queue.h                          \
        Examples/C++/QueueIn.cpp                      \
        Examples/C++/QueueOut.cpp                     \
        Examples/C++/QueueWork.cpp                    \
        Examples/C++/README.txt                       \
        Examples/C++/RunQueue.sh                      \
        Examples/C++/RunQueue.vbs                     \
        Examples/C++/Project_Files/HashMap.vcproj     \
        Examples/C++/Project_Files/HashMapLoop.vcproj \
        Examples/C++/Project_Files/QueueIn.vcproj     \
        Examples/C++/Project_Files/QueueOut.vcproj    \
        Examples/C++/Project_Files/QueueWork.vcproj   

PACKAGES_FILES =                \
        Packages/photon.iss     \
        Packages/photon.spec.in 

PYTHON_FILES =                       \
        Python/Makefile.in           \
        Python/MANIFEST.in           \
        Python/setup.py              \
        Python/src/BasicDef.h        \
        Python/src/Common.h          \
        Python/src/errors.h          \
        Python/src/FieldDefinition.h \
        Python/src/FolderEntry.h     \
        Python/src/Folder.h          \
        Python/src/Info.h            \
        Python/src/KeyDefinition.h   \
        Python/src/ObjStatus.h       \
        Python/src/pso.c             \
        Python/src/Session.h         \
        Python/test/folder.py        \
        Python/test/session.py       

EXTRA_DIST =                  \
        $(EXAMPLES_FILES)     \
        config-win32.h        \
        Makefile.win32        \
        photon.vs2005.sln     \
        $(PACKAGES_FILES)     \
        $(PYTHON_FILES)       \
        XML/quasar_config.xsd 

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

