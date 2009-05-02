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

#include "jni_photon.h"
#include "org_photon_Folder.h"
#include "API/DataDefinition.h"
#include "API/KeyDefinition.h"

jfieldID g_idFolderHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_Folder_initIDs( JNIEnv * env, jclass folderClass )
{
   g_idFolderHandle = (*env)->GetFieldID( env, folderClass, "handle", "J" );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoClose
 * Signature: (J)V
 */
JNIEXPORT void JNICALL 
Java_org_photon_Folder_psoClose( JNIEnv * env, 
                                 jobject  jobj, 
                                 jlong    jhandle )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   
   errcode = psoFolderClose( (PSO_HANDLE)handle );

   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoCreateFolder
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Folder_psoCreateFolder( JNIEnv  * env, 
                                        jobject   jobj,
                                        jlong     jhandle,
                                        jstring   jname )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   const char * name;

   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoFolderCreateFolder( (PSO_HANDLE) handle, 
                                    name,
                                    strlen(name) );

   (*env)->ReleaseStringUTFChars( env, jname, name );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoCreateObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;Lorg/photon/DataDefinition;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Folder_psoCreateObject( JNIEnv   * env,
                                        jobject    jobj,
                                        jlong      jhandle,
                                        jstring    jname, 
                                        jobject    jdefinition,
                                        jobject    jdataDef )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   const char * name;
   psoObjectDefinition definition;
   size_t dataDefHandle = 0;
   
   definition.type  = (*env)->GetIntField( env, jdefinition, g_idObjDefType );
   definition.flags = (*env)->GetIntField( env, jdefinition, g_idObjDefFlags );
   
   definition.minNumOfDataRecords = 
      (*env)->GetIntField( env, jdefinition, g_idObjDefMinNumOfDataRecords );
   definition.minNumBlocks = 
      (*env)->GetIntField( env, jdefinition, g_idObjDefMinNumBlocks );
   
   dataDefHandle = (*env)->GetLongField( env, jdataDef, g_idDataDefHandle );

   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoFolderCreateObject( (PSO_HANDLE) handle,
                                    name,
                                    strlen(name),
                                    &definition,
                                    (PSO_HANDLE) dataDefHandle );

   (*env)->ReleaseStringUTFChars( env, jname, name );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoCreateObjectEx
 * Signature: (JJLjava/lang/String;Lorg/photon/ObjectDefinition;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Folder_psoCreateObjectEx( JNIEnv * env,
                                          jobject  jobj,
                                          jlong    jhandle,
                                          jlong    jsessionHandle,
                                          jstring  jname,
                                          jobject  jdefinition,
                                          jstring  jdataDefName )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   size_t sessionHandle = (size_t) jsessionHandle;
   PSO_HANDLE dataDefHandle = NULL;
   const char * name;
   psoObjectDefinition definition;
   const char * dataDefName;
   
   definition.type  = (*env)->GetIntField( env, jdefinition, g_idObjDefType );
   definition.flags = (*env)->GetIntField( env, jdefinition, g_idObjDefFlags );
   definition.minNumOfDataRecords = (size_t) (*env)->GetLongField( env,
      jdefinition, g_idObjDefMinNumOfDataRecords );
   definition.minNumBlocks = (size_t) (*env)->GetLongField( env,
      jdefinition, g_idObjDefMinNumBlocks );

   dataDefName = (*env)->GetStringUTFChars( env, jdataDefName, NULL );
   if ( dataDefName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoDataDefOpen( (PSO_HANDLE)sessionHandle,
                             dataDefName,
                             strlen(dataDefName),
                             &dataDefHandle );
   (*env)->ReleaseStringUTFChars( env, jdataDefName, dataDefName );
   if ( errcode == 0 ) {
   
      name = (*env)->GetStringUTFChars( env, jname, NULL );
      if ( name == NULL ) {
         psoDataDefClose( dataDefHandle );
         return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
      }

      errcode = psoFolderCreateObject( (PSO_HANDLE) handle,
                                       name,
                                       strlen(name),
                                       &definition,
                                       (PSO_HANDLE)dataDefHandle );
   
      (*env)->ReleaseStringUTFChars( env, jname, name );
      psoDataDefClose( dataDefHandle );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoCreateKeyedObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;Lorg/photon/KeyDefinition;Lorg/photon/DataDefinition;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Folder_psoCreateKeyedObject( JNIEnv     * env,
                                             jobject      jobj,
                                             jlong        jhandle,
                                             jstring      jname, 
                                             jobject      jdefinition,
                                             jobject      jkeyDef,
                                             jobjectArray jdataDef )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   const char * name;
   psoObjectDefinition definition;
   size_t dataDefHandle = 0, keyDefHandle = 0;
   
   definition.type  = (*env)->GetIntField( env, jdefinition, g_idObjDefType );
   definition.flags = (*env)->GetIntField( env, jdefinition, g_idObjDefFlags );
   
   definition.minNumOfDataRecords = 
      (*env)->GetIntField( env, jdefinition, g_idObjDefMinNumOfDataRecords );
   definition.minNumBlocks = 
      (*env)->GetIntField( env, jdefinition, g_idObjDefMinNumBlocks );
   
   dataDefHandle = (*env)->GetLongField( env, jdataDef, g_idDataDefHandle );
   keyDefHandle  = (*env)->GetLongField( env, jkeyDef,  g_idKeyDefHandle );

   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoFolderCreateObject( (PSO_HANDLE) handle,
                                    name,
                                    strlen(name),
                                    &definition,
                                    (PSO_HANDLE) dataDefHandle );

   (*env)->ReleaseStringUTFChars( env, jname, name );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoCreateKeyedObjectEx
 * Signature: (JJLjava/lang/String;Lorg/photon/ObjectDefinition;Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Folder_psoCreateKeyedObjectEx( JNIEnv * env,
                                               jobject  jobj,
                                               jlong    jhandle,
                                               jlong    jsessionHandle,
                                               jstring  jname,
                                               jobject  jdefinition,
                                               jstring  jkeyDefName,
                                               jstring  jdataDefName )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   size_t sessionHandle = (size_t) jsessionHandle;
   PSO_HANDLE dataDefHandle = NULL, keyDefHandle = NULL;
   const char * name;
   psoObjectDefinition definition;
   const char * dataDefName, * keyDefName;
   
   definition.type  = (*env)->GetIntField( env, jdefinition, g_idObjDefType );
   definition.flags = (*env)->GetIntField( env, jdefinition, g_idObjDefFlags );
   definition.minNumOfDataRecords = (size_t) (*env)->GetLongField( env,
      jdefinition, g_idObjDefMinNumOfDataRecords );
   definition.minNumBlocks = (size_t) (*env)->GetLongField( env,
      jdefinition, g_idObjDefMinNumBlocks );

   dataDefName = (*env)->GetStringUTFChars( env, jdataDefName, NULL );
   if ( dataDefName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoDataDefOpen( (PSO_HANDLE)sessionHandle,
                             dataDefName,
                             strlen(dataDefName),
                             &dataDefHandle );
   (*env)->ReleaseStringUTFChars( env, jdataDefName, dataDefName );
   if ( errcode != 0 ) return errcode;
   
   keyDefName = (*env)->GetStringUTFChars( env, jkeyDefName, NULL );
   if ( keyDefName == NULL ) {
      psoDataDefClose( dataDefHandle );
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoKeyDefOpen( (PSO_HANDLE)sessionHandle,
                            keyDefName,
                            strlen(keyDefName),
                            &keyDefHandle );
   (*env)->ReleaseStringUTFChars( env, jkeyDefName, keyDefName );
   if ( errcode != 0 ) {
      psoDataDefClose( dataDefHandle );
      return errcode;
   }
   
   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      psoDataDefClose( dataDefHandle );
      psoKeyDefClose( keyDefHandle );
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoFolderCreateKeyedObject( (PSO_HANDLE)handle,
                                         name,
                                         strlen(name),
                                         &definition,
                                         dataDefHandle,
                                         keyDefHandle );
   
   (*env)->ReleaseStringUTFChars( env, jname, name );
   psoDataDefClose( dataDefHandle );
   psoKeyDefClose( keyDefHandle );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoDataDefinition
 * Signature: (JLjava/lang/String;Lorg/photon/DataDefinition;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Folder_psoDataDefinition( JNIEnv * env,
                                          jobject  jobj,
                                          jlong    jhandle,
                                          jstring  jname,
                                          jobject  jdefinition )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   const char * objectName;
   PSO_HANDLE dataDefHandle = NULL;
   enum psoDefinitionType  defType;
   unsigned char * dataDef;
   unsigned int  dataDefLength; 
   jbyteArray jba;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoFolderGetDataDefinition( (PSO_HANDLE) handle,
                                         objectName,
                                         strlen(objectName),
                                         &dataDefHandle );
   (*env)->ReleaseStringUTFChars( env, jname, objectName );

   if ( errcode == 0 ) {
      errcode = psoaDataDefGetDef( dataDefHandle,
                                   &defType,
                                   (unsigned char **)&dataDef,
                                   &dataDefLength );
      if ( errcode != 0 ) {
         psoDataDefClose( dataDefHandle );
         return errcode;
      }
   
      jba = (*env)->NewByteArray( env, dataDefLength );
      if ( jba == NULL ) {
         psoDataDefClose( dataDefHandle );
         return PSO_NOT_ENOUGH_HEAP_MEMORY;
      }
   
      (*env)->SetByteArrayRegion( env, jba, 0, dataDefLength, (jbyte *)dataDef );

      (*env)->SetObjectField( env, jdefinition, g_idDataDefDataDef, jba );
      (*env)->SetIntField(    env, jdefinition, g_idDataDefType,    defType );
      (*env)->SetLongField(   env, jdefinition, g_idDataDefHandle,  (size_t)dataDefHandle );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoDefinition
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Folder_psoDefinition( JNIEnv * env,
                                      jobject  jobj,
                                      jlong    jhandle,
                                      jstring  jname,
                                      jobject  jdefinition )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   const char * objectName;
   psoObjectDefinition objDefinition;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoFolderGetDefinition( (PSO_HANDLE) handle,
                                     objectName,
                                     strlen(objectName),
                                     &objDefinition );
   (*env)->ReleaseStringUTFChars( env, jname, objectName );

   if ( errcode == 0 ) {
      (*env)->SetIntField( env, jdefinition,  g_idObjDefType,
         objDefinition.type );
      (*env)->SetIntField( env, jdefinition,  g_idObjDefFlags,
         objDefinition.flags );
      (*env)->SetLongField( env, jdefinition, g_idObjDefMinNumOfDataRecords,
         objDefinition.minNumOfDataRecords );
      (*env)->SetLongField( env, jdefinition, g_idObjDefMinNumBlocks,
         objDefinition.minNumBlocks );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoDestroyObject
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Folder_psoDestroyObject( JNIEnv * env, 
                                         jobject  jobj, 
                                         jlong    jhandle, 
                                         jstring  jname )
{
   int errcode;
   size_t handle = (size_t) jhandle;
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
 * Class:     org_photon_Folder
 * Method:    psoGetFirst
 * Signature: (JLorg/photon/FolderEntry;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Folder_psoGetFirst( JNIEnv * env, 
                                    jobject  jobj, 
                                    jlong    jhandle,
                                    jobject  jentry )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   psoFolderEntry cEntry;
   jstring jname;
   
   errcode = psoFolderGetFirst( (PSO_HANDLE)handle, &cEntry );
   if ( errcode == PSO_OK ) {
      jname = getNotNullTerminatedString( env, 
         cEntry.name, PSO_MAX_NAME_LENGTH );
      if ( jname == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;

      (*env)->SetObjectField( env, jentry, g_idEntryName,   jname );
      (*env)->SetIntField( env, jentry, g_idEntryType,   cEntry.type );
      (*env)->SetIntField( env, jentry, g_idEntryStatus, cEntry.status );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoGetNext
 * Signature: (JLorg/photon/FolderEntry;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Folder_psoGetNext( JNIEnv * env, 
                                   jobject  jobj, 
                                   jlong    jhandle,
                                   jobject  jentry )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   psoFolderEntry cEntry;
   jstring jname;
   
   errcode = psoFolderGetNext( (PSO_HANDLE)handle, &cEntry );
   if ( errcode == PSO_OK ) {
      jname = getNotNullTerminatedString( env, 
         cEntry.name, PSO_MAX_NAME_LENGTH );
      if ( jname == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      
      (*env)->SetObjectField( env, jentry, g_idEntryName,   jname );
      (*env)->SetIntField( env, jentry, g_idEntryType,   cEntry.type );
      (*env)->SetIntField( env, jentry, g_idEntryStatus, cEntry.status );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoKeyDefinition
 * Signature: (JLjava/lang/String;Lorg/photon/KeyDefinition;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Folder_psoKeyDefinition( JNIEnv * env,
                                         jobject  jobj,
                                         jlong    jhandle,
                                         jstring  jname,
                                         jobject  jdefinition )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   const char * objectName;
   PSO_HANDLE keyDefHandle = NULL;
   enum psoDefinitionType  defType;
   unsigned char * keyDef;
   unsigned int  keyDefLength; 
   jbyteArray jba;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoFolderGetKeyDefinition( (PSO_HANDLE) handle,
                                        objectName,
                                        strlen(objectName),
                                        &keyDefHandle );
   (*env)->ReleaseStringUTFChars( env, jname, objectName );

   if ( errcode == 0 ) {
      errcode = psoaKeyDefGetDef( keyDefHandle,
                                  &defType,
                                  (unsigned char **)&keyDef,
                                  &keyDefLength );
      if ( errcode != 0 ) {
         psoKeyDefClose( keyDefHandle );
         return errcode;
      }
   
      jba = (*env)->NewByteArray( env, keyDefLength );
      if ( jba == NULL ) {
         psoKeyDefClose( keyDefHandle );
         return PSO_NOT_ENOUGH_HEAP_MEMORY;
      }
   
      (*env)->SetByteArrayRegion( env, jba, 0, keyDefLength, (jbyte *)keyDef );

      (*env)->SetObjectField( env, jdefinition, g_idKeyDefKeyDef, jba );
      (*env)->SetIntField(    env, jdefinition, g_idKeyDefType,   defType );
      (*env)->SetLongField(   env, jdefinition, g_idKeyDefHandle, (size_t)keyDefHandle );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoOpen
 * Signature: (Lorg/photon/Session;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Folder_psoOpen( JNIEnv * env, 
                                jobject  jobj, 
                                jobject  jsession, 
                                jstring  jstr )
{
   int errcode;
   PSO_HANDLE handle;
   size_t sessionHandle;
   const char *folderName;

   folderName = (*env)->GetStringUTFChars( env, jstr, NULL );
   if ( folderName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   sessionHandle = (*env)->GetLongField( env, jsession, g_idSessionHandle );
   
   errcode = psoFolderOpen( (PSO_HANDLE) sessionHandle,
                            folderName,
                            strlen(folderName),
                            &handle );
   (*env)->ReleaseStringUTFChars( env, jstr, folderName );

   if ( errcode == PSO_OK ) {
      (*env)->SetLongField( env, jobj, g_idFolderHandle, (size_t) handle );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoStatus
 * Signature: (JLorg/photon/ObjectStatus;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Folder_psoStatus( JNIEnv * env, 
                                  jobject  jobj, 
                                  jlong    jhandle, 
                                  jobject  jstatus )
{
   int errcode;
   size_t handle = (size_t)jhandle;
   psoObjStatus status;

   errcode = psoFolderStatus( (PSO_HANDLE) handle, &status );

   if ( errcode == 0 ) {
      (*env)->SetIntField(  env, jstatus, g_idStatusType,          status.type );
      (*env)->SetIntField(  env, jstatus, g_idStatusStatus,        status.status );
      (*env)->SetLongField( env, jstatus, g_idStatusNumBlocks,     status.numBlocks );
      (*env)->SetLongField( env, jstatus, g_idStatusNumBlockGroup, status.numBlockGroup );
      (*env)->SetLongField( env, jstatus, g_idStatusNumDataItem,   status.numDataItem );
      (*env)->SetLongField( env, jstatus, g_idStatusFreeBytes,     status.freeBytes );
      (*env)->SetIntField(  env, jstatus, g_idStatusMaxDataLength, status.maxDataLength );
      (*env)->SetIntField(  env, jstatus, g_idStatusMaxKeyLength,  status.maxKeyLength );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

