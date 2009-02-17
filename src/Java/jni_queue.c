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
#include "org_photon_Queue.h"

jfieldID g_idQueueHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_Queue_initIDs( JNIEnv * env, jclass queueClass )
{
   g_idQueueHandle = (*env)->GetFieldID( env, queueClass, "handle", "J" );
   if ( g_idQueueHandle == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    psoFini
 * Signature: (J)V
 */
JNIEXPORT void JNICALL 
Java_org_photon_Queue_psoFini( JNIEnv  * env,
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
 * Class:     org_photon_Queue
 * Method:    psoGetFirst
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Queue_psoGetFirst( JNIEnv  * env,
                                   jobject   jobj,
                                   jlong     jhandle,
                                   jobject   jrecord );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    psoGetNext
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Queue_psoGetNext( JNIEnv  * env,
                                  jobject   jobj,
                                  jlong     jhandle,
                                  jobject   jrecord );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    psoGetStatus
 * Signature: (JLorg/photon/ObjectStatus;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Queue_psoGetStatus( JNIEnv  * env,
                                    jobject   jobj,
                                    jlong     jhandle,
                                    jobject   jstatus )
{
   int errcode;
   size_t handle = (size_t)jhandle;
   psoObjStatus status;

   errcode = psoQueueStatus( (PSO_HANDLE) handle, &status );

   if ( errcode == 0 ) {
      (*env)->SetObjectField( env, jstatus, g_idStatusType, g_weakObjType[status.type-1] );

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
 * Class:     org_photon_Queue
 * Method:    psoInit
 * Signature: (Lorg/photon/Session;Ljava/lang/String;Lorg/photon/Definition;Lorg/photon/ObjectDefinition;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Queue_psoInit( JNIEnv  * env,
                               jobject   jobj,
                               jobject   jsession,
                               jstring   jname,
                               jobject   jdef,
                               jobject   jbase )
{
   int errcode;
   PSO_HANDLE handle;
   size_t sessionHandle;
   const char * queueName;
   psoObjectDefinition definition;
   jobject jfield;
   jobjectArray jarray;
   jstring jstr;
   unsigned int numFields = 0, i;
   psoFieldDefinition  * pFields = NULL;

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
   
   /*
    * Cache the queue definition of our object.
    *
    * We call GetDef a first time with numFields set to zero to retrieve the
    * number of fields.
    */
   errcode = psoQueueDefinition( (PSO_HANDLE) handle,
                                 &definition,
                                 0,
                                 NULL );
   if ( errcode == 0 && definition.numFields > 0 ) {
      numFields = definition.numFields;
      pFields = malloc( sizeof(psoFieldDefinition)*numFields );
      if ( pFields == NULL ) {
         return PSO_NOT_ENOUGH_HEAP_MEMORY;
      }
      errcode = psoQueueDefinition( (PSO_HANDLE) handle,
                                    &definition,
                                    numFields,
                                    pFields );
   }

   if ( errcode == 0 ) {
      
      (*env)->SetObjectField( env, jbase, g_idObjDefType, g_weakObjType[definition.type-1] );
      (*env)->SetIntField( env, jbase, g_idObjDefNumFields, definition.numFields );

      (*env)->SetObjectField( env, jdef, g_idDefinitionDef, jbase );
   
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

   if ( pFields != NULL ) free(pFields);
   
   // Normal return
   if ( errcode == PSO_OK ) {
      (*env)->SetLongField( env, jobj, g_idQueueHandle, (size_t) handle );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    psoPop
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Queue_psoPop( JNIEnv  * env,
                              jobject   jobj,
                              jlong     jhandle,
                              jobject   jrecord );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    psoPush
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Queue_psoPush( JNIEnv  * env,
                               jobject   jobj,
                               jlong     jhandle,
                               jobject   jrecord );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    psoPushNow
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Queue_psoPushNow( JNIEnv  * env,
                                  jobject   jobj,
                                  jlong     jhandle,
                                  jobject   jrecord );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

