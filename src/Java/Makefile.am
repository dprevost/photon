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

CLEANFILES = *~ ./org/photon/*~  $(top_builddir)/src/Java/org/photon/*.class

Java_Source =                        \
        org/photon/PsoException.java \
        org/photon/PsoFolder.java    \
        org/photon/PsoProcess.java   \
        org/photon/PsoQueue.java     \
        org/photon/PsoSession.java   

Java_Classes =                        \
        org/photon/PsoException.class \
        org/photon/PsoFolder.class    \
        org/photon/PsoProcess.class   \
        org/photon/PsoQueue.class     \
        org/photon/PsoSession.class

org_photondir = $(top_builddir)/src/Java
JAVAROOT      = $(top_builddir)/src/Java
org_photon_JAVA = $(Java_Source)

EXTRA_DIST=$(Java_Source)

#all-local: $(Java_Classes)

lib_LTLIBRARIES = libphoton_jni.la

# See http://www.gnu.org/software/libtool/manual.html#SEC34
# for more info on version-info. 
# Hints:
#  - the version is not in synch with the version of the whole package
#  - format:  currentversion:revision:age  (age is the tricky one here)

libphoton_jni_la_LDFLAGS = -version-info 1:0:0
AM_CPPFLAGS += $(JNI_CPPFLAGS)

libphoton_jni_la_SOURCES = \
        jni_folder.c  \
        jni_hashmap.c \
        jni_process.c \
        jni_queue.c   \
        jni_session.c 

libphoton_jni_la_LIBADD = \
   $(top_builddir)/src/API/libphoton.la 

