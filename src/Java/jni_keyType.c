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
#include "org_photon_KeyType.h"

jfieldID g_idKeyTypeType;

jweak * g_weakKeyType;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_KeyType
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_org_photon_KeyType_initIDs( JNIEnv * env , jclass keyClass )
{
   jobject jobj;
   jfieldID id;
   jclass exc;

   g_idKeyTypeType = (*env)->GetFieldID( env, keyClass, "type", "I" );
   if ( g_idKeyTypeType == NULL ) return;

   g_weakKeyType = malloc( sizeof(jweak)*5 );
   if ( g_weakKeyType == NULL ) {
      exc = (*env)->FindClass(env, "java/lang/OutOfMemoryError");
      if ( exc != NULL ) {
         (*env)->ThrowNew( env, exc, "malloc failed in jni code");
      }
      return;
   }
   
   id = (*env)->GetStaticFieldID(env, keyClass, "INTEGER", "Lorg/photon/KeyType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, keyClass, id );
   if ( jobj == NULL ) return;
   g_weakKeyType[0] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakKeyType[0] == NULL ) return;

   id = (*env)->GetStaticFieldID(env, keyClass, "BINARY", "Lorg/photon/KeyType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, keyClass, id );
   if ( jobj == NULL ) return;
   g_weakKeyType[1] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakKeyType[1] == NULL ) return;

   id = (*env)->GetStaticFieldID(env, keyClass, "STRING", "Lorg/photon/KeyType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, keyClass, id );
   if ( jobj == NULL ) return;
   g_weakKeyType[2] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakKeyType[2] == NULL ) return;

   id = (*env)->GetStaticFieldID(env, keyClass, "VAR_STRING", "Lorg/photon/KeyType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, keyClass, id );
   if ( jobj == NULL ) return;
   g_weakKeyType[3] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakKeyType[3] == NULL ) return;

   id = (*env)->GetStaticFieldID(env, keyClass, "VAR_BINARY", "Lorg/photon/KeyType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, keyClass, id );
   if ( jobj == NULL ) return;
   g_weakKeyType[4] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakKeyType[4] == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

