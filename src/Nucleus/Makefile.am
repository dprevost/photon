# Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# Include this at the beginning to allow the current Makefile.am to
# overwrite any macros set in the common file
include $(top_srcdir)/src/Makefile.Common.am

CLEANFILES = *~ 

SUBDIRS = . Tests
 
noinst_LTLIBRARIES   = libNucleus.la

# Trick: keeping psoErrorHandler.c as the first file forces make to
# generate it (and the .h) before trying to build the rest. Otherwise
# there was a bit of an issue (after doing a make distclean, which 
# remove the dependency files - the dependencies could have been written
# by hand but... this would defeat the whole idea of automated tools!).

libNucleus_la_SOURCES =   \
        psoErrorHandler.c \
        psoErrorHandler.h \
        BlockGroup.c      \
        BlockGroup.h      \
        DataType.h        \
        Engine.h          \
        FastMap.c         \
        FastMap.h         \
        Folder.c          \
        Folder.h          \
        HashCommon.h      \
        Hash.c            \
        Hash.h            \
        HashTx.c          \
        HashTx.h          \
        hash_fnv.c        \
        hash_fnv.h        \
        HashMap.c         \
        HashMap.h         \
        InitEngine.c      \
        InitEngine.h      \
        LinkedList.c      \
        LinkedList.h      \
        LinkedList.inl    \
        LinkNode.h        \
        LogFile.c         \
        LogFile.h         \
        MemBitmap.c       \
        MemBitmap.h       \
        MemBitmap.inl     \
        MemoryAllocator.c \
        MemoryAllocator.h \
        MemoryHeader.h    \
        MemoryObject.c    \
        MemoryObject.h    \
        Process.c         \
        Process.h         \
        ProcessManager.c  \
        ProcessManager.h  \
        Queue.c           \
        Queue.h           \
        SessionContext.h  \
        Session.c         \
        Session.h         \
        Transaction.c     \
        Transaction.h     \
        TransactionHash.h \
        TreeNode.h        \
        TxStatus.h        
   
EXTRA_DIST =                  \
        Nucleus.vs2005.vcproj \
        Nucleus.vs2008.vcproj \
        Makefile.win32        

