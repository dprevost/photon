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

#ifndef PSA_DATA_DEFINITION_H
#define PSA_DATA_DEFINITION_H

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
int psaGetDefinition( psnFieldDef          * pInternalDef,
                      uint16_t               numFields,
                      psoObjectDefinition ** ppDefinition );

PHOTON_API_EXPORT
void psaGetKeyLimits( psoKeyDefinition * pKeyDef,
                      size_t           * pMinLength,
                      size_t           * pMaxLength );

PHOTON_API_EXPORT
void psaGetLimits( psnFieldDef * pDefinition,
                   uint16_t      numFields,
                   size_t      * pMinLength,
                   size_t      * pMaxLength );

PHOTON_API_EXPORT
int psaValidateDefinition( psoObjectDefinition * pDefinition );

PHOTON_API_EXPORT
int psaXmlToDefinition( const char           * xmlBuffer,
                        size_t                 lengthInBytes,
                        psoObjectDefinition ** ppDefinition,
                        char                ** objectName,
                        size_t               * nameLengthInBytes );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSA_DATA_DEFINITION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
