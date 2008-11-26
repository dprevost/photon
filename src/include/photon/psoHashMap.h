/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSO_HASH_MAP_H
#define PSO_HASH_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <photon/psoCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 * This file provides the API needed to access a Photon hash map.
 *
 * Hash maps use unique keys - the data items are not sorted.
 */

/**
 * \defgroup psoHashMap_c API functions for Photon hash maps.
 *
 * Hash maps use unique keys - the data items are not sorted.
 */
/*@{*/

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Close a Hash Map.
 *
 * This function terminates the current access to the hash map in shared 
 * memory (the hash map itself is untouched).
 *
 * \warning Closing an object does not automatically commit or rollback 
 * data items that were inserted or removed. You still must use either 
 * ::psoCommit or ::psoRollback to end the current unit of work.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoHashMapOpen).
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoHashMapClose( PSO_HANDLE objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Retrieve the data definition of the hash map.
 *
 * \warning This function allocates a buffer to hold the definition (using 
 * malloc()). You must free it (with free()) when you no longer need the
 * definition.
 *
 * \param[in]   objectHandle The handle to the hash map (see ::psoHashMapOpen).
 * \param[out]  definition The buffer allocated by the API to hold the content 
 *              of the object definition. Freeing the memory (with free())
 *              is the responsability of the caller.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoHashMapDefinition( PSO_HANDLE             objectHandle, 
                          psoObjectDefinition ** definition );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Remove the data item identified by the given key from the hash map.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by this function and cannot be removed. Likewise, 
 * destroyed data items (even if not yet committed) are invisible.
 *
 * The removals only become permanent after a call to ::psoCommit.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoHashMapOpen).
 * \param[in]  key The key of the item to be removed.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoHashMapDelete( PSO_HANDLE   objectHandle,
                      const void * key,
                      psoUint32    keyLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Retrieve the data item identified by the given key from the hash map.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by this function. Likewise, 
 * destroyed data items (even if not yet committed) are invisible.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoHashMapOpen).
 * \param[in]  key The key of the item to be retrieved.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 * \param[out] buffer The buffer provided by the user to hold the content of
 *             the data item. Memory allocation for this buffer is the
 *             responsability of the caller.
 * \param[in]  bufferLength The length of \em buffer (in bytes).
 * \param[out] returnedLength The actual number of bytes in the data item.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoHashMapGet( PSO_HANDLE   objectHandle,
                   const void * key,
                   psoUint32    keyLength,
                   void       * buffer,
                   psoUint32    bufferLength,
                   psoUint32  * returnedLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Iterate through the hash map.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by the iterator. Likewise, destroyed data items (even if
 * not yet committed) are invisible.
 *
 * Data items retrieved this way will not be sorted.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoHashMapOpen).
 * \param[out] key The key buffer provided by the user to hold the content of
 *             the key associated with the first element. Memory allocation 
 *             for this buffer is the responsability of the caller.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 * \param[out] buffer The buffer provided by the user to hold the content of
 *             the first element. Memory allocation for this buffer is the
 *             responsability of the caller.
 * \param[in]  bufferLength The length of \em buffer (in bytes).
 * \param[out] retKeyLength The actual number of bytes in the key
 * \param[out] retDataLength The actual number of bytes in the data item.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoHashMapGetFirst( PSO_HANDLE   objectHandle,
                        void       * key,
                        psoUint32    keyLength,
                        void       * buffer,
                        psoUint32    bufferLength,
                        psoUint32  * retKeyLength,
                        psoUint32  * retDataLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Iterate through the hash map.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by the iterator. Likewise, destroyed data items (even if
 * not yet committed) are invisible.
 *
 * Evidently, you must call ::psoHashMapGetFirst to initialize the iterator. 
 * Not so evident - calling ::psoHashMapGet will reset the iteration to the
 * data item retrieved by this function (they use the same internal storage). 
 * If this cause a problem, please let us know.
 *
 * Data items retrieved this way will not be sorted.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoHashMapOpen).
 * \param[out] key The key buffer provided by the user to hold the content of
 *             the key associated with the data element. Memory allocation 
 *             for this buffer is the responsability of the caller.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 * \param[out] buffer The buffer provided by the user to hold the content of
 *             the data element. Memory allocation for this buffer is the
 *             responsability of the caller.
 * \param[in]  bufferLength The length of \em buffer (in bytes).
 * \param[out] retKeyLength The actual number of bytes in the key
 * \param[out] retDataLength The actual number of bytes in the data item.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoHashMapGetNext( PSO_HANDLE   objectHandle,
                       void       * key,
                       psoUint32    keyLength,
                       void       * buffer,
                       psoUint32    bufferLength,
                       psoUint32  * retKeyLength,
                       psoUint32  * retDataLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Insert a data element in the hash map.
 *
 * The additions only become permanent after a call to ::psoCommit.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoHashMapOpen).
 * \param[in]  key The key of the item to be inserted.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 * \param[in]  data  The data item to be inserted.
 * \param[in]  dataLength The length of \em data (in bytes).
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoHashMapInsert( PSO_HANDLE   objectHandle,
                      const void * key,
                      psoUint32    keyLength,
                      const void * data,
                      psoUint32    dataLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Open an existing hash map (see ::psoCreateObject to create a new object).
 *
 * \param[in]  sessionHandle The handle to the current session.
 * \param[in]  hashMapName The fully qualified name of the hash map. 
 * \param[in]  nameLengthInBytes The length of \em hashMapName (in bytes) not
 *             counting the null terminator (null-terminators are not used by
 *             the Photon engine).
 * \param[out] objectHandle The handle to the hash map, allowing us access to
 *             the map in shared memory. On error, this handle will be set
 *             to zero (NULL) unless the objectHandle pointer itself is NULL.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoHashMapOpen( PSO_HANDLE   sessionHandle,
                    const char * hashMapName,
                    psoUint32    nameLengthInBytes,
                    PSO_HANDLE * objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Replace a data element in the hash map.
 *
 * The replacements only become permanent after a call to ::psoCommit.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoHashMapOpen).
 * \param[in]  key The key of the item to be replaced.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 * \param[in]  data  The new data item that will replace the previous data.
 * \param[in]  dataLength The length of \em data (in bytes).
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoHashMapReplace( PSO_HANDLE   objectHandle,
                       const void * key,
                       psoUint32    keyLength,
                       const void * data,
                       psoUint32    dataLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Return the status of the hash map.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoHashMapOpen).
 * \param[out] pStatus      A pointer to the status structure.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoHashMapStatus( PSO_HANDLE     objectHandle,
                      psoObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*@}*/

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_HASH_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

