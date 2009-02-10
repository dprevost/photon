/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <jni.h>
#include <photon/photon.h>
#include <string.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderCreateObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonFolder_folderCreateObject( JNIEnv * env, 
                                                 jobject  obj, 
                                                 jlong    h, 
                                                 jstring  jname, 
                                                 jobject  def )
{
   int errcode;
   jclass myClass;
   size_t handle = (size_t)h;
   const char *objectName;
   jfieldID id;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   
   myClass = (*env)->FindClass( env, "org/photon/ObjectDefinition" );

   id = (*env)->GetFieldID( env, myClass, "handle", "J" );

//   errcode = psoFolderCreateObject( (PSO_HANDLE) handle,
//                                    objectName,
//                                    strlen(objectName),
//                           psoBasicObjectDef * pDefinition,
//                           psoFieldDefinition  * pFields );

   (*env)->ReleaseStringUTFChars( env, jname, objectName );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderCreateObjectXML
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonFolder_folderCreateObjectXML( JNIEnv * env, 
                                                    jobject  obj, 
                                                    jlong    h, 
                                                    jstring  jxml )
{
   int errcode;
   size_t handle = (size_t)h;
   const char *xmlDef;
   
   xmlDef = (*env)->GetStringUTFChars( env, jxml, NULL );
   if ( xmlDef == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   
   errcode = psoFolderCreateObjectXML( (PSO_HANDLE) handle,
                                       xmlDef,
                                       strlen(xmlDef) );

   (*env)->ReleaseStringUTFChars( env, jxml, xmlDef );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderDestroyObject
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonFolder_folderDestroyObject( JNIEnv * env, 
                                                  jobject  obj, 
                                                  jlong    h, 
                                                  jstring  jname )
{
   int errcode;
   size_t handle = (size_t)h;
   const char * objectName;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   
   errcode = psoFolderDestroyObject( (PSO_HANDLE) handle,
                                     objectName,
                                    strlen(objectName) );

   (*env)->ReleaseStringUTFChars( env, jname, objectName );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderFini
 * Signature: (J)V
 */
JNIEXPORT void JNICALL 
Java_org_photon_PhotonFolder_folderFini( JNIEnv * env, 
                                         jobject  obj, 
                                         jlong    h )
{
   int errcode;
   size_t handle = (size_t) h;
   
   errcode = psoFolderClose( (PSO_HANDLE)handle );

   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderGetFirst
 * Signature: (JLorg/photon/FolderEntry;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonFolder_folderGetFirst( JNIEnv * env, 
                                             jobject  obj, 
                                             jlong    h,
                                             jobject  entry )
{
   int errcode;
   size_t handle = (size_t) h;
   psoFolderEntry cEntry;
   jclass myClass, exc;
   jfieldID idType, idName, idStatus;
   jstring jname;
   
   myClass = (*env)->FindClass( env, "org/photon/FolderEntry" );
   if ( myClass == NULL ) return PSO_INTERNAL_ERROR;

   idType   = (*env)->GetFieldID( env, myClass, "type", "I" );
   if ( idType == NULL ) return PSO_INTERNAL_ERROR;
   idName   = (*env)->GetFieldID( env, myClass, "name", "Ljava/lang/String;" );
   if ( idName == NULL ) return PSO_INTERNAL_ERROR;
   idStatus = (*env)->GetFieldID( env, myClass, "status", "I" );
   if ( idStatus == NULL ) return PSO_INTERNAL_ERROR;

   errcode = psoFolderGetFirst( (PSO_HANDLE)handle, &cEntry );
   if ( errcode == PSO_OK ) {
      if ( cEntry.nameLengthInBytes == PSO_MAX_NAME_LENGTH ) {
         /* Special case - not null terminated string */
         char * s = malloc( PSO_MAX_NAME_LENGTH+1);
         if ( s == NULL ) {
            exc = (*env)->FindClass(env, "java/lang/OutOfMemoryError");
            if ( exc != NULL ) {
               (*env)->ThrowNew( env, exc, "malloc failed in jni code");
            }
            return PSO_NOT_ENOUGH_HEAP_MEMORY;
         }
         memcpy( s, cEntry.name, PSO_MAX_NAME_LENGTH );
         s[PSO_MAX_NAME_LENGTH] = 0;
         jname = (*env)->NewStringUTF( env, s );
         free(s);
      }
      else {
         jname = (*env)->NewStringUTF( env, cEntry.name );
      }
      if ( jname == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      
      (*env)->SetObjectField( env, entry, idName,   jname );
      (*env)->SetIntField(    env, entry, idType,   cEntry.type );
      (*env)->SetIntField(    env, entry, idStatus, cEntry.status );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderGetNext
 * Signature: (JLorg/photon/FolderEntry;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonFolder_folderGetNext( JNIEnv * env, 
                                            jobject  obj, 
                                            jlong    h,
                                            jobject  entry )
{
   int errcode;
   size_t handle = (size_t) h;
   psoFolderEntry cEntry;
   jclass myClass, exc;
   jfieldID idType, idName, idStatus;
   jstring jname;
   
   myClass = (*env)->FindClass( env, "org/photon/FolderEntry" );
   if ( myClass == NULL ) return PSO_INTERNAL_ERROR;

   idType   = (*env)->GetFieldID( env, myClass, "type", "I" );
   if ( idType == NULL ) return PSO_INTERNAL_ERROR;
   idName   = (*env)->GetFieldID( env, myClass, "name", "Ljava/lang/String;" );
   if ( idName == NULL ) return PSO_INTERNAL_ERROR;
   idStatus = (*env)->GetFieldID( env, myClass, "status", "I" );
   if ( idStatus == NULL ) return PSO_INTERNAL_ERROR;

   errcode = psoFolderGetNext( (PSO_HANDLE)handle, &cEntry );
   if ( errcode == PSO_OK ) {
      if ( cEntry.nameLengthInBytes == PSO_MAX_NAME_LENGTH ) {
         /* Special case - not null terminated string */
         char * s = malloc( PSO_MAX_NAME_LENGTH+1);
         if ( s == NULL ) {
            exc = (*env)->FindClass(env, "java/lang/OutOfMemoryError");
            if ( exc != NULL ) {
               (*env)->ThrowNew( env, exc, "malloc failed in jni code");
            }
            return PSO_NOT_ENOUGH_HEAP_MEMORY;
         }
         memcpy( s, cEntry.name, PSO_MAX_NAME_LENGTH );
         s[PSO_MAX_NAME_LENGTH] = 0;
         jname = (*env)->NewStringUTF( env, s );
         free(s);
      }
      else {
         jname = (*env)->NewStringUTF( env, cEntry.name );
      }
      if ( jname == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      
      (*env)->SetObjectField( env, entry, idName,   jname );
      (*env)->SetIntField(    env, entry, idType,   cEntry.type );
      (*env)->SetIntField(    env, entry, idStatus, cEntry.status );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderInit
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonFolder_folderInit( JNIEnv * env, 
                                         jobject  obj, 
                                         jlong    h, 
                                         jstring  jstr )
{
   int errcode;
   jclass myClass;
   PSO_HANDLE handle;
   size_t sessionHandle = (size_t)h;
   const char *folderName;
   jfieldID id;
   
   folderName = (*env)->GetStringUTFChars( env, jstr, NULL );
   if ( folderName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   
   myClass = (*env)->FindClass( env, "org/photon/PhotonFolder" );

   id = (*env)->GetFieldID( env, myClass, "handle", "J" );

   errcode = psoFolderOpen( (PSO_HANDLE) sessionHandle,
                            folderName,
                            strlen(folderName),
                            &handle );
   (*env)->ReleaseStringUTFChars( env, jstr, folderName );

   if ( errcode == PSO_OK ) {
      (*env)->SetLongField( env, obj, id, (size_t) handle );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderStatus
 * Signature: (JLorg/photon/ObjStatus;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonFolder_folderStatus( JNIEnv * env, 
                                           jobject  obj, 
                                           jlong    h, 
                                           jobject  status )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

