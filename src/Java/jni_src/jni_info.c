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
#include "org_photon_Info.h"

jfieldID g_idInfoTotalSizeInBytes;
jfieldID g_idInfoAllocatedSizeInBytes;
jfieldID g_idInfoNumObjects;
jfieldID g_idInfoNumGroups;
jfieldID g_idInfoNumMallocs;
jfieldID g_idInfoNumFrees;
jfieldID g_idInfoLargestFreeInBytes;
jfieldID g_idInfoMemoryVersion;
jfieldID g_idInfoBigEndian;
jfieldID g_idInfoCompiler;
jfieldID g_idInfoCompilerVersion;
jfieldID g_idInfoPlatform;
jfieldID g_idInfoDllVersion;
jfieldID g_idInfoQuasarVersion;
jfieldID g_idInfoCreationTime;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Info
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_org_photon_Info_initIDs( JNIEnv * env, jclass infoClass )
{
   g_idInfoTotalSizeInBytes = (*env)->GetFieldID( env, infoClass, 
      "totalSizeInBytes", "J" );
   if ( g_idInfoTotalSizeInBytes == NULL ) return;

   g_idInfoAllocatedSizeInBytes = (*env)->GetFieldID( env, infoClass,
      "allocatedSizeInBytes", "J" );
   if ( g_idInfoAllocatedSizeInBytes == NULL ) return;

   g_idInfoNumObjects = (*env)->GetFieldID( env, infoClass,
      "numObjects", "J" );
   if ( g_idInfoNumObjects == NULL ) return;

   g_idInfoNumGroups = (*env)->GetFieldID( env, infoClass,
      "numGroups", "J" );
   if ( g_idInfoNumGroups == NULL ) return;

   g_idInfoNumMallocs = (*env)->GetFieldID( env, infoClass,
      "numMallocs", "J" );
   if ( g_idInfoNumMallocs == NULL ) return;

   g_idInfoNumFrees = (*env)->GetFieldID( env, infoClass,
      "numFrees", "J" );
   if ( g_idInfoNumFrees == NULL ) return;

   g_idInfoLargestFreeInBytes = (*env)->GetFieldID( env, infoClass,
      "largestFreeInBytes", "J" );
   if ( g_idInfoLargestFreeInBytes == NULL ) return;

   g_idInfoMemoryVersion = (*env)->GetFieldID( env, infoClass,
      "memoryVersion", "I" );
   if ( g_idInfoMemoryVersion == NULL ) return;

   g_idInfoBigEndian = (*env)->GetFieldID( env, infoClass,
      "bigEndian", "I" );
   if ( g_idInfoBigEndian == NULL ) return;

   g_idInfoCompiler = (*env)->GetFieldID( env, infoClass,
      "compiler", "Ljava/lang/String;" );
   if ( g_idInfoCompiler == NULL ) return;

   g_idInfoCompilerVersion = (*env)->GetFieldID( env, infoClass,
      "compilerVersion", "Ljava/lang/String;" );
   if ( g_idInfoCompilerVersion == NULL ) return;

   g_idInfoPlatform = (*env)->GetFieldID( env, infoClass,
      "platform", "Ljava/lang/String;" );
   if ( g_idInfoPlatform == NULL ) return;

   g_idInfoDllVersion = (*env)->GetFieldID( env, infoClass,
      "dllVersion", "Ljava/lang/String;" );
   if ( g_idInfoDllVersion == NULL ) return;

   g_idInfoQuasarVersion = (*env)->GetFieldID( env, infoClass,
      "quasarVersion", "Ljava/lang/String;" );
   if ( g_idInfoQuasarVersion == NULL ) return;

   g_idInfoCreationTime = (*env)->GetFieldID( env, infoClass,
      "creationTime", "Ljava/lang/String;" );
   if ( g_idInfoCreationTime == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

