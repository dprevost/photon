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
#   3-    Engine
#   4-    Watchdog
#   5-    API
#   n-    ...
#   Last- Tests

if COND_USE_JAVA
  MAYBE_Java = Java
endif

SUBDIRS =             \
        Errors        \
        Common        \
        Engine        \
        Watchdog      \
        API           \
        C++           \
        $(MAYBE_Java) \
        include       \
        Tests         \
        Tools         

# To be added after the C++ subdir
# $(MAYBE_Java) 

BASE_LOCK_FILES =                        \
        Locking/linux/autoconf.h         \
        Locking/linux/compiler-gcc.h     \
        Locking/linux/compiler.h         \
        Locking/linux/compiler-intel.h   \
        Locking/linux/linkage.h          \
        Locking/linux/spinlock_api_smp.h \
        Locking/linux/spinlock.h         \
        Locking/linux/spinlock_types.h   

I386_FILES =                              \
        Locking/asm-i386/alternative.h    \
        Locking/asm-i386/bitops.h         \
        Locking/asm-i386/cpufeature.h     \
        Locking/asm-i386/linkage.h        \
        Locking/asm-i386/spinlock.h       \
        Locking/asm-i386/spinlock_types.h \
        Locking/asm-i386/system.h         

SPARC_FILES =                              \
        Locking/asm-sparc/linkage.h        \
        Locking/asm-sparc/spinlock.h       \
        Locking/asm-sparc/spinlock_types.h \
        Locking/asm-sparc/system.h         

X86_64_FILES =                              \
        Locking/asm-x86_64/linkage.h        \
        Locking/asm-x86_64/spinlock.h       \
        Locking/asm-x86_64/spinlock_types.h \
        Locking/asm-x86_64/system.h         

EXAMPLES_FILES =                                   \
        Examples/iso_3166.tab                      \
        Examples/README.txt                        \
        Examples/vdswd-config.example              \
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

PACKAGES_FILES =              \
        Packages/vdsf.iss     \
        Packages/vdsf.spec.in 

EXTRA_DIST =               \
        $(BASE_LOCK_FILES) \
        $(I386_FILES)      \
        $(EXAMPLES_FILES)  \
        config-win32.h     \
        Makefile.win32     \
        vdsf.dsw           \
        $(SPARC_FILES)     \
        $(X86_64_FILES)    \
        $(PACKAGES_FILES)  \
        XML/wd_config.xsd  

install-data-local:
	-mkdir -p $(DESTDIR)$(datadir)/doc/vdsf/Examples
	for f in $(EXAMPLES_FILES); do \
		$(install_sh_DATA) $(top_srcdir)/src/$$f $(DESTDIR)$(datadir)/doc/vdsf/$$f ; \
	done

uninstall-local:
	-for f in $(EXAMPLES_FILES); do \
		rm -f $(DESTDIR)$(datadir)/doc/vdsf/$$f ; \
	done
	-rmdir $(DESTDIR)$(datadir)/doc/vdsf
	-rmdir $(DESTDIR)$(datadir)/doc

