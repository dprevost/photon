/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSOA_DATA_DEFINITION_H
#define PSOA_DATA_DEFINITION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <photon/psoCommon.h>
#include "Nucleus/DataType.h"
#include "API/api.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Extract the definition from the internal definition.
 *
 * Explanation: the internal representation is more compact to minimize
 * the footprint in shared memory.
 * 
 * The memory must be free by the calling program.
 */
PHOTON_API_EXPORT
int psoaGetDefinition( psonFieldDef          * pInternalDef,
                       uint16_t               numFields,
                       psoObjectDefinition ** ppDefinition );

PHOTON_API_EXPORT
void psoaGetKeyLimits( psoKeyDefinition * pKeyDef,
                       uint32_t         * pMinLength,
                       uint32_t         * pMaxLength );

PHOTON_API_EXPORT
void psoaGetLimits( psonFieldDef * pDefinition,
                    uint16_t       numFields,
                    uint32_t     * pMinLength,
                    uint32_t     * pMaxLength );

/*
 * Similar to psoaGetLimits (but using the api struct), it calculates the 
 * offsets for each data field.
 * The array of offsets must be allocated by the caller.
 */
PHOTON_API_EXPORT
void psoaGetOffsets( psoObjectDefinition * pDefinition,
                     uint32_t            * pOffsets );

PHOTON_API_EXPORT
int psoaValidateDefinition( psoObjectDefinition * pDefinition );

PHOTON_API_EXPORT
int psoaXmlToDefinition( const char           * xmlBuffer,
                         uint32_t               lengthInBytes,
                         psoObjectDefinition ** ppDefinition,
                         char                ** objectName,
                         uint32_t             * nameLengthInBytes );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOA_DATA_DEFINITION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
