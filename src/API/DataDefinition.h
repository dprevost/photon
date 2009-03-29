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

#ifndef PSOA_DATA_DEFINITION_H
#define PSOA_DATA_DEFINITION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <photon/psoCommon.h>
#include "API/api.h"
#include <photon/psoPhotonODBC.h>
#include "Nucleus/Definitions.h"
#include "API/Session.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psoaDataDefinition
{
   /* Key or data definition */
   psoaDefinitionType definitionType;
   
   psoaSession * pSession;
   
   psonDataDefinition * pMemDefinition;
};

typedef struct psoaDataDefinition psoaDataDefinition;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * This function is not included in the published API.
 *
 * This function can be dangerous. Handles to data definition are 
 * not counted for performance reasons -> this might destroy a
 * definition which is used by someone else...
 */
PHOTON_API_EXPORT
int psoaDataDefDestroy( PSO_HANDLE   sessionHandle,
                        const char * definitionName,
                        psoUint32    nameLengthInBytes );

/*
 * Similar to psoaGetLimits (but using the api struct), it calculates the 
 * offsets for each data field.
 * The array of offsets must be allocated by the caller.
 */

PHOTON_API_EXPORT
void psoaGetFieldOffsets( psoFieldDefinition * pDefinition,
                          int                  numFields,
                          uint32_t           * pOffsets );

//PHOTON_API_EXPORT
//int psoaValidateDefinition( psoObjectDefinition * pDefinition,
//                            psoKeyDefinition    * pKey,
//                            psoFieldDefinition  * pFields );

#if 0
PHOTON_API_EXPORT
int psoaXmlToDefinition( const char           * xmlBuffer,
                         uint32_t               lengthInBytes,
                         psoObjectDefinition *  pDefinition,
                         psoKeyDefinition    *  pKey,
                         psoFieldDefinition  ** ppFields,
                         char                ** objectName,
                         uint32_t             * nameLengthInBytes );
#endif
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOA_DATA_DEFINITION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
