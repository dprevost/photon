/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDS_FAST_MAP_H
#define VDS_FAST_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <vdsf/vdsCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 * This file provides the API needed to access read-only VDSF hash maps.
 *
 * The features are very similar to the ordinary hash maps except that
 * no locks are require to access the data and special procedures are 
 * implemented for the occasional updates:
 *
 *    1) when a map is open in read-only mode (vdsFastMapOpen(), the 
 *       end-of-this-unit-of-work calls (vdsCommit/vdsRollback) will check 
 *       if a new version of the map exits and if indeed this is the case, 
 *       the new version will be use instead of the old one.
 *
 *    2) when a map is open for editing a working copy of the map is created
 *       in shared memory and the map can be updated (no locks again since
 *       only the updater can access the working copy). When the session is
 *       committed, the working version becomes the latest version and can
 *       be open/accessed by readers. And, of course, the same procedure 
 *       applies if you have a set of maps that must be changed together.
 *
 *       If vdsRollback is called, all changes done to the working copy 
 *       are erased.
 *
 * Note: the old versions are removed from memory when all readers have
 *       updated their versions. Even if a program is only doing read access
 *       to the VDS data, it is important to add vdsCommit() once in a while
 *       to refresh the "handles" if the program is running for a while. 
 */

/**
 * \defgroup vdsFastMap_c API functions for vdsf read-only hash maps.
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
 * data items that were inserted or removed (if the map was open with 
 * ::vdsFastMapEdit). You still must use either 
 * ::vdsCommit or ::vdsRollback to end the current unit of work.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::vdsFastMapOpen 
 *                          or ::vdsFastMapEdit).
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsFastMapClose( VDS_HANDLE objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Retrieve the data definition of the hash map.
 *
 * \warning This function allocates a buffer to hold the definition (using 
 * malloc()). You must free it (with free()) when you no longer need the
 * definition.
 *
 * \param[in]   objectHandle The handle to the hash map (see ::vdsFastMapOpen 
 *                           or ::vdsFastMapEdit).
 * \param[out]  definition The buffer allocated by the API to hold the content 
 *              of the object definition. Freeing the memory (with free())
 *              is the responsability of the caller.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsFastMapDefinition( VDS_HANDLE             objectHandle, 
                          vdsObjectDefinition ** definition );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Remove the data item identified by the given key from the hash map 
 * (you must be in edit mode).
 *
 * The removals only become permanent after a call to ::vdsCommit.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::vdsFastMapEdit).
 * \param[in]  key The key of the item to be removed.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsFastMapDelete( VDS_HANDLE   objectHandle,
                      const void * key,
                      size_t       keyLength );

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
 *             the vdsf engine).
 * \param[out] objectHandle The handle to the hash map, allowing us access to
 *             the map in shared memory. On error, this handle will be set
 *             to zero (NULL) unless the objectHandle pointer itself is NULL.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsFastMapEdit( VDS_HANDLE   sessionHandle,
                    const char * hashMapName,
                    size_t       nameLengthInBytes,
                    VDS_HANDLE * objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Retrieve the data item identified by the given key from the hash map.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::vdsFastMapOpen
 *                          or ::vdsFastMapEdit).
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
int vdsFastMapGet( VDS_HANDLE   objectHandle,
                   const void * key,
                   size_t       keyLength,
                   void       * buffer,
                   size_t       bufferLength,
                   size_t     * returnedLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Iterate through the hash map.
 *
 * Data items retrieved this way will not be sorted.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::vdsFastMapOpen
 *                          or ::vdsFastMapEdit).
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
int vdsFastMapGetFirst( VDS_HANDLE   objectHandle,
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
 * Evidently, you must call ::vdsFastMapGetFirst to initialize the iterator. 
 * Not so evident - calling ::vdsFastMapGet will reset the iteration to the
 * data item retrieved by this function (they use the same internal storage). 
 * If this cause a problem, please let us know.
 *
 * Data items retrieved this way will not be sorted.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::vdsFastMapOpen
 *                          or ::vdsFastMapEdit).
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
int vdsFastMapGetNext( VDS_HANDLE   objectHandle,
                       void       * key,
                       size_t       keyLength,
                       void       * buffer,
                       size_t       bufferLength,
                       size_t     * retKeyLength,
                       size_t     * retDataLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Insert a data element in the hash map (you must be in edit mode).
 *
 * The additions only become permanent after a call to ::vdsCommit.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::vdsFastMapEdit).
 * \param[in]  key The key of the item to be inserted.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 * \param[in]  data  The data item to be inserted.
 * \param[in]  dataLength The length of \em data (in bytes).
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsFastMapInsert( VDS_HANDLE   objectHandle,
                      const void * key,
                      size_t       keyLength,
                      const void * data,
                      size_t       dataLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Open an existing hash map read only (see ::vdsCreateObject to create a 
 * new object).
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
int vdsFastMapOpen( VDS_HANDLE   sessionHandle,
                    const char * hashMapName,
                    size_t       nameLengthInBytes,
                    VDS_HANDLE * objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Replace a data element in the hash map (you must be in edit mode).
 *
 * The replacements only become permanent after a call to ::vdsCommit.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::vdsFastMapEdit).
 * \param[in]  key The key of the item to be replaced.
 * \param[in]  keyLength The length of the \em key buffer (in bytes).
 * \param[in]  data  The new data item that will replace the previous data.
 * \param[in]  dataLength The length of \em data (in bytes).
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsFastMapReplace( VDS_HANDLE   objectHandle,
                       const void * key,
                       size_t       keyLength,
                       const void * data,
                       size_t       dataLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Return the status of the hash map.
 *
 * \param[in]  objectHandle The handle to the hash map (see ::vdsFastMapOpen or
 *                          ::vdsFastMapEdit).
 * \param[out] pStatus      A pointer to the status structure.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsFastMapStatus( VDS_HANDLE     objectHandle,
                      vdsObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*@}*/

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDS_FAST_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

