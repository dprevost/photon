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
#include "org_photon_DataDefBuilderUser.h"

jfieldID g_id_keyFields;
jfieldID g_id_currentLength;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_KeyDefBuilderUser
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_KeyDefBuilderUser_initIDs( JNIEnv * env,
                                           jclass classDefinition )
{
   g_id_keyFields = (*env)->GetFieldID( env, classDefinition, "keyFields", "[B" );
   if ( g_id_keyFields == NULL ) return;
   g_id_currentLength = (*env)->GetFieldID( env, classDefinition, "currentLength", "J" );
   if ( g_id_currentLength == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_KeyDefBuilderUser
 * Method:    psoAddKeyField
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_KeyDefBuilderUser_psoAddKeyField( JNIEnv * env,
                                                  jobject  jobj,
                                                  jstring  jdesc )
{
   int currentLength;
   const char * desc;
   int length = 0;
   jbyteArray jarray, jfields;
   unsigned char * data;
   char separator = '\0';
   
   currentLength = (*env)->GetIntField( env, jobj, g_id_currentLength );
   
   desc = (*env)->GetStringUTFChars( env, jdesc, NULL );
   if ( desc == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   if ( strlen(desc) == 0 ) {
      (*env)->ReleaseStringUTFChars( env, jdesc, desc );
      return PSO_INVALID_LENGTH;
   }

   length = strlen(desc);

   if ( currentLength > 0 ) {
      length = currentLength + 1;
   }

   jarray = (*env)->NewByteArray( env, length );
   if ( jarray == NULL ) {
      (*env)->ReleaseStringUTFChars( env, jdesc, desc );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   if ( currentLength == 0 ) {
      (*env)->SetByteArrayRegion( env, jarray, 0, length, (jbyte *)desc );
   }
   else {
      /* We copied the old definition into "data" */
      jfields = (*env)->GetObjectField( env, jobj, g_id_keyFields );
      if ( jfields == NULL ) {
         (*env)->ReleaseStringUTFChars( env, jdesc, desc );
         return PSO_NOT_ENOUGH_HEAP_MEMORY;
      }
      data = malloc(currentLength);
      if ( data == NULL ) {
         (*env)->ReleaseStringUTFChars( env, jdesc, desc );
         return PSO_NOT_ENOUGH_HEAP_MEMORY;
      }
      (*env)->GetByteArrayRegion( env, jfields, 0, currentLength, (jbyte *)data );

      /* Copy the data into the new array */
      (*env)->SetByteArrayRegion( env, jarray, 0, currentLength, (jbyte *)data );
      (*env)->SetByteArrayRegion( env, jarray, currentLength, 1, (jbyte *)&separator );
      (*env)->SetByteArrayRegion( env, jarray, currentLength+1, length, (jbyte *)desc );
      
   }
   (*env)->ReleaseStringUTFChars( env, jdesc, desc );
   
   (*env)->SetIntField( env, jobj, g_id_currentLength, currentLength );
   (*env)->SetObjectField( env, jobj, g_id_keyFields, jarray );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

