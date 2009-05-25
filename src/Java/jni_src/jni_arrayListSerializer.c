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
#include "org_photon_serializer_ArrayListSerializer.h"
//#include "API/Queue.h"

//jfieldID g_idQueueHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_BaseQueue
 * Method:    initIDs
 * Signature: ()V
 */
//JNIEXPORT void JNICALL
//Java_org_photon_ArrayListSerializer_initIDs( JNIEnv * env, jclass queueClass )
//{
//   g_idQueueHandle = (*env)->GetFieldID( env, queueClass, "handle", "J" );
//   if ( g_idQueueHandle == NULL ) return;
//}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_ArrayListSerializer
 * Method:    psoPackObject
 * Signature: (Ljava/util/ArrayList;)[B
 */
JNIEXPORT jbyteArray JNICALL
Java_org_photon_ArrayListSerializer_psoPackObject( JNIEnv * env,
                                                   jobject  jobj,
                                                   jobject  jlist )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_ArrayListSerializer
 * Method:    psoUnpackObject
 * Signature: ([B)Ljava/util/ArrayList;
 */
JNIEXPORT jobject JNICALL
Java_org_photon_ArrayListSerializer_psoUnpackObject( JNIEnv   * env,
                                                     jobject    jobj,
                                                     jbyteArray jdata )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

