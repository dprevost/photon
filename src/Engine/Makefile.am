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

SUBDIRS = . Tests
 
noinst_LTLIBRARIES   = libEngine.la

libEngine_la_SOURCES =     \
   BaseNode.cpp            \
   BaseNode.h              \
   BaseObject.h            \
   CleanupManager.cpp      \
   CleanupManager.h        \
   CleanupProcess.cpp      \
   CleanupProcess.h        \
   CleanupSession.cpp      \
   CleanupSession.h        \
   Engine.h                \
   Folder.cpp              \
   Folder.h                \
   HashList.cpp            \
   HashList.h              \
   LinkedList.c            \
   LinkedList.h            \
   LinkedList.inl          \
   LinkNode.h              \
   ListErrors.h            \
   LogFile.c               \
   LogFile.h               \
   MemoryAllocator.c       \
   MemoryAllocator.h       \
   MemoryManager.cpp       \
   MemoryManager.h         \
   Queue.cpp               \
   Queue.h                 \
   SessionContext.h        \
   Transaction.cpp         \
   Transaction.h           \
   TransactionBase.h       \
   TransactionManager.cpp  \
   TransactionManager.h    \
   TreeManager.cpp         \
   TreeManager.h 

AM_CPPFLAGS = -I$(top_srcdir)/src/Common  \
              -I$(top_srcdir)/src/API     \
              -I$(top_srcdir)/src/include \
              -Wall

if COND_USE_ASSERT
   AM_CPPFLAGS += -DUSE_ASSERT
endif
if COND_USE_TRAP_ERRORS
   AM_CPPFLAGS += -DUSE_TRAP_ERRORS
endif

if COND_USE_DBC
   AM_CPPFLAGS += -DUSE_DBC
endif
if COND_USE_EXTREME_DBC
   AM_CPPFLAGS += -DUSE_EXTREME_DBC
endif
