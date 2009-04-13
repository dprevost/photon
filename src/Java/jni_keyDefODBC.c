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

jfieldID g_idKeyDefType;
jfieldID g_idKeyDefLength;
jfieldID g_idKeyDefMinLength;
jfieldID g_idKeyDefMaxLength;
jclass   g_keyDefClass;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_KeyDefinition
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_org_photon_KeyDefinition_initIDs( JNIEnv * env, jclass keyClass )
{
   g_idKeyDefType = (*env)->GetFieldID( env, keyClass, "type", "Lorg/photon/KeyType;" );
   if ( g_idKeyDefType == NULL ) return;
   g_idKeyDefLength = (*env)->GetFieldID( env, keyClass, "length", "I" );
   if ( g_idKeyDefLength == NULL ) return;
   g_idKeyDefMinLength = (*env)->GetFieldID( env, keyClass, "minLength", "I" );
   if ( g_idKeyDefMinLength == NULL ) return;
   g_idKeyDefMaxLength = (*env)->GetFieldID( env, keyClass, "maxLength", "I" );
   if ( g_idKeyDefMaxLength == NULL ) return;

   g_keyDefClass = (*env)->NewWeakGlobalRef( env, keyClass );
   if ( g_keyDefClass == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

