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
#include "org_photon_DataRecord.h"

jfieldID g_idRecordFields;
jfieldID g_idRecordObj;
jfieldID g_idRecordconversionType;
jfieldID g_idRecordOffset;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_DataRecord
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_DataRecord_initIDs( JNIEnv * env, jclass recordClass )
{
   jclass descClass;

   g_idRecordFields = (*env)->GetFieldID( env, recordClass, "fields",
      "[Lorg/photon/DataRecord$FieldDescription;" );
   if ( g_idRecordFields == NULL ) return;
   
   descClass = (*env)->FindClass( env, "org.photon.DataRecord$FieldDescription" );
   if ( descClass == NULL ) return;
   
   g_idRecordObj = (*env)->GetFieldID( env, descClass, "obj", 
      "Ljava.lang.Object;" );
   if ( g_idRecordObj == NULL ) return;
   g_idRecordconversionType = (*env)->GetFieldID( env, descClass, 
      "conversionType", "I" );
   if ( g_idRecordconversionType == NULL ) return;
   g_idRecordOffset = (*env)->GetFieldID( env, descClass, "offset", "I" );
   if ( g_idRecordOffset == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

