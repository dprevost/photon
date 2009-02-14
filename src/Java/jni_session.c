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
#include "org_photon_PhotonSession.h"

jfieldID g_idSessionHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonSession
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_PhotonSession_initIDs( JNIEnv * env, jclass sessionClass )
{
   g_idSessionHandle = (*env)->GetFieldID( env, sessionClass, "handle", "J" );
   if ( g_idSessionHandle == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonSession
 * Method:    initSession
 * Signature: ()I
 */
JNIEXPORT jlong JNICALL
Java_org_photon_PhotonSession_initSession( JNIEnv * env,
                                           jobject  obj )
{
   int errcode;
   PSO_HANDLE handle;
   
   errcode = psoInitSession( &handle );

   // Normal return
   if ( errcode == PSO_OK ) {
      (*env)->SetLongField( env, obj, g_idSessionHandle, (size_t) handle );
   }
   
   return errcode;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

