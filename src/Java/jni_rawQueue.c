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
#include "org_photon_RawQueue.h"
#include "API/Queue.h"

jfieldID g_idQueueHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_RawQueue
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_RawQueue_initIDs( JNIEnv * env, jclass queueClass )
{
   g_idQueueHandle = (*env)->GetFieldID( env, queueClass, "handle", "J" );
   if ( g_idQueueHandle == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_RawQueue
 * Method:    psoClose
 * Signature: (J)V
 */
JNIEXPORT void JNICALL 
Java_org_photon_RawQueue_psoClose( JNIEnv  * env,
                                   jobject   jobj,
                                   jlong     jhandle )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   
   errcode = psoQueueClose( (PSO_HANDLE)handle );

   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/*
 * Class:     org_photon_RawQueue
 * Method:    psoDataDefinition
 * Signature: (JLorg/photon/DataDefinition;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_RawQueue_psoDataDefinition( JNIEnv * env,
                                            jobject  jobj,
                                            jlong    jhandle,
                                            jobject  jdefinition )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   PSO_HANDLE dataDefHandle = NULL;
   enum psoDefinitionType  defType;
   unsigned char * dataDef;
   unsigned int  dataDefLength; 
   jbyteArray jba;
   char * defName, * dataDefName;
   unsigned int defNameLength;
   jstring jdataDefName;
   
   errcode = psoQueueDefinition( (PSO_HANDLE) handle, &dataDefHandle );
   if ( errcode == 0 ) {
      errcode = psoaDataDefGetDef( dataDefHandle,
                                   &defName,
                                   &defNameLength,
                                   &defType,
                                   (unsigned char **)&dataDef,
                                   &dataDefLength );
      if ( errcode != 0 ) {
         psoDataDefClose( dataDefHandle );
         return errcode;
      }
   
      // The name is the key in the hashmap and is not null-terminated
      dataDefName = calloc( defNameLength + 1, 1 );
      if ( dataDefName == NULL ) {
         psoDataDefClose( dataDefHandle );
         return PSO_NOT_ENOUGH_HEAP_MEMORY;
      }
      memcpy( dataDefName, defName, defNameLength );
      jdataDefName = (*env)->NewStringUTF( env, dataDefName );
      free( dataDefName );
      if ( jdataDefName == NULL ) {
         psoDataDefClose( dataDefHandle );
         return PSO_NOT_ENOUGH_HEAP_MEMORY;
      }
      
      jba = (*env)->NewByteArray( env, dataDefLength );
      if ( jba == NULL ) {
         psoDataDefClose( dataDefHandle );
         (*env)->DeleteLocalRef( env, jdataDefName );
         return PSO_NOT_ENOUGH_HEAP_MEMORY;
      }
   
      (*env)->SetByteArrayRegion( env, jba, 0, dataDefLength, (jbyte *)dataDef );

      (*env)->SetObjectField( env, jdefinition, g_idDataDefDataDef, jba );
      (*env)->SetIntField(    env, jdefinition, g_idDataDefType,    defType );
      (*env)->SetLongField(   env, jdefinition, g_idDataDefHandle,  (size_t)dataDefHandle );
      (*env)->SetObjectField( env, jdefinition, g_idDataDefName,    jdataDefName );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_RawQueue
 * Method:    psoGetFirst
 * Signature: (J[B)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_RawQueue_psoGetFirst( JNIEnv   * env,
                                      jobject    jobj,
                                      jlong      jhandle,
                                      jbyteArray jbuffer )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   unsigned char * data;
   unsigned int length;
   
   errcode = psoaQueueFirst( (psoaQueue *) handle, &data, &length );
   if ( errcode == 0 ) {
      jbuffer = (*env)->NewByteArray( env, length );
      (*env)->SetByteArrayRegion( env, jbuffer, 0, length, (jbyte*)data );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_RawQueue
 * Method:    psoGetNext
 * Signature: (J[B)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_RawQueue_psoGetNext( JNIEnv   * env,
                                     jobject    jobj,
                                     jlong      jhandle,
                                     jbyteArray jbuffer )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   unsigned char * data;
   unsigned int length;
   
   errcode = psoaQueueNext( (psoaQueue *) handle, &data, &length );
   if ( errcode == 0 ) {
      jbuffer = (*env)->NewByteArray( env, length );
      (*env)->SetByteArrayRegion( env, jbuffer, 0, length, (jbyte*)data );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/*
 * Class:     org_photon_RawQueue
 * Method:    psoGetRecordDefinition
 * Signature: (JLorg/photon/DataDefinition;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_RawQueue_psoGetRecordDefinition( JNIEnv * env,
                                                 jobject  jobj,
                                                 jlong    jhandle, 
                                                 jobject  jdefinition )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   PSO_HANDLE dataDefHandle = NULL;
   enum psoDefinitionType  defType;
   unsigned char * dataDef;
   unsigned int  dataDefLength; 
   jbyteArray jba;
   char * defName, * dataDefName;
   unsigned int defNameLength;
   jstring jdataDefName;
   
   errcode = psoQueueRecordDefinition( (PSO_HANDLE) handle, &dataDefHandle );
   if ( errcode == 0 ) {
      errcode = psoaDataDefGetDef( dataDefHandle,
                                   &defName,
                                   &defNameLength,
                                   &defType,
                                   (unsigned char **)&dataDef,
                                   &dataDefLength );
      if ( errcode != 0 ) {
         psoDataDefClose( dataDefHandle );
         return errcode;
      }
   
      // The name is the key in the hashmap and is not null-terminated
      dataDefName = calloc( defNameLength + 1, 1 );
      if ( dataDefName == NULL ) {
         psoDataDefClose( dataDefHandle );
         return PSO_NOT_ENOUGH_HEAP_MEMORY;
      }
      memcpy( dataDefName, defName, defNameLength );
      jdataDefName = (*env)->NewStringUTF( env, dataDefName );
      free( dataDefName );
      if ( jdataDefName == NULL ) {
         psoDataDefClose( dataDefHandle );
         return PSO_NOT_ENOUGH_HEAP_MEMORY;
      }
      
      jba = (*env)->NewByteArray( env, dataDefLength );
      if ( jba == NULL ) {
         psoDataDefClose( dataDefHandle );
         (*env)->DeleteLocalRef( env, jdataDefName );
         return PSO_NOT_ENOUGH_HEAP_MEMORY;
      }
   
      (*env)->SetByteArrayRegion( env, jba, 0, dataDefLength, (jbyte *)dataDef );

      (*env)->SetObjectField( env, jdefinition, g_idDataDefDataDef, jba );
      (*env)->SetIntField(    env, jdefinition, g_idDataDefType,    defType );
      (*env)->SetLongField(   env, jdefinition, g_idDataDefHandle,  (size_t)dataDefHandle );
      (*env)->SetObjectField( env, jdefinition, g_idDataDefName,    jdataDefName );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_RawQueue
 * Method:    psoGetStatus
 * Signature: (JLorg/photon/ObjectStatus;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_RawQueue_psoGetStatus( JNIEnv  * env,
                                       jobject   jobj,
                                       jlong     jhandle,
                                       jobject   jstatus )
{
   int errcode;
   size_t handle = (size_t)jhandle;
   psoObjStatus status;

   errcode = psoQueueStatus( (PSO_HANDLE) handle, &status );

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

/*
 * Class:     org_photon_RawQueue
 * Method:    psoOpen
 * Signature: (Lorg/photon/Session;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_RawQueue_psoOpen( JNIEnv  * env,
                                  jobject   jobj,
                                  jobject   jsession,
                                  jstring   jname )
{
   int errcode;
   PSO_HANDLE handle;
   size_t sessionHandle;
   const char * queueName;

   queueName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( queueName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   sessionHandle = (*env)->GetLongField( env, jsession, g_idSessionHandle );

   errcode = psoQueueOpen( (PSO_HANDLE) sessionHandle,
                           queueName,
                           strlen(queueName),
                           &handle );
   (*env)->ReleaseStringUTFChars( env, jname, queueName );

   if ( errcode != PSO_OK ) return errcode;

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_RawQueue
 * Method:    psoPop
 * Signature: (J[B)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_RawQueue_psoPop( JNIEnv   * env,
                                 jobject    jobj,
                                 jlong      jhandle,
                                 jbyteArray jbuffer )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   unsigned char * data;
   unsigned int length;
   
   errcode = psoaQueueRemove( (psoaQueue *) handle, &data, &length );
   if ( errcode == 0 ) {
      jbuffer = (*env)->NewByteArray( env, length );
      (*env)->SetByteArrayRegion( env, jbuffer, 0, length, (jbyte*)data );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_RawQueue
 * Method:    psoPush
 * Signature: (J[B)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_RawQueue_psoPush( JNIEnv   * env,
                                  jobject    jobj,
                                  jlong      jhandle,
                                  jbyteArray jbuffer )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   void * buffer;
   jsize length;
   
   length = (*env)->GetArrayLength( env, jbuffer );
   buffer = (void *)(*env)->GetByteArrayElements( env, jbuffer, NULL );
   if ( buffer == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoQueuePush( (PSO_HANDLE)handle,
                           buffer,
                           length,
                           NULL );
   (*env)->ReleaseByteArrayElements( env, jbuffer, buffer, JNI_ABORT );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_RawQueue
 * Method:    psoPushNow
 * Signature: (J[B)I
 */
JNIEXPORT jint JNICALL
va_org_photon_RawQueue_psoPushNow( JNIEnv * env,
                                   jobject  jobj,
                                   jlong    jhandle,
                                   jbyteArray jbuffer )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   void * buffer;
   jsize length;
   
   length = (*env)->GetArrayLength( env, jbuffer );
   buffer = (void *)(*env)->GetByteArrayElements( env, jbuffer, NULL );
   if ( buffer == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoQueuePushNow( (PSO_HANDLE)handle,
                              buffer,
                              length,
                              NULL );
   (*env)->ReleaseByteArrayElements( env, jbuffer, buffer, JNI_ABORT );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

