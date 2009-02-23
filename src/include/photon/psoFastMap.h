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

#ifndef PSO_FAST_MAP_H
#define PSO_FAST_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <photon/psoCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 * This file provides the API needed to access read-only Photon hash maps.
 *
 * The features of fast maps are very similar to the features of standard 
 * hash maps with one major exception - no locks are required to access 
 * the data and special procedures are implemented for the occasional 
 * updates:.
 *
 * <ol>
 *   <li>
 *     when a map is open in read-only mode (::psoFastMapOpen), the 
 *     end-of-this-unit-of-work calls (::psoCommit, ::psoRollback) will 
 *     check if a new version of the map exist and if indeed this is 
 *     the case, the new version will be use instead of the old one.
 *   </li>
 *
 *   <li>
 *     when a map is open for editing (::psoFastMapEdit) a working copy 
 *     of the map is created in shared memory and the map can be updated
 *     (no locks again since only the updater can access the working copy). 
 *     When the session is committed, the working version becomes the 
 *     latest version and can be open/accessed by readers. And, of course, 
 *     the same procedure applies if you have a set of maps that must be 
 *     changed together.
 *
 *     If ::psoRollback is called, all changes done to the working copy 
 *     are erased.
 *   </li>
 * </ol>
 * 
 * Note: the old versions are removed from memory when all readers have 
 * updated their versions. Even if a program is only doing read access to 
 * shared-memory data, it is important to add ::psoCommit once in a while 
 * to refresh the "handles" periodically.
 */

/**
 * \defgroup psoFastMap_c API functions for Photon read-only hash maps.
 *
 * The features of fast maps are very similar to the features of standard 
 * hash maps with one major exception - no locks are required to access 
 * the data and special procedures are implemented for the occasional 
 * updates:.
 *
 * <ol>
 *   <li>
 *     when a map is open in read-only mode (::psoFastMapOpen), the 
 *     end-of-this-unit-of-work calls (::psoCommit, ::psoRollback) will 
 *     check if a new version of the map exist and if indeed this is 
 *     the case, the new version will be use instead of the old one.
 *   </li>
 *
 *   <li>
 *     when a map is open for editing (::psoFastMapEdit) a working copy 
 *     of the map is created in shared memory and the map can be updated
 *     (no locks again since only the updater can access the working copy). 
 *     When the session is committed, the working version becomes the 
 *     latest version and can be open/accessed by readers. And, of course, 
 *     the same procedure applies if you have a set of maps that must be 
 *     changed together.
 *
 *     If ::psoRollback is called, all changes done to the working copy 
 *     are erased.
 *   </li>
 * </ol>
 * 
 * Note: the old versions are removed from memory when all readers have 
 * updated their versions. Even if a program is only doing read access to 
 * shared-memory data, it is important to add ::psoCommit once in a while 
 * to refresh the "handles" periodically.
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
 * data items that were inserted or removed (if the map was open with 
 * ::psoFastMapEdit). You still must use either 
 * ::psoCommit or ::psoRollback to end the current unit of work.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoFastMapOpen 
 *                          or ::psoFastMapEdit).
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFastMapClose( PSO_HANDLE objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Retrieve the data definition of the hash map.
 *
 * You can call the function ::psoFastMapDefLength to retrieve the 
 * length of the field definitions. This allows you to allocate 
 * the proper size for \em fields.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoFastMapOpen 
 *                          or ::psoFastMapEdit).
 * \param[out] definition The definition of the object.
 * \param[out] key An opaque definition of the key. You can set this field
 *             to NULL if you do not want to retrieve the key definition.
 * \param[in]  keyLength The length in bytes of the buffer \em key. 
 *             It should be set to zero if \em key is NULL.
 * \param[out] fields An opaque definition of the data fields of the object.
 *             It can be set to NULL if you do not want to retrieve the
 *             definition.
 * \param[in]  fieldsLength The length in bytes of the buffer \em fields.
 *             It should be set to zero if \em fields is NULL.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFastMapDefinition( PSO_HANDLE            objectHandle, 
                          psoObjectDefinition * definition,
                          unsigned char       * key,
                          psoUint32             keyLength,
                          unsigned char       * fields,
                          psoUint32             fieldsLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Retrieve the lengths of the variable elements of the definition 
 *        of the hash map.
 *
 * This function is a helper function for ::psoFastMapDefinition. 
 * It allows you to allocate the proper size for \em key and \em fields.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoFastMapOpen 
 *                          or ::psoFastMapEdit).
 * \param[out] keyLength The length in bytes of the key buffer.
 * \param[out] fieldsLength The length in bytes of the fields buffer.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFastMapDefLength( PSO_HANDLE   objectHandle, 
                         psoUint32  * keyLength,
                         psoUint32  * fieldsLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Remove the data item identified by the given key from the hash map 
 * (you must be in edit mode).
 *
 * The removals only become permanent after a call to ::psoCommit.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoFastMapEdit).
 * \param[in]  key The key of the item to be removed.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFastMapDelete( PSO_HANDLE   objectHandle,
                      const void * key,
                      psoUint32    keyLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Open a temporary copy of an existing hash map for editing.
 *
 * The copy becomes the latest version of the map when a session is committed.
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
int psoFastMapEdit( PSO_HANDLE   sessionHandle,
                    const char * hashMapName,
                    psoUint32    nameLengthInBytes,
                    PSO_HANDLE * objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Remove all data items from the map (you must be in edit mode).
 *
 * The removals only become permanent after a call to ::psoCommit.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoFastMapEdit).
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFastMapEmpty( PSO_HANDLE objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Retrieve the data item identified by the given key from the hash map.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoFastMapOpen
 *                          or ::psoFastMapEdit).
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
int psoFastMapGet( PSO_HANDLE   objectHandle,
                   const void * key,
                   psoUint32    keyLength,
                   void       * buffer,
                   psoUint32    bufferLength,
                   psoUint32  * returnedLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Iterate through the hash map.
 *
 * Data items retrieved this way will not be sorted.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoFastMapOpen
 *                          or ::psoFastMapEdit).
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
int psoFastMapGetFirst( PSO_HANDLE   objectHandle,
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
 * Evidently, you must call ::psoFastMapGetFirst to initialize the iterator. 
 * Not so evident - calling ::psoFastMapGet will reset the iteration to the
 * data item retrieved by this function (they use the same internal storage). 
 * If this cause a problem, please let us know.
 *
 * Data items retrieved this way will not be sorted.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoFastMapOpen
 *                          or ::psoFastMapEdit).
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
int psoFastMapGetNext( PSO_HANDLE   objectHandle,
                       void       * key,
                       psoUint32    keyLength,
                       void       * buffer,
                       psoUint32    bufferLength,
                       psoUint32  * retKeyLength,
                       psoUint32  * retDataLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Insert a data element in the hash map (you must be in edit mode).
 *
 * The additions only become permanent after a call to ::psoCommit.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoFastMapEdit).
 * \param[in]  key The key of the item to be inserted.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 * \param[in]  data  The data item to be inserted.
 * \param[in]  dataLength The length of \em data (in bytes).
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFastMapInsert( PSO_HANDLE   objectHandle,
                      const void * key,
                      psoUint32    keyLength,
                      const void * data,
                      psoUint32    dataLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Open an existing hash map read only (see ::psoCreateObject to create a 
 * new object).
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
int psoFastMapOpen( PSO_HANDLE   sessionHandle,
                    const char * hashMapName,
                    psoUint32    nameLengthInBytes,
                    PSO_HANDLE * objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Replace a data element in the hash map (you must be in edit mode).
 *
 * The replacements only become permanent after a call to ::psoCommit.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoFastMapEdit).
 * \param[in]  key The key of the item to be replaced.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 * \param[in]  data  The new data item that will replace the previous data.
 * \param[in]  dataLength The length of \em data (in bytes).
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFastMapReplace( PSO_HANDLE   objectHandle,
                       const void * key,
                       psoUint32    keyLength,
                       const void * data,
                       psoUint32    dataLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Return the status of the hash map.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::psoFastMapOpen or
 *                          ::psoFastMapEdit).
 * \param[out] pStatus      A pointer to the status structure.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFastMapStatus( PSO_HANDLE     objectHandle,
                      psoObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*@}*/

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_FAST_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

