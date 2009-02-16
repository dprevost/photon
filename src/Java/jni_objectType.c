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
#include "org_photon_ObjectType.h"

jfieldID g_idObjTypeType;
jweak *  g_weakObjType;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_ObjectType
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_org_photon_ObjectType_initIDs( JNIEnv * env , jclass objClass )
{
   jobject jobj;
   jfieldID id;
   jclass exc;
   
   g_idObjTypeType = (*env)->GetFieldID( env, objClass, "type", "I" );
   if ( g_idObjTypeType == NULL ) return;

   g_weakObjType = malloc( sizeof(jweak)*(PSO_LAST_OBJECT_TYPE-1) );
   if ( g_weakObjType == NULL ) {
      exc = (*env)->FindClass(env, "java/lang/OutOfMemoryError");
      if ( exc != NULL ) {
         (*env)->ThrowNew( env, exc, "malloc failed in jni code");
      }
      return;
   }
   
   id = (*env)->GetStaticFieldID(env, objClass, "FOLDER", "Lorg/photon/ObjectType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, objClass, id );
   if ( jobj == NULL ) return;
   g_weakObjType[PSO_FOLDER-1] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakObjType[PSO_FOLDER-1] == NULL ) return;

   id = (*env)->GetStaticFieldID(env, objClass, "HASH_MAP", "Lorg/photon/ObjectType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, objClass, id );
   if ( jobj == NULL ) return;
   g_weakObjType[PSO_HASH_MAP-1] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakObjType[PSO_HASH_MAP-1] == NULL ) return;

   id = (*env)->GetStaticFieldID(env, objClass, "LIFO", "Lorg/photon/ObjectType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, objClass, id );
   if ( jobj == NULL ) return;
   g_weakObjType[PSO_LIFO-1] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakObjType[PSO_LIFO-1] == NULL ) return;
   
   id = (*env)->GetStaticFieldID(env, objClass, "FAST_MAP", "Lorg/photon/ObjectType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, objClass, id );
   if ( jobj == NULL ) return;
   g_weakObjType[PSO_FAST_MAP-1] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakObjType[PSO_FAST_MAP-1] == NULL ) return;

   id = (*env)->GetStaticFieldID(env, objClass, "QUEUE", "Lorg/photon/ObjectType;");
   if ( id == NULL ) return;
   jobj = (*env)->GetStaticObjectField( env, objClass, id );
   if ( jobj == NULL ) return;
   g_weakObjType[PSO_QUEUE-1] = (*env)->NewWeakGlobalRef( env, jobj );
   if ( g_weakObjType[PSO_QUEUE-1] == NULL ) return;

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

