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

#ifndef PSO_DATA_DEFINITION_H
#define PSO_DATA_DEFINITION_H

#include <photon/psoCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 * This file provides the API needed to access a Photon data definition.
 */
/**
 * \defgroup psoDataDefinition_c API functions for Photon data definitions.
 *
 * Objects in Photon must be associated with a data definition. These data
 * definitions are useful for essentially two reasons:
 *
 *  They provide an easy way to examine the type of content of specific 
 *  objects (the list of fields, for example).
 * 
 *  Data definitions are also needed to identify the content to object
 *  oriented languages to allow for the proper serialization of objects
 *  as an array of bytes and the deserialization of the array of bytes
 *  back to an object.
 *
 *  This last point is important since Photon can manage different types
 *  of data: record-oriented structures (ideal for COBOL, C), RDB rows and 
 *  serialized objects (Java, C#, etc.).
 *
 */
/*@{*/

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Closes the data definition.
 *
 * This function terminates the current access to the data definition.  
 * The definition itself is left untouched in shared memory.
 * \param[in] definitionHandle The handle to the data definition.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoDataDefClose( PSO_HANDLE definitionHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Creates a new data definition.
 *
 * \param[in]  sessionHandle The handle to the current session.
 * \param[in]  definitionName The name of the data definition. 
 * \param[in]  nameLengthInBytes The length of \em definitionName (in bytes).
 * \param[in]  type The type of definition (ODBC, user defined, etc.)
 * \param[in]  dataDef The data definition (as an opaque type)
 * \param[in]  dataDefLength The length in bytes of the buffer \em dataDef. 
 * \param[out] definitionHandle The handle to the data definition.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoDataDefCreate( PSO_HANDLE               sessionHandle,
                      const char             * definitionName,
                      psoUint32                nameLengthInBytes,
                      enum psoDefinitionType   type,
                      const unsigned char    * dataDef,
                      psoUint32                dataDefLength,
                      PSO_HANDLE             * definitionHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Retrieves a data definition.
 * 
 * You can call the function ::psoDataDefGetLength to retrieve the 
 * length of the data definition. This allows you to allocate 
 * the proper size for the \em dataDef buffer.
 *
 * \param[in]  definitionHandle The handle to the data definition.
 * \param[out] type The type of definition (ODBC, user defined, etc.)
 * \param[out] dataDef The data definition (as an opaque type). You are
 *             responsible for allocating the memory needed for this buffer. 
 * \param[in]  dataDefLength The length in bytes of the buffer \em dataDef. 
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoDataDefGet( PSO_HANDLE               definitionHandle,
                   enum psoDefinitionType * type,
                   unsigned char          * dataDef,
                   psoUint32                dataDefLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Retrieves the length of the data definition.
 * 
 * This function is a helper function for ::psoDataDefGet. 
 * It allows you to allocate the proper size for the data definition.
 *
 * \param[in]  definitionHandle The handle to the data definition.
 * \param[in]  dataDefLength The length in bytes of the data definition. 
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoDataDefGetLength( PSO_HANDLE   definitionHandle,
                         psoUint32  * dataDefLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Open an existing data definition.
 *
 * \param[in]  sessionHandle The handle to the current session.
 * \param[in]  definitionName The name of the data definition. 
 * \param[in]  nameLengthInBytes The length of \em definitionName (in bytes).
 * \param[out] definitionHandle The handle to the data definition.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoDataDefOpen( PSO_HANDLE   sessionHandle,
                    const char * definitionName,
                    psoUint32    nameLengthInBytes,
                    PSO_HANDLE * definitionHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* PSO_DATA_DEFINITION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

