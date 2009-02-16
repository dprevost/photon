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
#include "org_photon_FieldType.h"

jfieldID g_idFieldTypeType;
jweak *  g_weakFieldType;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_FieldType
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_org_photon_FieldType_initIDs( JNIEnv * env , jclass typeClass )
{
   jobject jobj;
   jfieldID id;
   jclass exc;
   
   g_idFieldTypeType = (*env)->GetFieldID( env, typeClass, "type", "I" );
   if ( g_idFieldTypeType == NULL ) return;

   g_weakFieldType = malloc( sizeof(jweak)*7 );
   if ( g_weakFieldType == NULL ) {
      exc = (*env)->FindClass(env, "java/lang/OutOfMemoryError");
      if ( exc != NULL ) {
         (*env)->ThrowNew( env, exc, "malloc failed in jni code");
      }
      return;
   }
   
   id = (*env)->GetStaticFieldID(env, typeClass, "INTEGER", "Lorg/photon/FieldType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, typeClass, id );
   if ( jobj == NULL ) return;
   g_weakFieldType[PSO_INTEGER-1] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakFieldType[PSO_INTEGER-1] == NULL ) return;

   id = (*env)->GetStaticFieldID(env, typeClass, "BINARY", "Lorg/photon/FieldType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, typeClass, id );
   if ( jobj == NULL ) return;
   g_weakFieldType[PSO_BINARY-1] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakFieldType[PSO_BINARY-1] == NULL ) return;

   id = (*env)->GetStaticFieldID(env, typeClass, "STRING", "Lorg/photon/FieldType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, typeClass, id );
   if ( jobj == NULL ) return;
   g_weakFieldType[PSO_STRING-1] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakFieldType[PSO_STRING-1] == NULL ) return;

   id = (*env)->GetStaticFieldID(env, typeClass, "DECIMAL", "Lorg/photon/FieldType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, typeClass, id );
   if ( jobj == NULL ) return;
   g_weakFieldType[PSO_DECIMAL-1] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakFieldType[PSO_DECIMAL-1] == NULL ) return;

   id = (*env)->GetStaticFieldID(env, typeClass, "BOOLEAN", "Lorg/photon/FieldType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, typeClass, id );
   if ( jobj == NULL ) return;
   g_weakFieldType[PSO_BOOLEAN-1] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakFieldType[PSO_BOOLEAN-1] == NULL ) return;

   id = (*env)->GetStaticFieldID(env, typeClass, "VAR_BINARY", "Lorg/photon/FieldType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, typeClass, id );
   if ( jobj == NULL ) return;
   g_weakFieldType[PSO_VAR_BINARY-1] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakFieldType[PSO_VAR_BINARY-1] == NULL ) return;

   id = (*env)->GetStaticFieldID(env, typeClass, "VAR_STRING", "Lorg/photon/FieldType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, typeClass, id );
   if ( jobj == NULL ) return;
   g_weakFieldType[PSO_VAR_STRING-1] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakFieldType[PSO_VAR_STRING-1] == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

