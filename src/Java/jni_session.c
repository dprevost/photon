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
#include "org_photon_Session.h"

jfieldID g_idSessionHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_Session_initIDs( JNIEnv * env, jclass sessionClass )
{
   g_idSessionHandle = (*env)->GetFieldID( env, sessionClass, "handle", "J" );
   if ( g_idSessionHandle == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoCommit
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Session_psoCommit( JNIEnv  * env, 
                                   jobject   jobj, 
                                   jlong     jhandle )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   
   errcode = psoCommit( (PSO_HANDLE)handle );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoCreateFolder
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Session_psoCreateFolder( JNIEnv * env,
                                         jobject  jobj,
                                         jlong    jhandle,
                                         jstring  jname )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   const char * name;

   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoCreateFolder( (PSO_HANDLE) handle,
                              name,
                              strlen(name) );

   (*env)->ReleaseStringUTFChars( env, jname, name );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoCreateObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;J)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Session_psoCreateObject( JNIEnv * env,
                                         jobject  jobj,
                                         jlong    jhandle,
                                         jstring  jname,
                                         jobject  jdefinition,
                                         jlong    jdataDefHandle )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   size_t dataDefHandle = (size_t) jdataDefHandle;
   const char * name;
   psoObjectDefinition definition;
   
   /* jni variables needed to access the jvm data */
   jobject jTypeObj;
   
   /*
    * Note: types are usually set using an enum. So we must extract
    * the enum object first before we can access the int field.
    */
   jTypeObj = (*env)->GetObjectField( env, jdefinition, g_idObjDefType );
   definition.type = (*env)->GetIntField( env, jTypeObj, g_idObjTypeType );
   (*env)->DeleteLocalRef( env, jTypeObj );

   definition.flags = (*env)->GetIntField( env, jdefinition, g_idObjDefFlags );
   definition.minNumOfDataRecords = (size_t) (*env)->GetLongField( env,
      jdefinition, g_idObjDefMinNumOfDataRecords );
   definition.minNumBlocks = (size_t) (*env)->GetLongField( env,
      jdefinition, g_idObjDefMinNumBlocks );
   
   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoCreateObject( (PSO_HANDLE) handle,
                              name,
                              strlen(name),
                              &definition,
                              (PSO_HANDLE)dataDefHandle );
   
   (*env)->ReleaseStringUTFChars( env, jname, name );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/*
 * Class:     org_photon_Session
 * Method:    psoCreateObjectEx
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Session_psoCreateObjectEx( JNIEnv * env,
                                           jobject  jobj,
                                           jlong    jhandle,
                                           jstring  jname,
                                           jobject  jdefinition,
                                           jstring  jdataDefName )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   PSO_HANDLE dataDefHandle = NULL;
   const char * name;
   psoObjectDefinition definition;
   const char * dataDefName;
   
   /* jni variables needed to access the jvm data */
   jobject jTypeObj;
   
   /*
    * Note: types are usually set using an enum. So we must extract
    * the enum object first before we can access the int field.
    */
   jTypeObj = (*env)->GetObjectField( env, jdefinition, g_idObjDefType );
   definition.type = (*env)->GetIntField( env, jTypeObj, g_idObjTypeType );
   (*env)->DeleteLocalRef( env, jTypeObj );

   definition.flags = (*env)->GetIntField( env, jdefinition, g_idObjDefFlags );
   definition.minNumOfDataRecords = (size_t) (*env)->GetLongField( env,
      jdefinition, g_idObjDefMinNumOfDataRecords );
   definition.minNumBlocks = (size_t) (*env)->GetLongField( env,
      jdefinition, g_idObjDefMinNumBlocks );

   dataDefName = (*env)->GetStringUTFChars( env, jdataDefName, NULL );
   if ( dataDefName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoDataDefOpen( (PSO_HANDLE)handle,
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

      errcode = psoCreateObject( (PSO_HANDLE) handle,
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
 * Class:     org_photon_Session
 * Method:    psoCreateKeyedObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;JJ)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Session_psoCreateKeyedObject( JNIEnv * env,
                                              jobject  jobj,
                                              jlong    jhandle,
                                              jstring  jname,
                                              jobject  jdefinition,
                                              jlong    jdataDefHandle,
                                              jlong    jkeyDefHandle )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   size_t dataDefHandle = (size_t) jdataDefHandle;
   size_t keyDefHandle  = (size_t) jkeyDefHandle;
   const char * name;
   psoObjectDefinition definition;
   
   /* jni variables needed to access the jvm data */
   jobject jTypeObj;
   
   /*
    * Note: types are usually set using an enum. So we must extract
    * the enum object first before we can access the int field.
    */
   jTypeObj = (*env)->GetObjectField( env, jdefinition, g_idObjDefType );
   definition.type = (*env)->GetIntField( env, jTypeObj, g_idObjTypeType );
   (*env)->DeleteLocalRef( env, jTypeObj );

   definition.flags = (*env)->GetIntField( env, jdefinition, g_idObjDefFlags );
   definition.minNumOfDataRecords = (size_t) (*env)->GetLongField( env,
      jdefinition, g_idObjDefMinNumOfDataRecords );
   definition.minNumBlocks = (size_t) (*env)->GetLongField( env,
      jdefinition, g_idObjDefMinNumBlocks );
   
   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoCreateKeyedObject( (PSO_HANDLE) handle,
                                   name,
                                   strlen(name),
                                   &definition,
                                   (PSO_HANDLE)dataDefHandle,
                                   (PSO_HANDLE)keyDefHandle );
   
   (*env)->ReleaseStringUTFChars( env, jname, name );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoCreateKeyedObjectEx
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Session_psoCreateKeyedObjectEx( JNIEnv * env,
                                                jobject  jobj,
                                                jlong    jhandle,
                                                jstring  jname,
                                                jobject  jdefinition,
                                                jstring  jkeyDefName,
                                                jstring  jdataDefName )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   PSO_HANDLE dataDefHandle = NULL, keyDefHandle = NULL;
   const char * name;
   psoObjectDefinition definition;
   const char * dataDefName, * keyDefName;
   
   /* jni variables needed to access the jvm data */
   jobject jTypeObj;
   
   /*
    * Note: types are usually set using an enum. So we must extract
    * the enum object first before we can access the int field.
    */
   jTypeObj = (*env)->GetObjectField( env, jdefinition, g_idObjDefType );
   definition.type = (*env)->GetIntField( env, jTypeObj, g_idObjTypeType );
   (*env)->DeleteLocalRef( env, jTypeObj );

   definition.flags = (*env)->GetIntField( env, jdefinition, g_idObjDefFlags );
   definition.minNumOfDataRecords = (size_t) (*env)->GetLongField( env,
      jdefinition, g_idObjDefMinNumOfDataRecords );
   definition.minNumBlocks = (size_t) (*env)->GetLongField( env,
      jdefinition, g_idObjDefMinNumBlocks );

   dataDefName = (*env)->GetStringUTFChars( env, jdataDefName, NULL );
   if ( dataDefName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoDataDefOpen( (PSO_HANDLE)handle,
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

   errcode = psoKeyDefOpen( (PSO_HANDLE)handle,
                            keyDefName,
                            strlen(keyDefName),
                            &keyDefHandle );
   (*env)->ReleaseStringUTFChars( env, jkeyDefName, dataDefName );
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

   errcode = psoCreateKeyedObject( (PSO_HANDLE) handle,
                                   name,
                                   strlen(name),
                                   &definition,
                                   keyDefHandle,
                                   dataDefHandle );
   
   (*env)->ReleaseStringUTFChars( env, jname, name );
   psoDataDefClose( dataDefHandle );
   psoKeyDefClose( keyDefHandle );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoDestroyObject
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Session_psoDestroyObject( JNIEnv  * env,
                                          jobject   jobj,
                                          jlong     jhandle,
                                          jstring   jname )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   const char * objectName;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   
   errcode = psoDestroyObject( (PSO_HANDLE) handle,
                               objectName,
                               strlen(objectName) );

   (*env)->ReleaseStringUTFChars( env, jname, objectName );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoFini
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Session_psoFini( JNIEnv  * env,
                                 jobject   jobj,
                                 jlong     jhandle )
{
   int errcode;
   size_t handle = (size_t)jhandle;
   
   errcode = psoExitSession( (PSO_HANDLE) handle );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoGetDataDefinition
 * Signature: (JLjava/lang/String;Lorg/photon/DataDefinition;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Session_psoGetDataDefinition( JNIEnv * env,
                                              jobject  jobj,
                                              jlong    jhandle,
                                              jstring  jname,
                                              jobject  jdefinition )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   const char * objectName;
   psoObjectDefinition objDefinition;
   PSO_HANDLE dataDefHandle = NULL, keyDefHandle = NULL;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoGetDataDefinition( (PSO_HANDLE) handle,
                                   objectName,
                                   strlen(objectName),
                                   &dataDefHandle );
   (*env)->ReleaseStringUTFChars( env, jname, objectName );

   if ( errcode == 0 ) {
//      (*env)->SetObjectField( env, jstatus, g_idStatusType, g_weakObjType[status.type-1] );

//      (*env)->SetIntField ( env, jstatus, g_idStatusStatus, status.status );
      (*env)->SetLongField( env, jdefinition, g_idDataDefHandle, dataDefHandle );
//      (*env)->SetLongField( env, jstatus, g_idStatusNumBlockGroup, status.numBlockGroup );
//      (*env)->SetLongField( env, jstatus, g_idStatusNumDataItem, status.numDataItem );
//      (*env)->SetLongField( env, jstatus, g_idStatusFreeBytes, status.freeBytes );
//      (*env)->SetIntField ( env, jstatus, g_idStatusMaxDataLength, status.maxDataLength );
//      (*env)->SetIntField ( env, jstatus, g_idStatusMaxKeyLength, status.maxKeyLength );
   }
      rc = psoaDataDefGetDef( m_definitionHandle, 
                           &m_defType,
                           (unsigned char **)&m_dataDef,
                           &m_dataDefLength );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if 0
/*
 * Class:     org_photon_Session
 * Method:    psoGetDefinition
 * Signature: (JLjava/lang/String;Lorg/photon/Definition;Lorg/photon/ObjectDefinition;Lorg/photon/KeyDefinition;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Session_psoGetDefinition( JNIEnv  * env,
                                          jobject   jobj,
                                          jlong     jhandle,
                                          jstring   jname,
                                          jobject   jdef,
                                          jobject   jbase,
                                          jobject   jkey )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   psoObjectDefinition definition;
   psoKeyDefinition key;
   psoFieldDefinition  * pFields = NULL;
   const char * objectName;
   jobject jfield;
   jobjectArray jarray;
   jstring jstr;
   unsigned int numFields = 0, i;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

#if 0
   /*
    * We call it a first time with numFields set to zero to retrieve the
    * number of fields.
    */
   errcode = psoGetDefinition( (PSO_HANDLE) handle,
                               objectName,
                               strlen(objectName),
                               &definition,
                               &key,
                               0,
                               NULL );
   if ( errcode == 0 && definition.numFields > 0 ) {
      numFields = definition.numFields;
      pFields = malloc( sizeof(psoFieldDefinition)*numFields );
      if ( pFields == NULL ) {
         (*env)->ReleaseStringUTFChars( env, jname, objectName );
         return PSO_NOT_ENOUGH_HEAP_MEMORY;
      }
      errcode = psoGetDefinition( (PSO_HANDLE) handle,
                                  objectName,
                                  strlen(objectName),
                                  &definition,
                                  &key,
                                  numFields,
                                  pFields );
   }
   (*env)->ReleaseStringUTFChars( env, jname, objectName );

   if ( errcode == 0 ) {
      
      (*env)->SetObjectField( env, jbase, g_idObjDefType, g_weakObjType[definition.type-1] );
      (*env)->SetIntField( env, jbase, g_idObjDefNumFields, definition.numFields );

      (*env)->SetObjectField( env, jdef, g_idDefinitionDef, jbase );
   
      /* Warning: new type using a key must be added here */
      if ( definition.type == PSO_HASH_MAP || definition.type == PSO_FAST_MAP ) {

         (*env)->SetObjectField( env, jkey, g_idKeyDefType, 
            g_weakKeyType[key.type-PSO_KEY_INTEGER] );

         (*env)->SetIntField( env, jkey, g_idKeyDefLength,    key.length );
         (*env)->SetIntField( env, jkey, g_idKeyDefMinLength, key.minLength );
         (*env)->SetIntField( env, jkey, g_idKeyDefMaxLength, key.maxLength );

         (*env)->SetObjectField( env, jdef, g_idDefinitionDef, jkey );
      }

      if ( definition.type != PSO_FOLDER ) {
         jarray = (*env)->NewObjectArray( env, 
                                          (jsize) numFields,
                                          g_FieldDefClass,
                                          NULL );
         if ( jarray == NULL ) {
            free(pFields);
            return PSO_NOT_ENOUGH_HEAP_MEMORY;
         }
         for ( i = 0; i < numFields; ++i ) {
            jfield = (*env)->AllocObject( env, g_FieldDefClass );
            if ( jfield == NULL ) {
               free(pFields);
               return PSO_NOT_ENOUGH_HEAP_MEMORY;
            }
            jstr = getNotNullTerminatedString( env, pFields[i].name, PSO_MAX_FIELD_LENGTH );
            if ( jstr == NULL ) {
               free(pFields);
               return PSO_NOT_ENOUGH_HEAP_MEMORY;
            }
            (*env)->SetObjectField( env, jfield, g_idFieldDefName, jstr );
            (*env)->DeleteLocalRef( env, jstr );

            (*env)->SetObjectField( env, jfield, g_idFieldDefType, 
               g_weakFieldType[pFields[i].type-1] );

            (*env)->SetIntField( env, jfield, g_idFieldDefLength,
               pFields[i].length );
            (*env)->SetIntField( env, jfield, g_idFieldDefMinLength,
               pFields[i].minLength );
            (*env)->SetIntField( env, jfield, g_idFieldDefMaxLength,
               pFields[i].maxLength );
            (*env)->SetIntField( env, jfield, g_idFieldDefPrecision,
               pFields[i].precision );
            (*env)->SetIntField( env, jfield, g_idFieldDefScale,
               pFields[i].scale );
            (*env)->SetObjectArrayElement( env, jarray, i, jfield );
            (*env)->DeleteLocalRef( env, jfield );
         }
         (*env)->SetObjectField( env, jdef, g_idDefinitionDef, jarray );
      }
   }

   if ( pFields != NULL ) free(pFields);
#endif

   return errcode;
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoGetInfo
 * Signature: (JLorg/photon/Info;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Session_psoGetInfo( JNIEnv  * env,
                                    jobject   jobj,
                                    jlong     jhandle,
                                    jobject   jinfo )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   psoInfo info;
   jstring jstr;
   
   errcode = psoGetInfo( (PSO_HANDLE) handle, &info );
   if ( errcode == 0 ) {

      (*env)->SetLongField( env, jinfo, g_idInfoTotalSizeInBytes, info.totalSizeInBytes );
      (*env)->SetLongField( env, jinfo, g_idInfoAllocatedSizeInBytes, info.allocatedSizeInBytes );
      (*env)->SetLongField( env, jinfo, g_idInfoNumObjects, info.numObjects );
      (*env)->SetLongField( env, jinfo, g_idInfoNumGroups, info.numGroups );
      (*env)->SetLongField( env, jinfo, g_idInfoNumMallocs, info.numMallocs );
      (*env)->SetLongField( env, jinfo, g_idInfoNumFrees, info.numFrees );
      (*env)->SetLongField( env, jinfo, g_idInfoLargestFreeInBytes, info.largestFreeInBytes );
      (*env)->SetIntField ( env, jinfo, g_idInfoMemoryVersion, info.memoryVersion );
      (*env)->SetIntField ( env, jinfo, g_idInfoBigEndian, info.bigEndian );

      jstr = getNotNullTerminatedString( env, info.compiler, 20 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoCompiler, jstr );
      (*env)->DeleteLocalRef( env, jstr );

      jstr = getNotNullTerminatedString( env, info.compilerVersion, 10 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoCompilerVersion, jstr );
      (*env)->DeleteLocalRef( env, jstr );

      jstr = getNotNullTerminatedString( env, info.platform, 20 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoPlatform, jstr );
      (*env)->DeleteLocalRef( env, jstr );

      jstr = getNotNullTerminatedString( env, info.dllVersion, 10 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoDllVersion, jstr );
      (*env)->DeleteLocalRef( env, jstr );

      jstr = getNotNullTerminatedString( env, info.quasarVersion, 10 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoQuasarVersion, jstr );
      (*env)->DeleteLocalRef( env, jstr );

      jstr = getNotNullTerminatedString( env, info.creationTime, 30 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoCreationTime, jstr );
      (*env)->DeleteLocalRef( env, jstr );
      
   }
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoGetStatus
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectStatus;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Session_psoGetStatus( JNIEnv  * env,
                                      jobject   jobj,
                                      jlong     jhandle,
                                      jstring   jname,
                                      jobject   jstatus )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   const char * objectName;
   psoObjStatus status;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoGetStatus( (PSO_HANDLE) handle,
                           objectName,
                           strlen(objectName),
                           &status );
   (*env)->ReleaseStringUTFChars( env, jname, objectName );

   if ( errcode == 0 ) {
      (*env)->SetObjectField( env, jstatus, g_idStatusType, g_weakObjType[status.type-1] );

      (*env)->SetIntField ( env, jstatus, g_idStatusStatus, status.status );
      (*env)->SetLongField( env, jstatus, g_idStatusNumBlocks, status.numBlocks );
      (*env)->SetLongField( env, jstatus, g_idStatusNumBlockGroup, status.numBlockGroup );
      (*env)->SetLongField( env, jstatus, g_idStatusNumDataItem, status.numDataItem );
      (*env)->SetLongField( env, jstatus, g_idStatusFreeBytes, status.freeBytes );
      (*env)->SetIntField ( env, jstatus, g_idStatusMaxDataLength, status.maxDataLength );
      (*env)->SetIntField ( env, jstatus, g_idStatusMaxKeyLength, status.maxKeyLength );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoInit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Session_psoInit( JNIEnv * env, jobject jobj )
{
   int errcode;
   PSO_HANDLE handle;
   
   errcode = psoInitSession( &handle );

   // Normal return
   if ( errcode == PSO_OK ) {
      (*env)->SetLongField( env, jobj, g_idSessionHandle, (size_t) handle );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoRollback
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Session_psoRollback( JNIEnv  * env, 
                                     jobject   jobj, 
                                     jlong     jhandle )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   
   errcode = psoRollback( (PSO_HANDLE)handle );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

