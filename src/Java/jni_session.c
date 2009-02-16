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
#include "org_photon_PhotonSession.h"

jfieldID g_idSessionHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonSession
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_PhotonSession_initIDs( JNIEnv * env, jclass sessionClass )
{
   g_idSessionHandle = (*env)->GetFieldID( env, sessionClass, "handle", "J" );
   if ( g_idSessionHandle == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonSession
 * Method:    psoCommit
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonSession_psoCommit( JNIEnv  * env, 
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
 * Class:     org_photon_PhotonSession
 * Method:    psoCreateObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;Lorg/photon/KeyDefinition;[Lorg/photon/FieldDefinition;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonSession_psoCreateObject( JNIEnv     * env,
                                               jobject      jobj,
                                               jlong        jhandle,
                                               jstring      jname,
                                               jobject      jdef,
                                               jobject      jkey,
                                               jobjectArray jfields )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   const char * name;
   psoObjectDefinition definition;
   psoKeyDefinition key, * pKey = NULL;
   psoFieldDefinition  * pFields = NULL;
   
   /* jni variables needed to access the jvm data */
   jsize  length, i;
   jobject objField;
   jstring jfieldName;
   jobject jTypeObj;
   
   /*
    * Note: types are usually set using an enum. So we must extract
    * the enum object first before we can access the int field.
    */
   jTypeObj = (*env)->GetObjectField( env, jdef, g_idObjDefType );
   definition.type = (*env)->GetIntField( env, jTypeObj, g_idObjTypeType );
   (*env)->DeleteLocalRef( env, jTypeObj );

   definition.numFields = (*env)->GetIntField( env, jdef, g_idObjDefNumFields );
   
   if ( jkey != NULL ) {
      jTypeObj = (*env)->GetObjectField( env, jkey, g_idKeyDefType );
      key.type = (*env)->GetIntField( env, jTypeObj, g_idKeyTypeType );
      (*env)->DeleteLocalRef( env, jTypeObj );

      key.length    = (*env)->GetIntField( env, jkey, g_idKeyDefLength );
      key.minLength = (*env)->GetIntField( env, jkey, g_idKeyDefMinLength );
      key.maxLength = (*env)->GetIntField( env, jkey, g_idKeyDefMaxLength );
      
      pKey = &key;
   }

   if ( jfields != NULL ) {
      length = (*env)->GetArrayLength( env, jfields );
      if ( (jsize)definition.numFields != length ) {
         return PSO_INVALID_NUM_FIELDS;
      }
      else {
         pFields = malloc( sizeof(psoFieldDefinition)*length );
         if ( pFields == NULL ) { return PSO_NOT_ENOUGH_HEAP_MEMORY; }
      }

      /*
       * Warning! At this point, memory leaks are possible. We have
       * to be careful on errors.
       */
   
      for ( i = 0; i < length; ++i ) {
         objField = (*env)->GetObjectArrayElement( env, jfields, i );

         jfieldName = (*env)->GetObjectField( env, objField, g_idFieldDefName );
         name = (*env)->GetStringUTFChars( env, jfieldName, NULL );
         if ( name == NULL ) {
            free(pFields);
            return PSO_NOT_ENOUGH_HEAP_MEMORY;
         }
         strncpy( pFields[i].name, name, PSO_MAX_FIELD_LENGTH );
         (*env)->ReleaseStringUTFChars( env, jfieldName, name );

         jTypeObj = (*env)->GetObjectField( env, objField, g_idFieldDefType );
         pFields[i].type = (*env)->GetIntField( env, jTypeObj, g_idFieldTypeType );
         (*env)->DeleteLocalRef( env, jTypeObj );
      
         pFields[i].length    = (*env)->GetIntField( env, objField, g_idFieldDefLength );
         pFields[i].minLength = (*env)->GetIntField( env, objField, g_idFieldDefMinLength );
         pFields[i].maxLength = (*env)->GetIntField( env, objField, g_idFieldDefMaxLength );
         pFields[i].precision = (*env)->GetIntField( env, objField, g_idFieldDefPrecision );
         pFields[i].scale     = (*env)->GetIntField( env, objField, g_idFieldDefScale );
         (*env)->DeleteLocalRef( env, objField );
      }
   }
   
   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      if ( pFields != NULL ) free(pFields);
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoCreateObject( (PSO_HANDLE) handle,
                              name,
                              strlen(name),
                              &definition,
                              pKey,
                              pFields );

   (*env)->ReleaseStringUTFChars( env, jname, name );
   if ( pFields != NULL ) free(pFields);

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonSession
 * Method:    psoDestroyObject
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonSession_psoDestroyObject( JNIEnv  * env,
                                                jobject   jobj,
                                                jlong     jhandle,
                                                jstring   jname )
{
   int errcode;
   size_t handle = (size_t)jhandle;
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
 * Class:     org_photon_PhotonSession
 * Method:    psoFini
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonSession_psoFini( JNIEnv  * env,
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
 * Class:     org_photon_PhotonSession
 * Method:    psoGetDefinition
 * Signature: (JLjava/lang/String;Lorg/photon/Definition;Lorg/photon/ObjectDefinition;Lorg/photon/KeyDefinition;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonSession_psoGetDefinition( JNIEnv  * env,
                                                jobject   jobj,
                                                jlong     jhandle,
                                                jstring   jname,
                                                jobject   jdef,
                                                jobject   jbase,
                                                jobject   jkey )
{
   int errcode;
   size_t handle = (size_t)jhandle;
   psoObjectDefinition definition;
   psoKeyDefinition key;
   psoFieldDefinition  * pFields = NULL;
   const char * objectName;
   jobject jfield;
   jobjectArray jarray;
   jstring jstr;
   unsigned int numFields, i;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

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
            
            
   /** The data type of the field/ */
   private FieldType type;
   
   /** For fixed-length data types */
   private int length;
extern jfieldID g_idFieldDefName;
extern jfieldID g_idFieldDefType;
extern jfieldID g_idFieldDefLength;
extern jfieldID g_idFieldDefMinLength;
extern jfieldID g_idFieldDefMaxLength;
extern jfieldID g_idFieldDefPrecision;
extern jfieldID g_idFieldDefScale;

   /** For variable-length data types */
   private int minLength;

   /** For variable-length data types */
   private int maxLength;

   /** Total number of digits in the decimal field. */
   private int precision;

   /** Number of digits following the decimal separator. */
   private int scale;
              
         }
      }


      
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonSession
 * Method:    psoGetInfo
 * Signature: (JLorg/photon/Info;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonSession_psoGetInfo( JNIEnv  * env,
                                          jobject   jobj,
                                          jlong     jhandle,
                                          jobject   jinfo )
{
   int errcode;
   size_t handle = (size_t)jhandle;
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
      (*env)->SetIntField( env, jinfo, g_idInfoMemoryVersion, info.memoryVersion );
      (*env)->SetIntField( env, jinfo, g_idInfoBigEndian, info.bigEndian );

      jstr = getNotNullTerminatedString( env, info.compiler, 20 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoCompiler, jstr );

      jstr = getNotNullTerminatedString( env, info.compilerVersion, 10 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoCompilerVersion, jstr );

      jstr = getNotNullTerminatedString( env, info.platform, 20 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoPlatform, jstr );

      jstr = getNotNullTerminatedString( env, info.dllVersion, 10 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoDllVersion, jstr );

      jstr = getNotNullTerminatedString( env, info.quasarVersion, 10 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoQuasarVersion, jstr );

      jstr = getNotNullTerminatedString( env, info.creationTime, 30 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoCreationTime, jstr );
      
   }
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonSession
 * Method:    psoGetStatus
 * Signature: (JLjava/lang/String;Lorg/photon/ObjStatus;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonSession_psoGetStatus( JNIEnv  * env,
                                            jobject   jobj,
                                            jlong     jhandle,
                                            jstring   jname,
                                            jobject   jstatus )
{
   int errcode;
   size_t handle = (size_t)jhandle;
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

      (*env)->SetIntField(  env, jstatus, g_idStatusStatus, status.status );
      (*env)->SetLongField( env, jstatus, g_idStatusNumBlocks, status.numBlocks );
      (*env)->SetLongField( env, jstatus, g_idStatusNumBlockGroup, status.numBlockGroup );
      (*env)->SetLongField( env, jstatus, g_idStatusNumDataItem, status.numDataItem );
      (*env)->SetLongField( env, jstatus, g_idStatusFreeBytes, status.freeBytes );
      (*env)->SetIntField(  env, jstatus, g_idStatusMaxDataLength, status.maxDataLength );
      (*env)->SetIntField(  env, jstatus, g_idStatusMaxKeyLength, status.maxKeyLength );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonSession
 * Method:    psoInit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonSession_psoInit( JNIEnv * env, jobject jobj )
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
 * Class:     org_photon_PhotonSession
 * Method:    psoRollback
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_PhotonSession_psoRollback( JNIEnv  * env, 
                                           jobject   jobj, 
                                           jlong     jhandle )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   
   errcode = psoRollback( (PSO_HANDLE)handle );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

