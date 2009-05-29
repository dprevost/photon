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
#include "org_photon_serializer_StringSerializer.h"

jfieldID g_idDataDef;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_serializer_StringSerializer
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_org_photon_serializer_StringSerializer_initIDs( JNIEnv * env,
                                                     jclass   class )
{
   g_idDataDef = (*env)->GetFieldID( env, class, "dataDef", "Ljava/util/ArrayList;" );
   if ( g_idDataDef == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_serializer_StringSerializer
 * Method:    psoPackObject
 * Signature: (Ljava/lang/String;)[B
 */
JNIEXPORT jbyteArray JNICALL 
Java_org_photon_serializer_StringSerializer_psoPackObject( JNIEnv * env,
                                                           jobject  jobj,
                                                           jstring  jstring )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_serializer_StringSerializer
 * Method:    psoUnpackObject
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_photon_serializer_StringSerializer_psoUnpackObject( JNIEnv   * env,
                                                             jobject    jobj,
                                                             jbyteArray jba )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_serializer_StringSerializer
 * Method:    psoValidate
 * Signature: ([B)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_serializer_StringSerializer_psoValidate( JNIEnv * env,
                                                         jobject  jobject,
                                                         jbyteArray jdataDef )
{
   jsize size;
   int numFields;
   psoFieldDefinition * dataDef;

   size = (*env)->GetArrayLength( env, jdataDef );
   numFields = size/sizeof(psoFieldDefinition);
   
   if ( numFields != 1 ) return 3; // INVALID_NUM_FIELDS_IN_DATA_DEF;
   
   dataDef = (psoFieldDefinition *)(*env)->GetByteArrayElements( env, jdataDef, NULL );
   if ( dataDef == NULL ) return 4; // OUT_OF_MEMORY_EXCEPTION;

   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

