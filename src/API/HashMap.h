/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSOA_HASH_MAP_H
#define PSOA_HASH_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/CommonObject.h"
#include "Nucleus/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct psoaHashMap
{
   psoaCommonObject object;
   
   /**
    * This struct holds both the result of the iteration (the pointer to
    * the current psonHashItem) and the values needed to get the next item.
    */
   psonHashMapItem iterator;

   /** A pointer to the key definition of the object. */
   unsigned char * keyDef;

   /** The length of the key definition of the object. */
   uint32_t keyLength;
   
   uint32_t minLength;
   uint32_t maxLength;
   uint32_t minKeyLength;
   uint32_t maxKeyLength;

} psoaHashMap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct psoaHashMapEntry
{
   uint32_t keyLength;
   uint32_t dataLength;
   const void * key;
   const void * data;

} psoaHashMapEntry;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_API_EXPORT
int psoaHashMapFirst( psoaHashMap      * pHashMap,
                     psoaHashMapEntry * pEntry );

PHOTON_API_EXPORT
int psoaHashMapNext( psoaHashMap      * pHashMap,
                    psoaHashMapEntry * pEntry );

PHOTON_API_EXPORT
int psoaHashMapRetrieve( psoaHashMap   * pHashMap,
                        const void   * key,
                        uint32_t       keyLength,
                        psoaDataEntry * pEntry );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOA_HASH_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

