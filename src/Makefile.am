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
#   1-    Common
#   2-    BuildTools
#   3-    Engine
#   4-    Watchdog
#   5-    API
#   n-    ...
#   Last- Tests

if COND_USE_JAVA
  MAYBE_Java = Java
endif

SUBDIRS =               \
          Common        \
          BuildTools    \
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
          
BASE_LOCK_FILES =                                  \
                  Locking/linux/autoconf.h         \
                  Locking/linux/compiler-gcc.h     \
                  Locking/linux/compiler.h         \
                  Locking/linux/compiler-intel.h   \
                  Locking/linux/linkage.h          \
                  Locking/linux/spinlock_api_smp.h \
                  Locking/linux/spinlock.h         \
                  Locking/linux/spinlock_types.h   

I386_FILES =                                   \
             Locking/asm-i386/alternative.h    \
             Locking/asm-i386/bitops.h         \
             Locking/asm-i386/cpufeature.h     \
             Locking/asm-i386/linkage.h        \
             Locking/asm-i386/spinlock.h       \
             Locking/asm-i386/spinlock_types.h \
             Locking/asm-i386/system.h         

EXAMPLES_FILES =                              \
                 Examples/iso_3166.tab        \
                 Examples/README.txt          \
                 Examples/C/iso_3166.h        \
                 Examples/C/HashMap.c         \
                 Examples/C/HashMapLoop.c     \
                 Examples/C/Makefile          \
                 Examples/C/Queue.h           \
                 Examples/C/QueueIn.c         \
                 Examples/C/QueueOut.c        \
                 Examples/C/QueueWork.c       \
                 Examples/C/README.txt        \
                 Examples/C/RunQueue.sh       \
                 Examples/C++/iso_3166.h      \
                 Examples/C++/HashMap.cpp     \
                 Examples/C++/HashMapLoop.cpp \
                 Examples/C++/Makefile        \
                 Examples/C++/Queue.h         \
                 Examples/C++/QueueIn.cpp     \
                 Examples/C++/QueueOut.cpp    \
                 Examples/C++/QueueWork.cpp   \
                 Examples/C++/README.txt      \
                 Examples/C++/RunQueue.sh     

EXTRA_DIST = $(BASE_LOCK_FILES) $(I386_FILES) $(EXAMPLES_FILES)

install-data-local:
	-mkdir -p $(DESTDIR)$(datadir)/doc/vdsf/Examples
	for f in $(EXAMPLES_FILES); do \
	     $(install_sh_DATA) $$f $(DESTDIR)$(datadir)/doc/vdsf/$$f ; \
	done

uninstall-local:
	for f in $(EXAMPLES_FILES); do \
     -rm -f $(DESTDIR)$(datadir)/doc/vdsf/$$f ; \
   done
	-rm -f $(DESTDIR)$(datadir)/doc/vdsf/refman.pdf
	-rmdir $(DESTDIR)$(datadir)/doc/vdsf
	-rmdir $(DESTDIR)$(datadir)/doc

