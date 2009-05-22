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

MAIN_JAVA_DIR = org/photon
JAVA_SERIAL= $(MAIN_JAVA_DIR)/serializer

CLEANFILES = \
        *~                                                \
        ./org/photon/*~                                   \
        $(top_builddir)/src/Java/org/photon/*.class       \
        ./org/photon/Tests/*~                             \
        $(top_builddir)/src/Java/org/photon/Tests/*.class 

Java_Source =                                    \
        $(JAVA_SERIAL)/ArrayListSerializer.java  \
        $(MAIN_JAVA_DIR)/BaseQueue.java          \
        $(MAIN_JAVA_DIR)/DataDefBuilderODBC.java \
        $(MAIN_JAVA_DIR)/DataDefBuilderUser.java \
        $(MAIN_JAVA_DIR)/DataDefinition.java     \
        $(MAIN_JAVA_DIR)/DefinitionType.java     \
        $(MAIN_JAVA_DIR)/FieldType.java          \
        $(MAIN_JAVA_DIR)/Folder.java             \
        $(MAIN_JAVA_DIR)/FolderEntry.java        \
        $(MAIN_JAVA_DIR)/Info.java               \
        $(JAVA_SERIAL)/JavaSerializer.java       \
        $(MAIN_JAVA_DIR)/KeyDefBuilderODBC.java  \
        $(MAIN_JAVA_DIR)/KeyDefBuilderUser.java  \
        $(MAIN_JAVA_DIR)/KeyDefinition.java      \
        $(MAIN_JAVA_DIR)/KeyType.java            \
        $(MAIN_JAVA_DIR)/ObjectDefinition.java   \
        $(MAIN_JAVA_DIR)/ObjectType.java         \
        $(MAIN_JAVA_DIR)/ObjectStatus.java       \
        $(MAIN_JAVA_DIR)/Photon.java             \
        $(MAIN_JAVA_DIR)/PhotonErrors.java       \
        $(MAIN_JAVA_DIR)/PhotonException.java    \
        $(JAVA_SERIAL)/PSOSerialize.java         \
        $(MAIN_JAVA_DIR)/Queue.java              \
        $(MAIN_JAVA_DIR)/Session.java            \
        $(MAIN_JAVA_DIR)/Tests/FolderTest.java   \
        $(MAIN_JAVA_DIR)/Tests/QueueTest.java    \
        $(MAIN_JAVA_DIR)/Tests/SessionTest.java  \
        $(MAIN_JAVA_DIR)/Tests/Test.java         

Java_Classes =                              \
        org/photon/DataDefBuilderODBC.class \
        org/photon/DataDefinition.class     \
        org/photon/DataRecord.class         \
        org/photon/FieldType.class          \
        org/photon/Folder.class             \
        org/photon/FolderEntry.class        \
        org/photon/KeyDefinition.class      \
        org/photon/KeyType.class            \
        org/photon/Info.java                \
        org/Photon/ObjectDefinition.class   \
        org/photon/ObjectType.class         \
        org/photon/ObjectStatus.class       \
        org/photon/Photon.class             \
        org/photon/PhotonErrors.class       \
        org/photon/PhotonException.class    \
        org/photon/Queue.class              \
        org/photon/Session.class            

org_photondir = $(top_builddir)/src/Java
JAVAROOT      = $(top_builddir)/src/Java
org_photon_JAVA = $(Java_Source)
AM_JAVACFLAGS = -Xlint:unchecked

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

libphoton_jni_la_SOURCES =               \
        jni_photon.h                     \
        jni_arrayListSerializer.c        \
        jni_baseQueue.c                  \
        jni_dataDefBuilderODBC.c         \
        jni_dataDefBuilderUser.c         \
        jni_dataDefinition.c             \
        jni_dataRecord.c                 \
        jni_fieldDef.c                   \
        jni_folder.c                     \
        jni_folderEntry.c                \
        jni_hashmap.c                    \
        jni_info.c                       \
        jni_keyDefBuilderODBC.c          \
        jni_keyDefBuilderUser.c          \
        jni_keyDefinition.c              \
        jni_keyType.c                    \
        jni_objDefinition.c              \
        jni_objectType.c                 \
        jni_objStatus.c                  \
        jni_photon.c                     \
        jni_session.c                    \
        org_photon_ArrayListSerializer.h \
        org_photon_BaseQueue.h           \
        org_photon_DataDefinition.h      \
        org_photon_DataRecord.h          \
        org_photon_FieldDefinition.h     \
        org_photon_Folder.h              \
        org_photon_FolderEntry.h         \
        org_photon_Info.h                \
        org_photon_KeyDefinition.h       \
        org_photon_KeyType.h             \
        org_photon_ObjectDefinition.h    \
        org_photon_ObjectType.h          \
        org_photon_ObjectStatus.h        \
        org_photon_Photon.h              \
        org_photon_Session.h             

libphoton_jni_la_LIBADD = \
   $(top_builddir)/src/API/libphoton.la 

