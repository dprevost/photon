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
#include "org_photon_PhotonQueue.h"

jfieldID g_idQueueHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonQueue
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_PhotonQueue_initIDs( JNIEnv * env, jclass queueClass )
{
   g_idQueueHandle = (*env)->GetFieldID( env, queueClass, "handle", "J" );
   if ( g_idQueueHandle == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonQueue
 * Method:    fini
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_PhotonQueue_fini( JNIEnv * env, jobject obj, jlong h );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonQueue
 * Method:    init
 * Signature: (Lorg/photon/PhotonSession;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_PhotonQueue_init( JNIEnv  * env,
                                  jobject   obj,
                                  jobject   jsession,
                                  jstring   jname );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonQueue
 * Method:    pop
 * Signature: (J[BI[I)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_PhotonQueue_pop( JNIEnv  * env,
                                 jobject   obj,
                                 jlong     h,
                                 jbyteArray,
                                 jint,
                                 jintArray );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonQueue
 * Method:    push
 * Signature: (J[BI)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_PhotonQueue_push( JNIEnv  * env,
                                  jobject   obj,
                                  jlong     h,
                                  jbyteArray, 
                                  jint );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonQueue
 * Method:    pushNow
 * Signature: (J[BI)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_PhotonQueue_pushNow( JNIEnv  * env,
                                     jobject   obj,
                                     jlong     h,
                                     jbyteArray, 
                                     jint );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonQueue
 * Method:    retrieveDefinition
 * Signature: (JLorg/photon/ObjectDefinition;I[Lorg/photon/FieldDefinition;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_PhotonQueue_retrieveDefinition( JNIEnv     * env, 
                                                jobject      obj,
                                                jlong        h,
                                                jobject      jdef,
                                                jint         numFields,
                                                jobjectArray jfields );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonQueue
 * Method:    retrieveFirst
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_PhotonQueue_retrieveFirst( JNIEnv  * env,
                                           jobject   obj,
                                           jlong     h,
                                           jobject   jrecord );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonQueue
 * Method:    retrieveNext
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_PhotonQueue_retrieveNext( JNIEnv  * env,
                                          jobject   obj,
                                          jlong     h,
                                          jobject   jrecord );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonQueue
 * Method:    retrieveStatus
 * Signature: (JLorg/photon/ObjStatus;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_PhotonQueue_retrieveStatus( JNIEnv * env,
                                            jobject  obj,
                                            jlong    h,
                                            jobject  jstatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

