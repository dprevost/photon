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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderCreateObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonFolder_folderCreateObject( JNIEnv * env, 
                                                 jobject  obj, 
                                                 jlong    h, 
                                                 jstring  name, 
                                                 jobject  def )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderCreateObjectXML
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonFolder_folderCreateObjectXML( JNIEnv * env, 
                                                    jobject  obj, 
                                                    jlong    h, 
                                                    jstring  xml )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderDestroyObject
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonFolder_folderDestroyObject( JNIEnv * env, 
                                                  jobject  obj, 
                                                  jlong    h, 
                                                  jstring  name )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderFini
 * Signature: (J)V
 */
JNIEXPORT void JNICALL 
Java_org_photon_PhotonFolder_folderFini( JNIEnv * env, 
                                         jobject  obj, 
                                         jlong    h )
{
   int errcode;
   size_t handle = (size_t) h;
   
   errcode = psoFolderClose( (PSO_HANDLE)handle );

   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderGetFirst
 * Signature: (JLorg/photon/FolderEntry;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonFolder_folderGetFirst( JNIEnv * env, 
                                             jobject  obj, 
                                             jlong    h,
                                             jobject  entry )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderGetNext
 * Signature: (JLorg/photon/FolderEntry;)I
 */
JNIEXPORT jboolean JNICALL 
Java_org_photon_PhotonFolder_folderGetNext( JNIEnv * env, 
                                            jobject  obj, 
                                            jlong    h,
                                            jobject  entry )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderInit
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonFolder_folderInit( JNIEnv * env, 
                                         jobject  obj, 
                                         jlong    h, 
                                         jstring  jstr )
{
   int errcode;
   jclass myClass;
   PSO_HANDLE handle;
   size_t sessionHandle = (size_t)h;
   const char *folderName;
   jfieldID id;
   
   folderName = (*env)->GetStringUTFChars( env, jstr, NULL );
   if ( folderName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   
   myClass = (*env)->FindClass( env, "org/photon/PhotonFolder" );

   id = (*env)->GetFieldID( env, myClass, "handle", "L" );

   errcode = psoFolderOpen( (PSO_HANDLE) sessionHandle,
                            folderName,
                            strlen(folderName),
                            &handle );
   (*env)->ReleaseStringUTFChars( env, jstr, folderName );

   if ( errcode == PSO_OK ) {
      (*env)->SetLongField( env, obj, id, (size_t) handle );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderStatus
 * Signature: (JLorg/photon/ObjStatus;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_PhotonFolder_folderStatus( JNIEnv * env, 
                                           jobject  obj, 
                                           jlong    h, 
                                           jobject  status )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

  /////////////////////////////////////

#if 0
/*
 * Class:     org_photon_psoFolder
 * Method:    Close
 */
 
JNIEXPORT jlong JNICALL Java_org_photon_psoFolder_Close (
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

   exc = (*env)->FindClass( env, "org/photon/psoException" );
   if ( exc  != NULL ) {
      sprintf( msg, "photon Error = %d", errcode );
      (*env)->ThrowNew( env, exc, msg );
   }

   return (jlong) NULL; 
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_psoFolder
 * Method:    init
 * Signature: (JLjava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_photon_psoFolder_init(
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
      return (size_t) NULL; // out-of-memory exception by the JVM
   }
   
   errcode = psoFolderOpen( (PSO_HANDLE) sessionHandle,
                            folderName,
                            strlen(folderName),
                            &handle );
  (*env)->ReleaseStringUTFChars( env, jstr, folderName );   

   // Normal return
   if ( errcode == PSO_OK ) return (size_t) handle;
   
   // Throw a java exception
   exc = (*env)->FindClass( env, "org/photon/psoException" );
   if ( exc  != NULL ) {
      sprintf( msg, "photon Error = %d", errcode );
      (*env)->ThrowNew( env, exc, msg );
   }

   return (size_t) NULL; 
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderCreateObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;)V
 */
JNIEXPORT void JNICALL Java_org_photon_PhotonFolder_folderCreateObject
  (JNIEnv *, jobject, jlong, jstring, jobject);

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderCreateObjectXML
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_photon_PhotonFolder_folderCreateObjectXML
  (JNIEnv *, jobject, jlong, jstring);

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderDestroyObject
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_photon_PhotonFolder_folderDestroyObject
  (JNIEnv *, jobject, jlong, jstring);

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderFini
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL 
Java_org_photon_PhotonFolder_folderFini( JNIEnv * env, 
                                         jobject  obj, 
                                         jlong    handle )
{
   int errcode;
//   PSO_HANDLE handle = (Psize_t) h;
   
   errcode = psoFolderClose( (PSO_HANDLE)handle );

   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderGetFirst
 * Signature: (JILjava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_photon_PhotonFolder_folderGetFirst
  (JNIEnv *, jobject, jlong, jint, jstring, jint);

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderGetNext
 * Signature: (JILjava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_photon_PhotonFolder_folderGetNext
  (JNIEnv *, jobject, jlong, jint, jstring, jint);

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderInit
 * Signature: (JLjava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_photon_PhotonFolder_folderInit
  (JNIEnv *, jobject, jlong, jstring);

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_PhotonFolder
 * Method:    folderOpen
 * Signature: (JLjava/lang/String;)J
 */
   
#endif
