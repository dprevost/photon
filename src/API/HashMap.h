/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
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

#ifndef PSA_HASH_MAP_H
#define PSA_HASH_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/CommonObject.h"
#include "Nucleus/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct psaHashMap
{
   psaCommonObject object;
   
   /**
    * This struct holds both the result of the iteration (the pointer to
    * the current psnHashItem) and the values needed to get the next item.
    */
   psnHashMapItem iterator;

   /** A pointer to the data definition of the queue. */
   psnFieldDef * pDefinition;
   
   size_t minLength;
   size_t maxLength;
   size_t minKeyLength;
   size_t maxKeyLength;

} psaHashMap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct psaHashMapEntry
{
   size_t keyLength;
   size_t dataLength;
   const void * key;
   const void * data;

} psaHashMapEntry;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_API_EXPORT
int psaHashMapFirst( psaHashMap      * pHashMap,
                     psaHashMapEntry * pEntry );

PHOTON_API_EXPORT
int psaHashMapNext( psaHashMap      * pHashMap,
                    psaHashMapEntry * pEntry );

PHOTON_API_EXPORT
int psaHashMapRetrieve( psaHashMap   * pHashMap,
                        const void   * key,
                        size_t         keyLength,
                        psaDataEntry * pEntry );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSA_HASH_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

