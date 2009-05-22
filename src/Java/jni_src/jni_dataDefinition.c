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
#include "org_photon_DataDefinition.h"
#include "API/DataDefinition.h"

jfieldID g_idDataDefHandle;
jfieldID g_idDataDefDataDef;
jfieldID g_idDataDefType;
jfieldID g_idDataDefCurrentLength;
jfieldID g_idDataDefName;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_DataDefinition
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_DataDefinition_initIDs( JNIEnv * env,
                                        jclass   classDefinition )
{
   g_idDataDefHandle  = (*env)->GetFieldID( env, classDefinition, "handle", "J" );
   if ( g_idDataDefHandle == NULL ) return;
   g_idDataDefDataDef = (*env)->GetFieldID( env, classDefinition, "dataDef", "[B" );
   if ( g_idDataDefDataDef == NULL ) return;
   g_idDataDefType = (*env)->GetFieldID( env, classDefinition, "type", "J" );
   if ( g_idDataDefType == NULL ) return;
   g_idDataDefCurrentLength = (*env)->GetFieldID( env, classDefinition, 
      "currentLength", "J" );
   if ( g_idDataDefCurrentLength == NULL ) return;
   g_idDataDefName = (*env)->GetFieldID( env, classDefinition, 
      "name", "Ljava/lang/String;" );
   if ( g_idDataDefName == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_DataDefinition
 * Method:    psoClose
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_DataDefinition_psoClose( JNIEnv * env,
                                         jobject  jobj,
                                         jlong    jhandle )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;

   errcode = psoDataDefClose( (PSO_HANDLE)handle );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_DataDefinition
 * Method:    psoCreate
 * Signature: (JLjava/lang/String;I[BI)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_DataDefinition_psoCreate( JNIEnv *   env,
                                          jobject    jobj,
                                          jlong      jhandle,
                                          jstring    jname,
                                          jint       jtype,
                                          jbyteArray jdataDef,
                                          jint       jlength )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   const char * name;
   PSO_HANDLE definitionHandle;
   jbyte * dataDef;
   
   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   dataDef = (*env)->GetByteArrayElements( env, jdataDef, NULL );
   if ( dataDef == NULL ) {
      (*env)->ReleaseStringUTFChars( env, jname, name );
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   
   errcode = psoDataDefCreate( (PSO_HANDLE)handle,
                               name,
                               strlen(name),
                               jtype,
                               (const unsigned char *)dataDef,
                               jlength,
                               &definitionHandle );
   (*env)->ReleaseStringUTFChars( env, jname, name );
   (*env)->ReleaseByteArrayElements( env, jdataDef, dataDef, JNI_ABORT );

   if ( errcode == PSO_OK ) {
      (*env)->SetLongField( env, jobj, g_idDataDefHandle, 
         (size_t) definitionHandle );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_DataDefinition
 * Method:    psoGetNext
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL 
Java_org_photon_DataDefinition_psoGetNext( JNIEnv * env,
                                           jobject  jobj )
{
   jbyte * dataDef;
   jbyteArray jdataDef;
   int type, i, count = 0;
   int currentLength;
   jsize dataDefLength;
   char msg[1000];
   
   jdataDef = (*env)->GetObjectField( env, jobj, g_idDataDefDataDef );
   type = (*env)->GetIntField( env, jobj, g_idDataDefType );
   currentLength = (*env)->GetIntField( env, jobj, g_idDataDefCurrentLength );
   
   dataDef = (*env)->GetByteArrayElements( env, jdataDef, NULL );
   if ( dataDef == NULL ) {
//      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   dataDefLength = (*env)->GetArrayLength( env, jdataDef );

   if ( type == PSO_DEF_USER_DEFINED ) {
      
      for ( i = currentLength; i < dataDefLength; ++i ) {
         if ( dataDef[i] == 0 ) {
            currentLength = i + 1;
            break;
         }
         msg[count] = dataDef[i];
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
      psoFieldDefinition * field = (psoFieldDefinition *)dataDef;
   
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

      case PSO_TINYINT:
         strcat( msg, "TinyInt" );
         break;
      case PSO_SMALLINT:
         strcat( msg, "SmallInt" );
         break;
      case PSO_INTEGER:
         strcat( msg, "Integer" );
         break;
      case PSO_BIGINT:
         strcat( msg, "BigInt" );
         break;
      case PSO_REAL:
         strcat( msg, "Real" );
         break;
      case PSO_DOUBLE:
         strcat( msg, "Double" );
         break;
      case PSO_DATE:
         strcat( msg, "Date" );
         break;
      case PSO_TIME:
         strcat( msg, "Time" );
         break;
      case PSO_TIMESTAMP:
         strcat( msg, "TimeStamp" );
         break;

      case PSO_BINARY:
         strcat( msg, "Binary, Length: " );
         sprintf( tmp, "%d", field[currentField].vals.length );
         strcat( msg, tmp );
         break;
      case PSO_CHAR:
         strcat( msg, "Char, Length: " );
         sprintf( tmp, "%d", field[currentField].vals.length );
         strcat( msg, tmp );
         break;

      case PSO_VARBINARY:
         strcat( msg, "VarBinary, Length: " );
         sprintf( tmp, "%d", field[currentField].vals.length );
         strcat( msg, tmp );
         break;

      case PSO_VARCHAR:
         strcat( msg, "VarChar, Length: " );
         sprintf( tmp, "%d", field[currentField].vals.length );
         strcat( msg, tmp );
         break;

      case PSO_LONGVARBINARY:
         strcat( msg, "LongVarBinary" );
         break;
      case PSO_LONGVARCHAR:
         strcat( msg, "LongVarChar" );
         break;

      case PSO_NUMERIC:
         strcat( msg, "Numeric, Precision = " );
         sprintf( tmp, "%d", field[currentField].vals.decimal.precision );
         strcat( msg, tmp );
         strcat( msg, ", Scale = " );
         sprintf( tmp, "%d", field[currentField].vals.decimal.scale );
         strcat( msg, tmp );
         break;

      default:
         strcat( msg, "Invalid/unknown data type" );
      }

      currentLength += sizeof(psoFieldDefinition);
   }

   (*env)->SetIntField( env, jobj, g_idDataDefCurrentLength, currentLength );
   (*env)->ReleaseByteArrayElements( env, jdataDef, dataDef, JNI_ABORT );

   return (*env)->NewStringUTF( env, msg );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_DataDefinition
 * Method:    psoOpen
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_DataDefinition_psoOpen( JNIEnv * env,
                                        jobject  jobj,
                                        jlong    jhandle,
                                        jstring  jname )
{
   int errcode;
   
   /* Native variables */
   size_t handle = (size_t) jhandle;
   const char * name;
   PSO_HANDLE definitionHandle;
   unsigned char * dataDef;
   enum psoDefinitionType defType;
   unsigned int dataDefLength;
   jbyteArray jba;
   char * dummyName;
   unsigned int dummyLength;
   
   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   
   errcode = psoDataDefOpen( (PSO_HANDLE)handle,
                             name,
                             strlen(name),
                             &definitionHandle );
   (*env)->ReleaseStringUTFChars( env, jname, name );
   
   if ( errcode != 0 ) return errcode;
   
   errcode = psoaDataDefGetDef( definitionHandle,
                                &dummyName,
                                &dummyLength,
                                &defType,
                                &dataDef,
                                &dataDefLength );
   if ( errcode != 0 ) {
      psoDataDefClose( definitionHandle );
      return errcode;
   }
   
   //g_idDataDefDataDef
   jba = (*env)->NewByteArray( env, dataDefLength );
   if ( jba == NULL ) {
      psoDataDefClose( definitionHandle );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   (*env)->SetByteArrayRegion( env, jba, 0, dataDefLength, (jbyte *)dataDef );

   (*env)->SetObjectField( env, jobj, g_idDataDefDataDef, jba );
   (*env)->SetIntField(    env, jobj, g_idDataDefType,    defType );
   (*env)->SetLongField(   env, jobj, g_idDataDefHandle,  (size_t)definitionHandle );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

