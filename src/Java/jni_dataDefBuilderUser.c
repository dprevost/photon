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

jfieldID g_id_fields;
jfieldID g_id_current;
jfieldID g_id_numFields;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_DataDefBuilderUser
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_org_photon_DataDefBuilderUser_initIDs( JNIEnv * env,
                                            jclass classDefinition )
{
   g_id_fields = (*env)->GetFieldID( env, classDefinition, "fields", "[B" );
   if ( g_id_fields == NULL ) return;
   g_id_current = (*env)->GetFieldID( env, classDefinition, "currentField", "J" );
   if ( g_id_current == NULL ) return;
   g_id_numFields = (*env)->GetFieldID( env, classDefinition, "numFields", "J" );
   if ( g_id_numFields == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_DataDefBuilderUser
 * Method:    psoAddField
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_DataDefBuilderUser_psoAddField( JNIEnv * env, 
                                                jobject  jobj,
                                                jstring  jdesc )
{
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

