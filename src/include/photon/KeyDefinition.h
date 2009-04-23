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

#ifndef PSO_KEY_DEFINITION_H
#define PSO_KEY_DEFINITION_H

#include <photon/psoCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 * This file provides the API needed to access a Photon key definition.
 */
/**
 * \defgroup psoKeyFieldDefinition_c API functions for Photon key definitions.
 * 
 */
/*@{*/

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Closes the key definition.
 *
 * This function terminates the current access to the key definition.  
 * The definition itself is left untouched in shared memory.
 * \param[in] definitionHandle The handle to the key definition.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoKeyDefClose( PSO_HANDLE definitionHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Creates a new key definition.
 *
 * \param[in]  sessionHandle The handle to the current session.
 * \param[in]  definitionName The name of the key definition. 
 * \param[in]  nameLengthInBytes The length of \em definitionName (in bytes).
 * \param[in]  type The type of definition (ODBC, user defined, etc.)
 * \param[in]  keyDef The key definition (as an opaque type)
 * \param[in]  keyDefLength The length in bytes of the buffer \em keyDef. 
 * \param[out] definitionHandle The handle to the key definition.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoKeyDefCreate( PSO_HANDLE               sessionHandle,
                     const char             * definitionName,
                     psoUint32                nameLengthInBytes,
                     enum psoDefinitionType   type,
                     const unsigned char    * keyDef,
                     psoUint32                keyDefLength,
                     PSO_HANDLE             * definitionHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Retrieves a key definition.
 * 
 * You can call the function ::psoKeyDefGetLength to retrieve the 
 * length of the key definition. This allows you to allocate 
 * the proper size for the \em keyDef buffer.
 *
 * \param[in]  definitionHandle The handle to the key definition.
 * \param[out] type The type of definition (ODBC, user defined, etc.)
 * \param[out] keyDef The key definition (as an opaque type). You are
 *             responsible for allocating the memory needed for this buffer. 
 * \param[in]  keyDefLength The length in bytes of the buffer \em keyDef. 
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoKeyDefGet( PSO_HANDLE               definitionHandle,
                  enum psoDefinitionType * type,
                  unsigned char          * keyDef,
                  psoUint32                keyDefLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Retrieves the length of the key definition.
 * 
 * This function is a helper function for ::psoKeyDefGet. 
 * It allows you to allocate the proper size for the key definition.
 *
 * \param[in]  definitionHandle The handle to the key definition.
 * \param[in]  keyDefLength The length in bytes of the key definition. 
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoKeyDefGetLength( PSO_HANDLE   definitionHandle,
                        psoUint32  * keyDefLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Open an existing key definition.
 *
 * \param[in]  sessionHandle The handle to the current session.
 * \param[in]  definitionName The name of the key definition. 
 * \param[in]  nameLengthInBytes The length of \em definitionName (in bytes).
 * \param[out] definitionHandle The handle to the key definition.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoKeyDefOpen( PSO_HANDLE   sessionHandle,
                   const char * definitionName,
                   psoUint32    nameLengthInBytes,
                   PSO_HANDLE * definitionHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* PSO_KEY_DEFINITION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
