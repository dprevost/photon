/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSO_QUEUE_H
#define PSO_QUEUE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <photon/psoCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 * This file provides the API needed to access a Photon FIFO queue.
 */
/**
 * \defgroup psoQueue_c API functions for Photon FIFO queues.
 *
 * A reminder: FIFO, First In First Out. Data items are placed at the end of
 * the queue and retrieved from the beginning of the queue.
 */
/*@{*/

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Close a FIFO queue.
 *
 * This function terminates the current access to the queue in shared memory
 * (the queue itself is untouched).
 *
 * \warning Closing an object does not automatically commit or rollback 
 * data items that were inserted or removed. You still must use either 
 * ::psoCommit or ::psoRollback to end the current unit of work.
 *
 * \param[in] objectHandle The handle to the queue (see ::psoQueueOpen).
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoQueueClose( PSO_HANDLE objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Retrieve the data definition of the queue.
 *
 * To avoid memory leaks, you must close the handle that will be 
 * returned by this function (see ::psoDataDefClose).
 *
 * \param[in]  objectHandle The handle to the queue (see ::psoQueueOpen).
 * \param[out] dataDefHandle Handle to the definition of the data fields.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoQueueDefinition( PSO_HANDLE   objectHandle, 
                        PSO_HANDLE * dataDefHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * \brief Initiates an iteratation through the queue - no data items are 
 * removed from the queue by this function.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by the iterator. Likewise, destroyed data items (even if
 * not yet committed) are invisible.
 *
 * \param[in]   objectHandle The handle to the queue (see ::psoQueueOpen).
 * \param[out]  buffer The buffer provided by the user to hold the content of
 *              the first element. Memory allocation for this buffer is the
 *              responsability of the caller.
 * \param[in]   bufferLength The length of \em buffer (in bytes).
 * \param[out]  returnedLength The actual number of bytes copied in the 
 *              data buffer.
 *
 * \return 0 on success, PSO_IS_EMPTY if the queue is empty or a ::psoErrors 
 *         on error.
 */
PHOTON_EXPORT
int psoQueueGetFirst( PSO_HANDLE   objectHandle,
                      void       * buffer,
                      psoUint32    bufferLength,
                      psoUint32  * returnedLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Iterate through the queue - no data items are removed from the queue
 * by this function.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by the iterator. Likewise, destroyed data items (even if
 * not yet committed) are invisible.
 *
 * Evidently, you must call ::psoQueueGetFirst to initialize the iterator. 
 * Not so evident - calling ::psoQueuePop will reset the iteration to the
 * last element (they use the same internal storage). If this cause a problem,
 * please let us know.
 *
 * \param[in]   objectHandle The handle to the queue (see ::psoQueueOpen).
 * \param[out]  buffer The buffer provided by the user to hold the content of
 *              the next element. Memory allocation for this buffer is the
 *              responsability of the caller.
 * \param[in]   bufferLength The length of \em buffer (in bytes).
 * \param[out]  returnedLength The actual number of bytes copied in the 
 *              data buffer.
 *
 * \return 0 on success, PSO_REACHED_THE_END when the iteration reaches
 *           the end of the queue or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoQueueGetNext( PSO_HANDLE   objectHandle,
                     void       * buffer,
                     psoUint32    bufferLength,
                     psoUint32  * returnedLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Open an existing FIFO queue (see ::psoCreateQueue to create a new queue).
 *
 * \param[in]  sessionHandle The handle to the current session.
 * \param[in]  queueName The fully qualified name of the queue. 
 * \param[in]  nameLengthInBytes The length of \em queueName (in bytes) not
 *             counting the null terminator.
 * \param[out] objectHandle The handle to the queue, allowing us access to
 *             the queue in shared memory. On error, this handle will be set
 *             to zero (NULL) unless the objectHandle pointer itself is NULL.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoQueueOpen(  PSO_HANDLE   sessionHandle,
                   const char * queueName,
                   psoUint32    nameLengthInBytes,
                   PSO_HANDLE * objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Remove the first item from the beginning of a FIFO queue and return it 
 * to the caller.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by this function. Likewise, destroyed data items (even if
 * not yet committed) are invisible.
 *
 * The removals only become permanent after a call to ::psoCommit.
 *
 * \param[in]   objectHandle The handle to the queue (see ::psoQueueOpen).
 * \param[out]  buffer The buffer provided by the user to hold the content of
 *              the data item. Memory allocation for this buffer is the
 *              responsability of the caller.
 * \param[in]   bufferLength The length of \em buffer (in bytes).
 * \param[out]  returnedLength The actual number of bytes copied in the 
 *              data buffer.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoQueuePop( PSO_HANDLE   objectHandle,
                 void       * buffer,
                 psoUint32    bufferLength,
                 psoUint32  * returnedLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Insert a data element at the end of the FIFO queue.
 *
 * The additions only become permanent after a call to ::psoCommit.
 *
 * The \em dataDefHandle argument should be used (non-NULL) only if
 * you use this queue to store data records having different data 
 * definitions.
 *
 * This could be used to implement inheritance of the data records or
 * to build a mismatched collection of records.
 *
 * \param[in]  objectHandle The handle to the queue (see ::psoQueueOpen).
 * \param[in]  pItem  The data item to be inserted.
 * \param[in]  length The length of \em pItem (in bytes).
 * \param[in]  dataDefHandle An optional handle to a data definition
 *             for this specific data record. The queue must have been created 
 *             with the appropriate flag to support this feature.
 *             Set this handle to NULL to use the default data definition.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoQueuePush( PSO_HANDLE   objectHandle, 
                  const void * pItem, 
                  psoUint32    length,
                  PSO_HANDLE   dataDefHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Insert a data element at the end of the FIFO queue.
 *
 * The additions become permanent immediately, not after a call to ::psoCommit.
 * (in other words, you cannot easily undo these insertions). 
 *
 * The \em dataDefHandle argument should be used (non-NULL) only if
 * you use this queue to store data records having different data 
 * definitions.
 *
 * This could be used to implement inheritance of the data records or
 * to build a mismatched collection of records.
 *
 * \param[in]  objectHandle The handle to the queue (see ::psoQueueOpen).
 * \param[in]  pItem  The data item to be inserted.
 * \param[in]  length The length of \em pItem (in bytes).
 * \param[in]  dataDefHandle A handle to the data definition for this 
 *             specific data record. The queue must have been created 
 *             with the appropriate flag to support this feature.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoQueuePushNow( PSO_HANDLE   objectHandle, 
                     const void * pItem, 
                     psoUint32    length,
                     PSO_HANDLE   dataDefHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Retrieves a handle allowing you to access the data definition of the 
 * last accessed record.
 *
 * Explanation:
 * 
 * Queues will usually contain data records with an identical layout (data 
 * definition of the items). This layout was defined when the queue was 
 * created. 
 *
 * You can also insert and retrieve data records with different layouts if
 * the object was created with the flag PSO_MULTIPLE_DATA_DEFINITIONS. The
 * layout defined when a queue is created is then used as the default one.
 * 
 * To access the layout on a record-by-record base, use the argument 
 * \em dataDefHandle - it will be set to the layout of the last retrieved
 * record.
 *
 * Note: you only need to get the handle once. The hidden fields associated
 * with this handle will be updated after each record is retrieved. The
 * handle will point to the data definition of the queue map upon
 * initialization.
 *
 * You must close the handle with ::psoDataDefClose when you no longer
 * need it.
 *
 * \param[in]  objectHandle The handle to the queue
 * \param[out] dataDefHandle This optional handle gives you access to the
 *             data definition of the record on a record by record basis.
 *             This handle must be closed with ::psoDataDefClose.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoQueueRecordDefinition( PSO_HANDLE   objectHandle,
                              PSO_HANDLE * dataDefHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Return the status of the queue.
 *
 * \param[in]  objectHandle The handle to the queue (see ::psoQueueOpen).
 * \param[out] pStatus      A pointer to the status structure.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoQueueStatus( PSO_HANDLE     objectHandle,
                    psoObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*@}*/

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_QUEUE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

