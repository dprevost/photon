/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
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

#ifndef VDSA_HASH_MAP_H
#define VDSA_HASH_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/CommonObject.h"
#include "Engine/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdsaHashMap
{
   vdsaCommonObject object;
   
   /**
    * This struct holds both the result of the iteration (the pointer to
    * the current vdseHashItem) and the values needed to get the next item.
    */
   vdseHashMapItem iterator;

   /** A pointer to the data definition of the queue. */
   vdseFieldDef * pDefinition;
   
   size_t minLength;
   size_t maxLength;
   size_t minKeyLength;
   size_t maxKeyLength;

} vdsaHashMap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdsaHashMapEntry
{
   size_t keyLength;
   size_t dataLength;
   const void * key;
   const void * data;

} vdsaHashMapEntry;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_API_EXPORT
int vdsaHashMapFirst( vdsaHashMap      * pHashMap,
                      vdsaHashMapEntry * pEntry );

VDSF_API_EXPORT
int vdsaHashMapNext( vdsaHashMap      * pHashMap,
                     vdsaHashMapEntry * pEntry );

VDSF_API_EXPORT
int vdsaHashMapRetrieve( vdsaHashMap   * pHashMap,
                         const void    * key,
                         size_t          keyLength,
                         vdsaDataEntry * pEntry );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSA_HASH_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

