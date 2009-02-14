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

/* The KeyDefinition class */

extern jfieldID g_idKeyDefType;
extern jfieldID g_idKeyDefLength;
extern jfieldID g_idKeyDefMinLength;
extern jfieldID g_idKeyDefMaxLength;
extern jclass   g_keyDefClass;

/* The FieldDefinition class */

extern jfieldID g_idFieldDefName;
extern jfieldID g_idFieldDefType;
extern jfieldID g_idFieldDefLength;
extern jfieldID g_idFieldDefMinLength;
extern jfieldID g_idFieldDefMaxLength;
extern jfieldID g_idFieldDefPrecision;
extern jfieldID g_idFieldDefScale;

/* The FieldType class (enum) */

extern jfieldID g_idFieldTypeType;

/* The ObjectDefinition class */

extern jfieldID g_idObjDefType;
extern jfieldID g_idObjDefNumFields;

/* The KeyType class (enum) */

extern jfieldID g_idKeyTypeType;

/* The ObjectType class (enum) */

extern jfieldID g_idObjTypeType;

/* The folder class */

extern jfieldID g_idFolderHandle;

/* The Queue class */

extern jfieldID g_idQueueHandle;

/* The Session class */

extern jfieldID g_idSessionHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* JNI_PHOTON_H */
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
