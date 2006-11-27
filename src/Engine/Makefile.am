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

libEngine_la_SOURCES =                   \
                       Engine.h          \
                       LinkedList.c      \
                       LinkedList.h      \
                       LinkedList.inl    \
                       LinkNode.h        \
                       ListErrors.h      \
                       LogFile.c         \
                       LogFile.h         \
                       Hash.c            \
                       Hash.h            \
                       MemoryAllocator.c \
                       MemoryAllocator.h \
                       MemBitmap.c       \
                       MemBitmap.h       \
                       MemBitmap.inl     \
                       MemoryObject.c    \
                       MemoryObject.h    \
                       PageGroup.c       \
                       PageGroup.h       \
                       SessionContext.h  \
                       VdsErrorHandler.c \
                       VdsErrorHandler.h
   
#   MemoryManager.c         \
   MemoryManager.h         

#  Queue.cpp               \
   Queue.h                 \
   Transaction.cpp         \
   Transaction.h           \
   TransactionBase.h       \
   TransactionManager.cpp  \
   TransactionManager.h    \
   TreeManager.cpp         \
   TreeManager.h 

#   BaseNode.cpp            \
   BaseNode.h              \
   BaseObject.h            \
   CleanupManager.cpp      \
   CleanupManager.h        \
   CleanupProcess.cpp      \
   CleanupProcess.h        \
   CleanupSession.cpp      \
   CleanupSession.h        \
   Folder.cpp              \
   Folder.h                \
   HashList.cpp            \
   HashList.h              
   
Engine.h: VdsErrorHandler.h

VdsErrorHandler.c VdsErrorHandler.h:
	$(top_builddir)/src/BuildTools/errorParser -i $(top_srcdir)/src/include/vdsf/vdsErrors.h -o VdsErrorHandler -p vdseErr

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
