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
#include "org_photon_Queue.h"

jfieldID g_idQueueHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_Queue_initIDs( JNIEnv * env, jclass queueClass )
{
   g_idQueueHandle = (*env)->GetFieldID( env, queueClass, "handle", "J" );
   if ( g_idQueueHandle == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    psoFini
 * Signature: (J)V
 */
JNIEXPORT void JNICALL 
Java_org_photon_Queue_psoFini( JNIEnv  * env,
                               jobject   jobj,
                               jlong     jhandle )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   
   errcode = psoQueueClose( (PSO_HANDLE)handle );

   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    psoGetFirst
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Queue_psoGetFirst( JNIEnv  * env,
                                   jobject   jobj,
                                   jlong     jhandle,
                                   jobject   jrecord );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    psoGetNext
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Queue_psoGetNext( JNIEnv  * env,
                                  jobject   jobj,
                                  jlong     jhandle,
                                  jobject   jrecord );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    psoGetStatus
 * Signature: (JLorg/photon/ObjectStatus;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Queue_psoGetStatus( JNIEnv  * env,
                                    jobject   jobj,
                                    jlong     jhandle,
                                    jobject   jstatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    psoInit
 * Signature: (Lorg/photon/Session;Ljava/lang/String;Lorg/photon/Definition;Lorg/photon/ObjectDefinition;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Queue_psoInit( JNIEnv  * env,
                               jobject   jobj,
                               jobject   jhandle,
                               jstring   jname,
                               jobject   jdef,
                               jobject   jbase );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    psoPop
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Queue_psoPop( JNIEnv  * env,
                              jobject   jobj,
                              jlong     jhandle,
                              jobject   jrecord );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    psoPush
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Queue_psoPush( JNIEnv  * env,
                               jobject   jobj,
                               jlong     jhandle,
                               jobject   jrecord );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Queue
 * Method:    psoPushNow
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Queue_psoPushNow( JNIEnv  * env,
                                  jobject   jobj,
                                  jlong     jhandle,
                                  jobject   jrecord );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

