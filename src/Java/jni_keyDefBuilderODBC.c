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
#include "org_photon_DataDefBuilderODBC.h"

jfieldID g_id_keyFields;
jfieldID g_id_current;
jfieldID g_id_simple;
jfieldID g_id_numKeyFields;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_KeyDefBuilderODBC
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_KeyDefBuilderODBC_initIDs( JNIEnv * env,
                                           jclass classDefinition)
{
   g_id_keyFields = (*env)->GetFieldID( env, classDefinition, "keyFields", "[B" );
   if ( g_id_keyFields == NULL ) return;
   g_id_current = (*env)->GetFieldID( env, classDefinition, "currentKeyField", "J" );
   if ( g_id_current == NULL ) return;
   g_id_simple = (*env)->GetFieldID( env, classDefinition, "simpleDef", "Z" );
   if ( g_id_simple == NULL ) return;
   g_id_numKeyFields = (*env)->GetFieldID( env, classDefinition, "numKeyFields", "J" );
   if ( g_id_numKeyFields == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_KeyDefBuilderODBC
 * Method:    psoAddKeyField
 * Signature: (Ljava/lang/String;II)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_KeyDefBuilderODBC_psoAddKeyField(JNIEnv * env,
                                                 jobject  jobj,
                                                 jstring  jname,
                                                 jint     jtype,
                                                 jint     jlength )
{
   enum psoKeyType type = jtype;
   psoKeyFieldDefinition * key;
   const char * name;
   jbyteArray jkeys;
   int currentKey, numKeys;
   jboolean simpleDef;

   jkeys = (*env)->GetObjectField( env, jobj, g_id_keyFields );
   currentKey = (*env)->GetIntField( env, jobj, g_id_current );
   simpleDef = (*env)->GetBooleanField( env, jobj, g_id_simple );
   numKeys = (*env)->GetIntField( env, jobj, g_id_numKeyFields );
   
   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   if ( strlen(name) == 0 || strlen(name) > PSO_MAX_FIELD_LENGTH ) {
      (*env)->ReleaseStringUTFChars( env, jname, name );
      return PSO_INVALID_FIELD_NAME;
   }

   key = (psoKeyFieldDefinition *)(*env)->GetByteArrayElements( env, jkeys, NULL );
   if ( key == NULL ) {
      (*env)->ReleaseStringUTFChars( env, jname, name );
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   memcpy( key[currentKey].name, name, strlen(name) );
   (*env)->ReleaseStringUTFChars( env, jname, name );

   switch ( type ) {
   case PSO_KEY_INTEGER:
   case PSO_KEY_BIGINT:
   case PSO_KEY_DATE:
   case PSO_KEY_TIME:
   case PSO_KEY_TIMESTAMP:
      key[currentKey].type = type;
      key[currentKey].length = 0;
      currentKey++;
      break;

   case PSO_KEY_BINARY:
   case PSO_KEY_CHAR:
      if ( jlength == 0 ) {
         (*env)->ReleaseByteArrayElements( env, jkeys, (jbyte *)key, JNI_ABORT );
         return PSO_INVALID_FIELD_LENGTH;
      }
      key[currentKey].type = type;
      key[currentKey].length = jlength;
      currentKey++;
      break;

   case PSO_KEY_VARBINARY:
   case PSO_KEY_VARCHAR:
      if ( simpleDef && currentKey != numKeys-1 ) {
         (*env)->ReleaseByteArrayElements( env, jkeys, (jbyte *)key, JNI_ABORT );
         return PSO_INVALID_FIELD_TYPE;
      }
      if ( jlength == 0 ) {
         (*env)->ReleaseByteArrayElements( env, jkeys, (jbyte *)key, JNI_ABORT );
         return PSO_INVALID_FIELD_LENGTH;
      }
      key[currentKey].type = type;
      key[currentKey].length = jlength;
      currentKey++;
      break;

   case PSO_KEY_LONGVARBINARY:
   case PSO_KEY_LONGVARCHAR:
      if ( simpleDef && currentKey != numKeys-1 ) {
         (*env)->ReleaseByteArrayElements( env, jkeys, (jbyte *)key, JNI_ABORT );
         return PSO_INVALID_FIELD_TYPE;
      }
      key[currentKey].type = type;
      key[currentKey].length = 0;
      currentKey++;
      break;

   default:
      (*env)->ReleaseByteArrayElements( env, jkeys, (jbyte *)key, JNI_ABORT );
      return PSO_INVALID_FIELD_TYPE;
   }
   
   (*env)->ReleaseByteArrayElements( env, jkeys, (jbyte *)key, 0 );
   (*env)->SetIntField( env, jobj, g_id_current, currentKey );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_KeyDefBuilderODBC
 * Method:    psoGetLength
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_photon_KeyDefBuilderODBC_psoGetLength( JNIEnv * env,
                                                jobject  jobj )
{
   return (jint) sizeof(psoKeyFieldDefinition);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

