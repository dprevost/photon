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

jfieldID g_id_fields;
jfieldID g_id_current;
jfieldID g_id_simple;
jfieldID g_id_numFields;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_DataDefBuilderODBC
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_org_photon_DataDefBuilderODBC_initIDs( JNIEnv * env,
                                            jclass classDefinition )
{
   g_id_fields = (*env)->GetFieldID( env, classDefinition, "fields", "[B" );
   if ( g_id_fields == NULL ) return;
   g_id_current = (*env)->GetFieldID( env, classDefinition, "currentField", "J" );
   if ( g_id_current == NULL ) return;
   g_id_simple = (*env)->GetFieldID( env, classDefinition, "simpleDef", "Z" );
   if ( g_id_simple == NULL ) return;
   g_id_numFields = (*env)->GetFieldID( env, classDefinition, "numFields", "J" );
   if ( g_id_numFields == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_DataDefBuilderODBC
 * Method:    psoAddField
 * Signature: (Ljava/lang/String;IIII)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_DataDefBuilderODBC_psoAddField( JNIEnv * env,
                                                jobject  jobj,
                                                jstring  jname,
                                                jint     jtype,
                                                jint     jlength,
                                                jint     jscale,
                                                jint     jprecision )
{
   enum psoFieldType type = jtype;
   psoFieldDefinition * field;
   const char * name;
   jbyteArray jfields;
   int currentField, numFields;
   jboolean simpleDef;
   
   jfields = (*env)->GetObjectField( env, jobj, g_id_fields );
   currentField = (*env)->GetIntField( env, jobj, g_id_current );
   simpleDef = (*env)->GetBooleanField( env, jobj, g_id_simple );
   numFields = (*env)->GetIntField( env, jobj, g_id_numFields );
   
   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   if ( strlen(name) == 0 || strlen(name) > PSO_MAX_FIELD_LENGTH ) {
      (*env)->ReleaseStringUTFChars( env, jname, name );
      return PSO_INVALID_FIELD_NAME;
   }

   field = (psoFieldDefinition *)(*env)->GetByteArrayElements( env, jfields, NULL );
   if ( field == NULL ) {
      (*env)->ReleaseStringUTFChars( env, jname, name );
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   memcpy( field[currentField].name, name, strlen(name) );
   (*env)->ReleaseStringUTFChars( env, jname, name );
   
   switch ( type ) {
   case PSO_TINYINT:
   case PSO_SMALLINT:
   case PSO_INTEGER:
   case PSO_BIGINT:
   case PSO_REAL:
   case PSO_DOUBLE:
   case PSO_DATE:
   case PSO_TIME:
   case PSO_TIMESTAMP:
      field[currentField].type = type;
      field[currentField].vals.length = 0;
      currentField++;
      break;

   case PSO_BINARY:
   case PSO_CHAR:
      if ( jlength == 0 ) {
         (*env)->ReleaseByteArrayElements( env, jfields, (jbyte *)field, JNI_ABORT );
         return PSO_INVALID_FIELD_LENGTH;
      }
      field[currentField].type = type;
      field[currentField].vals.length = jlength;
      currentField++;
      break;

   case PSO_VARBINARY:
   case PSO_VARCHAR:
      if ( simpleDef && currentField != numFields-1 ) {
         (*env)->ReleaseByteArrayElements( env, jfields, (jbyte *)field, JNI_ABORT );
         return PSO_INVALID_FIELD_TYPE;
      }
      if ( jlength == 0 ) {
         (*env)->ReleaseByteArrayElements( env, jfields, (jbyte *)field, JNI_ABORT );
         return PSO_INVALID_FIELD_LENGTH;
      }
      field[currentField].type = type;
      field[currentField].vals.length = jlength;
      currentField++;
      break;

   case PSO_LONGVARBINARY:
   case PSO_LONGVARCHAR:
      if ( simpleDef && currentField != numFields-1 ) {
         (*env)->ReleaseByteArrayElements( env, jfields, (jbyte *)field, JNI_ABORT );
         return PSO_INVALID_FIELD_TYPE;
      }
      field[currentField].type = type;
      field[currentField].vals.length = 0;
      currentField++;
      break;

   case PSO_NUMERIC:
      if ( jprecision == 0 || jprecision > PSO_FIELD_MAX_PRECISION ) {
         (*env)->ReleaseByteArrayElements( env, jfields, (jbyte *)field, JNI_ABORT );
         return PSO_INVALID_PRECISION;
      }
      if ( jscale > jprecision ) {
         (*env)->ReleaseByteArrayElements( env, jfields, (jbyte *)field, JNI_ABORT );
         return PSO_INVALID_SCALE;
      }
      field[currentField].type = type;
      field[currentField].vals.decimal.precision = jprecision;
      field[currentField].vals.decimal.scale = jscale;
      currentField++;
      break;

   default:
      (*env)->ReleaseByteArrayElements( env, jfields, (jbyte *)field, JNI_ABORT );
      return PSO_INVALID_FIELD_TYPE;
   }
   
   (*env)->ReleaseByteArrayElements( env, jfields, (jbyte *)field, 0 );
   (*env)->SetIntField( env, jobj, g_id_current, currentField );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_DataDefBuilderODBC
 * Method:    psoGetLength
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_photon_DataDefBuilderODBC_psoGetLength( JNIEnv * env,
                                                 jobject jobj )
{
   return (jint) sizeof(psoFieldDefinition);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

