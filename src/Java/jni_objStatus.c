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
#include "org_photon_ObjectStatus.h"

jfieldID g_idStatusType;
jfieldID g_idStatusStatus;
jfieldID g_idStatusNumBlocks;
jfieldID g_idStatusNumBlockGroup;
jfieldID g_idStatusNumDataItem;
jfieldID g_idStatusFreeBytes;
jfieldID g_idStatusMaxDataLength;
jfieldID g_idStatusMaxKeyLength;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_ObjectStatus
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_ObjectStatus_initIDs( JNIEnv * env, jclass statusClass )
{
   g_idStatusType = (*env)->GetFieldID( env, statusClass, "type", "I" );
   if ( g_idStatusType == NULL ) return;
   g_idStatusStatus = (*env)->GetFieldID( env, statusClass, "status", "I" );
   if ( g_idStatusStatus == NULL ) return;
   g_idStatusNumBlocks = (*env)->GetFieldID( env, statusClass, "numBlocks", "J" );
   if ( g_idStatusNumBlocks == NULL ) return;
   g_idStatusNumBlockGroup = (*env)->GetFieldID( env, statusClass, "numBlockGroup", "J" );
   if ( g_idStatusNumBlockGroup == NULL ) return;
   g_idStatusNumDataItem = (*env)->GetFieldID( env, statusClass, "numDataItem", "J" );
   if ( g_idStatusNumDataItem == NULL ) return;
   g_idStatusFreeBytes = (*env)->GetFieldID( env, statusClass, "freeBytes", "J" );
   if ( g_idStatusFreeBytes == NULL ) return;
   g_idStatusMaxDataLength = (*env)->GetFieldID( env, statusClass, "maxDataLength", "I" );
   if ( g_idStatusMaxDataLength == NULL ) return;
   g_idStatusMaxKeyLength = (*env)->GetFieldID( env, statusClass, "maxKeyLength", "I" );
   if ( g_idStatusMaxKeyLength == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

