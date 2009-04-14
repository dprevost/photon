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
//jfieldID g_id_current;
jfieldID g_id_length;

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
//   g_id_current = (*env)->GetFieldID( env, classDefinition, "currentKeyField", "J" );
//   if ( g_id_current == NULL ) return;
   g_id_length = (*env)->GetFieldID( env, classDefinition, "currentLength", "J" );
   if ( g_id_length == NULL ) return;
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
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

