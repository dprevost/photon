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

#ifndef JNI_PHOTON_H
#define JNI_PHOTON_H

#include <jni.h>

/*
 * This header file declares variables that we will cache to speedup
 * the jni.
 *
 * WARNING: The jclass fields must be global references, not local
 *          references. They should be weak references however if they
 *          are initialized by the class static initializer.
 */
 
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The DataDefinition class */
extern jfieldID g_idDataDefHandle;
extern jfieldID g_idDataDefDataDef;
extern jfieldID g_idDataDefType;
extern jfieldID g_idDataDefName;

/* The FolderEntry class */

extern jfieldID g_idEntryType;
extern jfieldID g_idEntryName;
extern jfieldID g_idEntryStatus;

/* The Info class */

extern jfieldID g_idInfoTotalSizeInBytes;
extern jfieldID g_idInfoAllocatedSizeInBytes;
extern jfieldID g_idInfoNumObjects;
extern jfieldID g_idInfoNumGroups;
extern jfieldID g_idInfoNumMallocs;
extern jfieldID g_idInfoNumFrees;
extern jfieldID g_idInfoLargestFreeInBytes;
extern jfieldID g_idInfoMemoryVersion;
extern jfieldID g_idInfoBigEndian;
extern jfieldID g_idInfoCompiler;
extern jfieldID g_idInfoCompilerVersion;
extern jfieldID g_idInfoPlatform;
extern jfieldID g_idInfoDllVersion;
extern jfieldID g_idInfoQuasarVersion;
extern jfieldID g_idInfoCreationTime;

/* The KeyDefinition class */
extern jfieldID g_idKeyDefHandle;
extern jfieldID g_idKeyDefKeyDef;
extern jfieldID g_idKeyDefType;
extern jfieldID g_idKeyDefName;

/* The ObjectDefinition class */

extern jfieldID g_idObjDefType;
extern jfieldID g_idObjDefFlags;
extern jfieldID g_idObjDefMinNumOfDataRecords;
extern jfieldID g_idObjDefMinNumBlocks;

/* The ObjectStatus class */

extern jfieldID g_idStatusType;
extern jfieldID g_idStatusStatus;
extern jfieldID g_idStatusNumBlocks;
extern jfieldID g_idStatusNumBlockGroup;
extern jfieldID g_idStatusNumDataItem;
extern jfieldID g_idStatusFreeBytes;
extern jfieldID g_idStatusMaxDataLength;
extern jfieldID g_idStatusMaxKeyLength;

/* The Session class */

extern jfieldID g_idSessionHandle;

////////////////////// beyond this point - not vetted

/* The FieldDefinition class */

//extern jfieldID g_idFieldDefName;
//extern jfieldID g_idFieldDefType;
//extern jfieldID g_idFieldDefLength;
//extern jfieldID g_idFieldDefMinLength;
//extern jfieldID g_idFieldDefMaxLength;
//extern jfieldID g_idFieldDefPrecision;
//extern jfieldID g_idFieldDefScale;

//extern jweak g_FieldDefClass;

/* The KeyType class (enum) */

//extern jfieldID g_idKeyTypeType;

//extern jweak * g_weakKeyType;

/* The folder class */

extern jfieldID g_idFolderHandle;

/* The Queue class */

extern jfieldID g_idQueueHandle;



/* The Definition class */

//extern jfieldID g_idDefinitionDef;
//extern jfieldID g_idDefinitionKey;
//extern jfieldID g_idDefinitionFields;

/* The DataRecord class */
extern jfieldID g_idRecordFields;
extern jfieldID g_idRecordObj;
extern jfieldID g_idRecordconversionType;
extern jfieldID g_idRecordOffset;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline jstring getNotNullTerminatedString( JNIEnv * env, 
                                                  char   * inStr,
                                                  size_t   maxLength )
{
   char * s;
   jstring jstr;
   jclass exc;
   
   if ( inStr[maxLength-1] != 0 ) {
      /* Special case - not null terminated string */
      s = malloc( maxLength+1 );
      if ( s == NULL ) {
         exc = (*env)->FindClass(env, "java/lang/OutOfMemoryError");
         if ( exc != NULL ) {
            (*env)->ThrowNew( env, exc, "malloc failed in jni code");
         }
         return NULL;
      }
      memcpy( s, inStr, maxLength );
      s[maxLength] = 0;
      jstr = (*env)->NewStringUTF( env, s );
      free(s);
   }
   else {
      jstr = (*env)->NewStringUTF( env, inStr );
   }

   return jstr;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* JNI_PHOTON_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

