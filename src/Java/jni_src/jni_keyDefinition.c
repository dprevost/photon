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
#include "org_photon_KeyDefinition.h"
#include "API/KeyDefinition.h"

jfieldID g_idKeyDefHandle;
jfieldID g_idKeyDefKeyDef;
jfieldID g_idKeyDefType;
jfieldID g_idKeyDefCurrentLength;
jfieldID g_idKeyDefName;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_KeyDefinition
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_KeyDefinition_initIDs( JNIEnv * env,
                                       jclass   classDefinition )
{
   g_idKeyDefHandle  = (*env)->GetFieldID( env, classDefinition, "handle", "J" );
   if ( g_idKeyDefHandle == NULL ) return;
   g_idKeyDefKeyDef = (*env)->GetFieldID( env, classDefinition, "keyDef", "[B" );
   if ( g_idKeyDefKeyDef == NULL ) return;
   g_idKeyDefType = (*env)->GetFieldID( env, classDefinition, "type", "J" );
   if ( g_idKeyDefType == NULL ) return;
   g_idKeyDefCurrentLength = (*env)->GetFieldID( env, classDefinition, 
      "currentLength", "J" );
   if ( g_idKeyDefCurrentLength == NULL ) return;
   g_idKeyDefName = (*env)->GetFieldID( env, classDefinition, 
      "name", "Ljava/lang/String;" );
   if ( g_idKeyDefName == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_KeyDefinition
 * Method:    psoClose
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_KeyDefinition_psoClose( JNIEnv * env,
                                         jobject  jobj,
                                         jlong    jhandle )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;

   errcode = psoKeyDefClose( (PSO_HANDLE)handle );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_KeyDefinition
 * Method:    psoCreate
 * Signature: (JLjava/lang/String;I[BI)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_KeyDefinition_psoCreate( JNIEnv *   env,
                                         jobject    jobj,
                                         jlong      jhandle,
                                         jstring    jname,
                                         jint       jtype,
                                         jbyteArray jkeyDef,
                                         jint       jlength )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   const char * name;
   PSO_HANDLE definitionHandle;
   jbyte * keyDef;
   
   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   keyDef = (*env)->GetByteArrayElements( env, jkeyDef, NULL );
   if ( keyDef == NULL ) {
      (*env)->ReleaseStringUTFChars( env, jname, name );
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   
   errcode = psoKeyDefCreate( (PSO_HANDLE)handle,
                              name,
                              strlen(name),
                              jtype,
                              (const unsigned char *)keyDef,
                              jlength,
                              &definitionHandle );
   (*env)->ReleaseStringUTFChars( env, jname, name );
   (*env)->ReleaseByteArrayElements( env, jkeyDef, keyDef, JNI_ABORT );

   if ( errcode == PSO_OK ) {
      (*env)->SetLongField( env, jobj, g_idKeyDefHandle, 
         (size_t) definitionHandle );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_KeyDefinition
 * Method:    psoGetNext
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL 
Java_org_photon_KeyDefinition_psoGetNext( JNIEnv * env,
                                          jobject  jobj )
{
   jbyte * keyDef;
   jbyteArray jkeyDef;
   int type, i, count = 0;
   int currentLength;
   jsize keyDefLength;
   char msg[1000];
   
   jkeyDef = (*env)->GetObjectField( env, jobj, g_idKeyDefKeyDef );
   type = (*env)->GetIntField( env, jobj, g_idKeyDefType );
   currentLength = (*env)->GetIntField( env, jobj, g_idKeyDefCurrentLength );
   
   keyDef = (*env)->GetByteArrayElements( env, jkeyDef, NULL );
   if ( keyDef == NULL ) {
      (*env)->NewStringUTF( env, "" );
   }
   keyDefLength = (*env)->GetArrayLength( env, jkeyDef );

   if ( type == PSO_DEF_USER_DEFINED ) {
      
      for ( i = currentLength; i < keyDefLength; ++i ) {
         if ( keyDef[i] == 0 ) {
            currentLength = i + 1;
            break;
         }
         msg[count] = keyDef[i];
         currentLength++;
         count++;
         if ( count == 999 ) {
            // Reallocation would be better here
            msg[count] = 0;
            break;
         }
      }
   }
   else if ( type == PSO_DEF_PHOTON_ODBC_SIMPLE ) {
      /*
       * We don't need to count - msg cannot overflow with this type.
       */
      char name [PSO_MAX_FIELD_LENGTH+1];
      char tmp[20];
      uint32_t currentField = currentLength / sizeof(psoFieldDefinition);
      psoFieldDefinition * field = (psoFieldDefinition *)keyDef;
   
      strcpy( msg, "Name: " );
      
      if ( field[currentField].name[PSO_MAX_FIELD_LENGTH-1] == '\0' ) {
         strcat( msg, field[currentField].name );
      }
      else {
         memcpy( name, field[currentField].name, PSO_MAX_FIELD_LENGTH );
         name[PSO_MAX_FIELD_LENGTH] = '\0';
         strcat( msg, name );
      }
      
      strcat( msg, ", Type: " );
      switch ( field[currentField].type ) {

      case PSO_KEY_INTEGER:
         strcat( msg, "Integer" );
         break;
      case PSO_KEY_BIGINT:
         strcat( msg, "BigInt" );
         break;
      case PSO_KEY_DATE:
         strcat( msg, "Date" );
         break;
      case PSO_KEY_TIME:
         strcat( msg, "Time" );
         break;
      case PSO_KEY_TIMESTAMP:
         strcat( msg, "TimeStamp" );
         break;

      case PSO_KEY_BINARY:
         strcat( msg, "Binary, Length: " );
         sprintf( tmp, "%d", field[currentField].vals.length );
         strcat( msg, tmp );
         break;
      case PSO_KEY_CHAR:
         strcat( msg, "Char, Length: " );
         sprintf( tmp, "%d", field[currentField].vals.length );
         strcat( msg, tmp );
         break;

      case PSO_KEY_VARBINARY:
         strcat( msg, "VarBinary, Length: " );
         sprintf( tmp, "%d", field[currentField].vals.length );
         strcat( msg, tmp );
         break;

      case PSO_KEY_VARCHAR:
         strcat( msg, "VarChar, Length: " );
         sprintf( tmp, "%d", field[currentField].vals.length );
         strcat( msg, tmp );
         break;

      case PSO_KEY_LONGVARBINARY:
         strcat( msg, "LongVarBinary" );
         break;
      case PSO_KEY_LONGVARCHAR:
         strcat( msg, "LongVarChar" );
         break;

      default:
         strcat( msg, "Invalid/unknown key type" );
      }

      currentLength += sizeof(psoFieldDefinition);
   }

   (*env)->SetIntField( env, jobj, g_idKeyDefCurrentLength, currentLength );
   (*env)->ReleaseByteArrayElements( env, jkeyDef, keyDef, JNI_ABORT );

   return (*env)->NewStringUTF( env, msg );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_KeyDefinition
 * Method:    psoOpen
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_KeyDefinition_psoOpen( JNIEnv * env,
                                       jobject  jobj,
                                       jlong    jhandle,
                                       jstring  jname )
{
   int errcode;
   
   /* Native variables */
   size_t handle = (size_t) jhandle;
   const char * name;
   PSO_HANDLE definitionHandle;
   unsigned char * keyDef;
   enum psoDefinitionType defType;
   unsigned int keyDefLength;
   jbyteArray jba;
   char * dummyName;
   unsigned int dummyLength;
   
   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   
   errcode = psoKeyDefOpen( (PSO_HANDLE)handle,
                            name,
                            strlen(name),
                            &definitionHandle );
   (*env)->ReleaseStringUTFChars( env, jname, name );
   
   if ( errcode != 0 ) return errcode;
   
   errcode = psoaKeyDefGetDef( definitionHandle,
                               &dummyName,
                               &dummyLength,
                               &defType,
                               &keyDef,
                               &keyDefLength );
   if ( errcode != 0 ) {
      psoKeyDefClose( definitionHandle );
      return errcode;
   }
   
   jba = (*env)->NewByteArray( env, keyDefLength );
   if ( jba == NULL ) {
      psoKeyDefClose( definitionHandle );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   (*env)->SetByteArrayRegion( env, jba, 0, keyDefLength, (jbyte *)keyDef );

   (*env)->SetObjectField( env, jobj, g_idKeyDefKeyDef, jba );
   (*env)->SetIntField(    env, jobj, g_idKeyDefType,   defType );
   (*env)->SetLongField(   env, jobj, g_idKeyDefHandle, (size_t)definitionHandle );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

