/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef PSA_MAP_H
#define PSA_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/CommonObject.h"
#include "Nucleus/Map.h"
#include "API/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct psaMap
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

   int editMode;

   psaReader reader;
   
} psaMap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_API_EXPORT
int psaMapFirst( psaMap          * pHashMap,
                 psaHashMapEntry * pEntry );

VDSF_API_EXPORT
int psaMapNext( psaMap          * pHashMap,
                psaHashMapEntry * pEntry );

VDSF_API_EXPORT
int psaMapRetrieve( psaMap       * pHashMap,
                    const void   * key,
                    size_t         keyLength,
                    psaDataEntry * pEntry );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSA_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

