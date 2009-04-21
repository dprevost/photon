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
#include "org_photon_ObjectDefinition.h"

jfieldID g_idObjDefType;
jfieldID g_idObjDefFlags;
jfieldID g_idObjDefMinNumOfDataRecords;
jfieldID g_idObjDefMinNumBlocks;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_ObjectDefinition
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_org_photon_ObjectDefinition_initIDs( JNIEnv * env, jclass defClass )
{
   g_idObjDefType = (*env)->GetFieldID( env, defClass, "type", "I" );
   if ( g_idObjDefType == NULL ) return;
   g_idObjDefFlags = (*env)->GetFieldID( env, defClass, "flags", "I" );
   if ( g_idObjDefFlags == NULL ) return;
   g_idObjDefMinNumOfDataRecords = (*env)->GetFieldID( env, defClass, "minNumOfDataRecords", "J" );
   if ( g_idObjDefMinNumOfDataRecords == NULL ) return;
   g_idObjDefMinNumBlocks = (*env)->GetFieldID( env, defClass, "minNumBlocks", "J" );
   if ( g_idObjDefMinNumBlocks == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

