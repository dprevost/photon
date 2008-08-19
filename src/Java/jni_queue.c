/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <jni.h>
#include <photon/vds.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */


/*
 * Class:     org_vdsf_VdsQueue
 * Method:    initSession
 * Signature: (Z)J
 */
JNIEXPORT jlong JNICALL Java_org_vdsf_VdsQueue_initSession (
   JNIEnv * env, 
   jobject  obj )
{
   int errcode;
   jclass exc;
   char msg[100];
   VDS_HANDLE handle;
   
   errcode = vdsInitSession( &handle );

   // Normal return
   if ( errcode == VDS_OK ) return (jlong) handle;
   
   // Throw a java exception

   exc = (*env)->FindClass( env, "org/photon/VdsException" );
   if ( exc  != NULL ) {
      sprintf( msg, "vdsf Error = %d", errcode );
      (*env)->ThrowNew( env, exc, msg );
   }

   return (jlong) NULL; 
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

