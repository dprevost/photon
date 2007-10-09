# Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~

# The code have to be build following a certain order:
#   1-    Locking (???)
#   2-    Common
#   3-    BuildTools
#   4-    Engine
#   5-    API
#   n-    ...
#   Last- Tests

SUBDIRS =               \
          Locking       \
          Common        \
          BuildTools    \
          Engine        \
          API           \
          include       \
          Watchdog      \
          CrashRecovery \
          Tests         

if COND_USE_JAVA
   SUBDIRS += Java
endif

if COND_INCLUDE_DEMO
  SUBDIRS += Demo
endif


DIST_SUBDIRS = $(SUBDIRS) Java Demo Locking      

