/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef PSOA_MAP_H
#define PSOA_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/CommonObject.h"
#include "Nucleus/Map.h"
#include "API/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct psoaMap
{
   psoaCommonObject object;
   
   /**
    * This struct holds both the result of the iteration (the pointer to
    * the current psonHashItem) and the values needed to get the next item.
    */
   psonHashMapItem iterator;

   /** A pointer to the data definition of the queue. */
   psonFieldDef * pDefinition;
   
   size_t minLength;
   size_t maxLength;
   size_t minKeyLength;
   size_t maxKeyLength;

   int editMode;

   psoaReader reader;
   
} psoaMap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_API_EXPORT
int psoaMapFirst( psoaMap          * pHashMap,
                 psoaHashMapEntry * pEntry );

PHOTON_API_EXPORT
int psoaMapNext( psoaMap          * pHashMap,
                psoaHashMapEntry * pEntry );

PHOTON_API_EXPORT
int psoaMapRetrieve( psoaMap       * pHashMap,
                    const void   * key,
                    size_t         keyLength,
                    psoaDataEntry * pEntry );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOA_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

