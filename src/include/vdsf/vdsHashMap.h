/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
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

#ifndef VDS_HASH_MAP_H
#define VDS_HASH_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <vdsf/vdsCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 * This file provides the API needed to access a VDSF hash map.
 *
 * Hash maps use unique keys - the data items are not sorted.
 */

/**
 * \defgroup vdsHashMap_c API functions for vdsf hash maps.
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
 * ::vdsCommit or ::vdsRollback to end the current unit of work.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::vdsHashMapOpen).
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsHashMapClose( VDS_HANDLE objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Remove the data item identified by the given key from the hash map.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by this function and cannot be removed. Likewise, 
 * destroyed data items (even if not yet committed) are invisible.
 *
 * The removals only become permanent after a call to ::vdsCommit.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::vdsHashMapOpen).
 * \param[in]  key The key of the item to be removed.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsHashMapDelete( VDS_HANDLE   objectHandle,
                      const void * key,
                      size_t       keyLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Retrieve the data item identified by the given key from the hash map.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by this function. Likewise, 
 * destroyed data items (even if not yet committed) are invisible.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::vdsHashMapOpen).
 * \param[in]  key The key of the item to be retrieved.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 * \param[out] buffer The buffer provided by the user to hold the content of
 *             the data item. Memory allocation for this buffer is the
 *             responsability of the caller.
 * \param[in]  bufferLength The length of \em buffer (in bytes).
 * \param[out] returnedLength The actual number of bytes in the data item.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsHashMapGet( VDS_HANDLE   objectHandle,
                   const void * key,
                   size_t       keyLength,
                   void       * buffer,
                   size_t       bufferLength,
                   size_t     * returnedLength );

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
 * \param[in]  objectHandle The handle to the hash map (see ::vdsHashMapOpen).
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
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsHashMapGetFirst( VDS_HANDLE   objectHandle,
                        void       * key,
                        size_t       keyLength,
                        void       * buffer,
                        size_t       bufferLength,
                        size_t     * retKeyLength,
                        size_t     * retDataLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Iterate through the hash map.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by the iterator. Likewise, destroyed data items (even if
 * not yet committed) are invisible.
 *
 * Evidently, you must call ::vdsHashMapGetFirst to initialize the iterator. 
 * Not so evident - calling ::vdsHashMapGet will reset the iteration to the
 * data item retrieved by this function (they use the same internal storage). 
 * If this cause a problem, please let us know.
 *
 * Data items retrieved this way will not be sorted.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::vdsHashMapOpen).
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
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsHashMapGetNext( VDS_HANDLE   objectHandle,
                       void       * key,
                       size_t       keyLength,
                       void       * buffer,
                       size_t       bufferLength,
                       size_t     * retKeyLength,
                       size_t     * retDataLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Insert a data element in the hash map.
 *
 * The additions only become permanent after a call to ::vdsCommit.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::vdsHashMapOpen).
 * \param[in]  key The key of the item to be inserted.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 * \param[in]  data  The data item to be inserted.
 * \param[in]  dataLength The length of \em data (in bytes).
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsHashMapInsert( VDS_HANDLE   objectHandle,
                      const void * key,
                      size_t       keyLength,
                      const void * data,
                      size_t       dataLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Open an existing hash map (see ::vdsCreateObject to create a new object).
 *
 * \param[in]  sessionHandle The handle to the current session.
 * \param[in]  hashMapName The fully qualified name of the hash map. 
 * \param[in]  nameLengthInBytes The length of \em hashMapName (in bytes) not
 *             counting the null terminator (null-terminators are not used by
 *             the vdsf engine).
 * \param[out] objectHandle The handle to the hash map, allowing us access to
 *             the map in shared memory. On error, this handle will be set
 *             to zero (NULL) unless the objectHandle pointer itself is NULL.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsHashMapOpen( VDS_HANDLE   sessionHandle,
                    const char * hashMapName,
                    size_t       nameLengthInBytes,
                    VDS_HANDLE * objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Replace a data element in the hash map.
 *
 * The replacements only become permanent after a call to ::vdsCommit.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::vdsHashMapOpen).
 * \param[in]  key The key of the item to be replaced.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 * \param[in]  data  The new data item that will replace the previous data.
 * \param[in]  dataLength The length of \em data (in bytes).
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsHashMapReplace( VDS_HANDLE   objectHandle,
                       const void * key,
                       size_t       keyLength,
                       const void * data,
                       size_t       dataLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Return the status of the hash map.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::vdsHashMapOpen).
 * \param[out] pStatus      A pointer to the status structure.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsHashMapStatus( VDS_HANDLE     objectHandle,
                      vdsObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*@}*/

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDS_HASH_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

