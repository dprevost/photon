# Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

SUBDIRS =               \
          Common        \
          BuildTools    \
          Engine        \
          Watchdog      \
          API           \
          include       \
          Tests
          
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

EXAMPLES_FILES =                        \
                 Examples/iso_3166.h    \
                 Examples/iso_3166.tab  \
                 Examples/HashMap.c     \
                 Examples/HashMapLoop.c \
                 Examples/Makefile      \
                 Examples/Queue.h       \
                 Examples/QueueIn.c     \
                 Examples/QueueOut.c    \
                 Examples/QueueWork.c   \
                 Examples/README        

EXTRA_DIST = $(BASE_LOCK_FILES) $(I386_FILES) $(EXAMPLES_FILES)

