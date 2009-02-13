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
#include "org_photon_FieldDefinition.h"

jfieldID g_idFieldDefName;
jfieldID g_idFieldDefType;
jfieldID g_idFieldDefLength;
jfieldID g_idFieldDefMinLength;
jfieldID g_idFieldDefMaxLength;
jfieldID g_idFieldDefPrecision;
jfieldID g_idFieldDefScale;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_FieldDefinition
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_org_photon_FieldDefinition_initIDs(JNIEnv * env, jclass fieldClass )
{
   g_idFieldDefName = (*env)->GetFieldID( env, fieldClass, "name", "Ljava/lang/String;" );
   if ( g_idFieldDefName == NULL ) return;
   g_idFieldDefType = (*env)->GetFieldID( env, fieldClass, "type", "Lorg/photon/FieldType;" );
   if ( g_idFieldDefType == NULL ) return;
   g_idFieldDefLength = (*env)->GetFieldID( env, fieldClass, "length", "I" );
   if ( g_idFieldDefLength == NULL ) return;
   g_idFieldDefMinLength = (*env)->GetFieldID( env, fieldClass, "minLength", "I" );
   if ( g_idFieldDefMinLength == NULL ) return;
   g_idFieldDefMaxLength = (*env)->GetFieldID( env, fieldClass, "maxLength", "I" );
   if ( g_idFieldDefMaxLength == NULL ) return;
   g_idFieldDefPrecision = (*env)->GetFieldID( env, fieldClass, "precision", "I" );
   if ( g_idFieldDefPrecision == NULL ) return;
   g_idFieldDefScale = (*env)->GetFieldID( env, fieldClass, "", "scale" );
   if ( g_idFieldDefScale == NULL ) return;

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

