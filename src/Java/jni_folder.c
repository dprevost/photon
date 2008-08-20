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
#include <photon/photon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_vdsf_VdsFolder
 * Method:    Close
 */
JNIEXPORT jlong JNICALL Java_org_vdsf_VdsFolder_Close (
   JNIEnv * env, 
   jobject  obj )
{
   int errcode;
   jclass exc;
   char msg[100];
   PSO_HANDLE handle;
   
   errcode = psoFolderClose( handle );

   // Normal return
   if ( errcode == PSO_OK ) return (jlong) handle;
   
   // Throw a java exception

   exc = (*env)->FindClass( env, "org/photon/VdsException" );
   if ( exc  != NULL ) {
      sprintf( msg, "vdsf Error = %d", errcode );
      (*env)->ThrowNew( env, exc, msg );
   }

   return (jlong) NULL; 
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_vdsf_VdsFolder
 * Method:    init
 * Signature: (JLjava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_vdsf_VdsFolder_init(
   JNIEnv * env, 
   jobject  obj, 
   jlong    sessionHandle, 
   jstring  jstr )
{
   int errcode;
   jclass exc;
   char msg[100];
   PSO_HANDLE handle;
   const char *folderName = (*env)->GetStringUTFChars( env, jstr, NULL );
   if ( folderName == NULL ) {
      return (jlong) NULL; // out-of-memory exception by the JVM
   }
   
   errcode = psoFolderOpen( (PSO_HANDLE) sessionHandle,
                            folderName,
                            strlen(folderName),
                            &handle );
  (*env)->ReleaseStringUTFChars( env, jstr, folderName );   

   // Normal return
   if ( errcode == PSO_OK ) return (jlong) handle;
   
   // Throw a java exception
   exc = (*env)->FindClass( env, "org/photon/VdsException" );
   if ( exc  != NULL ) {
      sprintf( msg, "vdsf Error = %d", errcode );
      (*env)->ThrowNew( env, exc, msg );
   }

   return (jlong) NULL; 
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

