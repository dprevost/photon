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

# The code have to be build following a certain order:
#   1-    Common
#   2-    BuildTools
#   3-    Engine
#   4-    API
#   n-    ...
#   Last- Tests

if COND_INCLUDE_DEMO
  MAYBE_DEMO = Demo
endif

SUBDIRS =               \
          Common        \
          BuildTools    \
          Engine        \
          API           \
          include       \
          java          \
          Watchdog      \
          CrashRecovery \
          Tests         \
          $(MAYBE_DEMO)
