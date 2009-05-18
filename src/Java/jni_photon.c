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
#include "org_photon_Photon.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Photon
 * Method:    psoInit
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT int JNICALL
Java_org_photon_Photon_psoInit( JNIEnv  * env,
                                jobject   obj, 
                                jstring   jaddress,
                                jstring   jname )
{
   int errcode;
   const char * address;
   const char * name;
   
   address = (*env)->GetStringUTFChars( env, jaddress, NULL );
   if ( address == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      (*env)->ReleaseStringUTFChars( env, jaddress, address );
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   
   errcode = psoInit( address, name );
   (*env)->ReleaseStringUTFChars( env, jaddress, address );
   (*env)->ReleaseStringUTFChars( env, jname, name );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Photon
 * Method:    psoFini
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_org_photon_Photon_psoFini( JNIEnv * env, jobject obj )
{
   psoExit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

