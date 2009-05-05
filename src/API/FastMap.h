/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSOA_FASTMAP_H
#define PSOA_FASTMAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/CommonObject.h"
#include "Nucleus/FastMap.h"
#include "API/HashMap.h"
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct psoaFastMap
{
   psoaCommonObject object;
   
   /**
    * This struct holds both the result of the iteration (the pointer to
    * the current psonHashItem) and the values needed to get the next item.
    */
   psonFastMapItem iterator;

   /*
    * Pointer to the data defintion struct used to hold the data
    * definition for each retrieved record
    */
   psoaDataDefinition * pRecordDefinition;

   int editMode;

   psoaReader reader;
   
} psoaFastMap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_API_EXPORT
int psoaFastMapFirst( psoaFastMap    * pHashMap,
                      unsigned char ** key,
                      uint32_t       * keyLength,
                      unsigned char ** data,
                      uint32_t       * dataLength );

PHOTON_API_EXPORT
int psoaFastMapNext( psoaFastMap    * pHashMap,
                     unsigned char ** key,
                     uint32_t       * keyLength,
                     unsigned char ** data,
                     uint32_t       * dataLength );

PHOTON_API_EXPORT
int psoaFastMapRetrieve( psoaFastMap    * pHashMap,
                         const void     * key,
                         uint32_t         keyLength,
                         unsigned char ** data,
                         uint32_t       * length );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOA_FASTMAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

