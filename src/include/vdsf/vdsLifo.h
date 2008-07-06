/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDS_LIFO_H
#define VDS_LIFO_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <vdsf/vdsCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 * This file provides the API needed to access a VDSF LIFO queue.
 */
/**
 * \defgroup vdsLifo_c API functions for vdsf LIFO queues.
 *
 * A reminder: LIFO, Last In First Out. Data items are placed at the end of
 * the queue and retrieved from the end of the queue.
 */
/*@{*/

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Close a LIFO queue.
 *
 * This function terminates the current access to the queue in shared memory
 * (the queue itself is untouched).
 *
 * \warning Closing an object does not automatically commit or rollback 
 * data items that were inserted or removed. You still must use either 
 * ::vdsCommit or ::vdsRollback to end the current unit of work.
 *
 * \param[in] objectHandle The handle to the queue (see ::vdsLifoOpen).
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsLifoClose( VDS_HANDLE objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Retrieve the data definition of the queue.
 *
 * \warning This function allocates a buffer to hold the definition (using 
 * malloc()). You must free it (with free()) when you no longer need the
 * definition.
 *
 * \param[in]   objectHandle The handle to the queue (see ::vdsLifoOpen).
 * \param[out]  definition The buffer allocated by the API to hold the content 
 *              of the object definition. Freeing the memory (with free())
 *              is the responsability of the caller.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsLifoDefinition( VDS_HANDLE             objectHandle, 
                       vdsObjectDefinition ** definition );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * \brief Iterate through the queue - no data items are removed from the queue
 * by this function.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by the iterator. Likewise, destroyed data items (even if
 * not yet committed) are invisible.
 *
 * \param[in]   objectHandle The handle to the queue (see ::vdsLifoOpen).
 * \param[out]  buffer The buffer provided by the user to hold the content of
 *              the first element. Memory allocation for this buffer is the
 *              responsability of the caller.
 * \param[in]   bufferLength The length of \em buffer (in bytes).
 * \param[out]  returnedLength The actual number of bytes in the data item.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsLifoGetFirst( VDS_HANDLE   objectHandle,
                     void       * buffer,
                     size_t       bufferLength,
                     size_t     * returnedLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Iterate through the queue - no data items are removed from the queue
 * by this function.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by the iterator. Likewise, destroyed data items (even if
 * not yet committed) are invisible.
 *
 * Evidently, you must call ::vdsLifoGetFirst to initialize the iterator. 
 * Not so evident - calling ::vdsLifoPop will reset the iteration to the
 * last element (they use the same internal storage). If this cause a problem,
 * please let us know.
 *
 * \param[in]   objectHandle The handle to the queue (see ::vdsLifoOpen).
 * \param[out]  buffer The buffer provided by the user to hold the content of
 *              the next element. Memory allocation for this buffer is the
 *              responsability of the caller.
 * \param[in]   bufferLength The length of \em buffer (in bytes).
 * \param[out]  returnedLength The actual number of bytes in the data item.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsLifoGetNext( VDS_HANDLE   objectHandle,
                    void       * buffer,
                    size_t       bufferLength,
                    size_t     * returnedLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Open an existing LIFO queue (see ::vdsCreateObject to create a new queue).
 *
 * \param[in]  sessionHandle The handle to the current session.
 * \param[in]  queueName The fully qualified name of the queue. 
 * \param[in]  nameLengthInBytes The length of \em queueName (in bytes) not
 *             counting the null terminator (null-terminators are not used by
 *             the vdsf engine).
 * \param[out] objectHandle The handle to the queue, allowing us access to
 *             the queue in shared memory. On error, this handle will be set
 *             to zero (NULL) unless the objectHandle pointer itself is NULL.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsLifoOpen(  VDS_HANDLE   sessionHandle,
                  const char * queueName,
                  size_t       nameLengthInBytes,
                  VDS_HANDLE * objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Remove the first item from the beginning of a LIFO queue and return it 
 * to the caller.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by this function. Likewise, destroyed data items (even if
 * not yet committed) are invisible.
 *
 * The removals only become permanent after a call to ::vdsCommit.
 *
 * \param[in]   objectHandle The handle to the queue (see ::vdsLifoOpen).
 * \param[out]  buffer The buffer provided by the user to hold the content of
 *              the data item. Memory allocation for this buffer is the
 *              responsability of the caller.
 * \param[in]   bufferLength The length of \em buffer (in bytes).
 * \param[out]  returnedLength The actual number of bytes in the data item.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsLifoPop( VDS_HANDLE   objectHandle,
                void       * buffer,
                size_t       bufferLength,
                size_t     * returnedLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Insert a data element at the end of the LIFO queue.
 *
 * The additions only become permanent after a call to ::vdsCommit.
 *
 * \param[in]  objectHandle The handle to the queue (see ::vdsLifoOpen).
 * \param[in]  pItem  The data item to be inserted.
 * \param[in]  length The length of \em pItem (in bytes).
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsLifoPush( VDS_HANDLE   objectHandle, 
                 const void * pItem, 
                 size_t       length );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Return the status of the queue.
 *
 * \param[in]  objectHandle The handle to the queue (see ::vdsLifoOpen).
 * \param[out] pStatus      A pointer to the status structure.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsLifoStatus( VDS_HANDLE     objectHandle,
                   vdsObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*@}*/

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDS_LIFO_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
