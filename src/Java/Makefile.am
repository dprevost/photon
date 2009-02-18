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

CLEANFILES = \
        *~                                                \
        ./org/photon/*~                                   \
        $(top_builddir)/src/Java/org/photon/*.class       \
        ./org/photon/Tests/*~                             \
        $(top_builddir)/src/Java/org/photon/Tests/*.class 

Java_Source =                             \
        org/photon/DataRecord.java        \
        org/photon/Definition.java        \
        org/photon/FieldDefinition.java   \
        org/photon/FieldType.java         \
        org/photon/Folder.java            \
        org/photon/FolderEntry.java       \
        org/photon/KeyDefinition.java     \
        org/photon/KeyType.java           \
        org/photon/Info.java              \
        org/photon/ObjectDefinition.java  \
        org/photon/ObjectType.java        \
        org/photon/ObjectStatus.java      \
        org/photon/Photon.java            \
        org/photon/PhotonErrors.java      \
        org/photon/PhotonException.java   \
        org/photon/Queue.java             \
        org/photon/Session.java           \
        org/photon/Tests/FolderTest.java  \
        org/photon/Tests/QueueTest.java   \
        org/photon/Tests/SessionTest.java \
        org/photon/Tests/Test.java        

Java_Classes =                             \
        org/photon/DataRecord.class        \
        org/photon/Definition.class        \
        org/photon/FieldDefinition.class   \
        org/photon/FieldType.class         \
        org/photon/Folder.class            \
        org/photon/FolderEntry.class       \
        org/photon/KeyDefinition.class     \
        org/photon/KeyType.class           \
        org/photon/Info.java               \
        org/Photon/ObjectDefinition.class  \
        org/photon/ObjectType.class        \
        org/photon/ObjectStatus.class      \
        org/photon/Photon.class            \
        org/photon/PhotonErrors.class      \
        org/photon/PhotonException.class   \
        org/photon/Queue.class             \
        org/photon/Session.class           

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

# javah org.photon.FieldType

libphoton_jni_la_SOURCES =            \
        jni_photon.h                  \
        jni_dataRecord.c              \
        jni_definition.c              \
        jni_fieldDef.c                \
        jni_fieldType.c               \
        jni_folder.c                  \
        jni_folderEntry.c             \
        jni_hashmap.c                 \
        jni_info.c                    \
        jni_keyDefinition.c           \
        jni_keyType.c                 \
        jni_objDefinition.c           \
        jni_objectType.c              \
        jni_objStatus.c               \
        jni_photon.c                  \
        jni_queue.c                   \
        jni_session.c                 \
        org_photon_DataRecord.h       \
        org_photon_Definition.h       \
        org_photon_FieldDefinition.h  \
        org_photon_FieldType.h        \
        org_photon_Folder.h           \
        org_photon_FolderEntry.h      \
        org_photon_Info.h             \
        org_photon_KeyDefinition.h    \
        org_photon_KeyType.h          \
        org_photon_ObjectDefinition.h \
        org_photon_ObjectType.h       \
        org_photon_ObjectStatus.h     \
        org_photon_Photon.h           \
        org_photon_Queue.h            \
        org_photon_Session.h          

libphoton_jni_la_LIBADD = \
   $(top_builddir)/src/API/libphoton.la 

