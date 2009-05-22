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
#include "org_photon_FolderEntry.h"

jfieldID g_idEntryType;
jfieldID g_idEntryName;
jfieldID g_idEntryStatus;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_FolderEntry
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_FolderEntry_initIDs( JNIEnv * env, jclass entryClass )
{
   g_idEntryType   = (*env)->GetFieldID( env, entryClass, "type", "I");
   if ( g_idEntryType == NULL ) return;
   g_idEntryName   = (*env)->GetFieldID( env, entryClass, "name", "Ljava/lang/String;" );
   if ( g_idEntryName == NULL ) return;
   g_idEntryStatus = (*env)->GetFieldID( env, entryClass, "status", "I" );
   if ( g_idEntryStatus == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

