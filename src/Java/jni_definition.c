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
#include "org_photon_Definition.h"

jfieldID g_idDefinitionDef;
jfieldID g_idDefinitionKey;
jfieldID g_idDefinitionFields;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Definition
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_Definition_initIDs( JNIEnv * env, jclass defClass )
{
   g_idDefinitionDef = (*env)->GetFieldID( env, defClass, "definition", "Lorg/photon/ObjectDefinition;" );
   if ( g_idDefinitionDef == NULL ) return;
   g_idDefinitionKey = (*env)->GetFieldID( env, defClass, "key", "Lorg/photon/KeyDefinition;" );
   if ( g_idDefinitionKey == NULL ) return;
   g_idDefinitionFields = (*env)->GetFieldID( env, defClass, "fields", "[Lorg/photon/FieldDefinition;" );
   if ( g_idDefinitionFields == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

