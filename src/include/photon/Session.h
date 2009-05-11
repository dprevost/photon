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

#ifndef PSO_SESSION_H
#define PSO_SESSION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <photon/psoCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 * This file provides the API needed to create and use a session.
 *
 * Sessions exist mainly to support multi-threaded (MT) programs. 
 *
 * Best practices:
 * <ul>
 *   <li>
 *     Each thread of an MT program should have its own session, making it
 *     independent of the other threads.
 *   </li>
 *
 *   <li>
 *     If you need to open the same Photon object in multiple threads,
 *     simply open it multiple times using a different session for each
 *     thread. The overhead of having multiple handles to the same 
 *     shared-memory data container is minimal and using this technique
 *     instead of using multi-thread locks increases the overall 
 *      performance of the software.
 *   </li>
 *   <li>
 *     If you cannot use this, you must implement your own thread 
 *     synchronization mecanism.
 *   </li>
 *   <li>
 *     Signal handlers (and similar calls): you should not call ::psoExit or
 *     ::psoExitSession (or similar) from a signal handler. 
 *     Instead, some global flags should be set that will allow each session 
 *     to terminate in an orderly fashion.
 *   </li>
 * </ul>
 *
 * Additional note: API objects (or C handles) are just proxies for the real 
 * objects sitting in shared memory. Proper synchronization is already done 
 * in shared memory and it is best to avoid the additional synchronization 
 * of these proxy objects.
 */
/**
 * \defgroup psoSession_c API functions for Photon sessions.
 *
 * Sessions exist mainly to support multi-threaded (MT) programs. 
 *
 * Best practices:
 * <ul>
 *   <li>
 *     Each thread of an MT program should have its own session, making it
 *     independent of the other threads.
 *   </li>
 *
 *   <li>
 *     If you need to open the same Photon object in multiple threads,
 *     simply open it multiple times using a different session for each
 *     thread. The overhead of having multiple handles to the same 
 *     shared-memory data container is minimal and using this technique
 *     instead of using multi-thread locks increases the overall 
 *      performance of the software.
 *   </li>
 *   <li>
 *     If you cannot use this, you must implement your own thread 
 *     synchronization mecanism.
 *   </li>
 *   <li>
 *     Signal handlers (and similar calls): you should not call ::psoExit or
 *     ::psoExitSession (or similar) from a signal handler. 
 *     Instead, some global flags should be set that will allow each session 
 *     to terminate in an orderly fashion.
 *   </li>
 * </ul>
 *
 * Additional note: API objects (or C handles) are just proxies for the real 
 * objects sitting in shared memory. Proper synchronization is already done 
 * in shared memory and it is best to avoid the additional synchronization 
 * of these proxy objects.
 */
/*@{*/

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Commit all insertions and deletions (of the current session) executed 
 * since the previous call to psoCommit or psoRollback.
 *
 * Insertions and deletions subjected to this call include both data items
 * inserted, replaced and deleted from data containers (maps, etc.) and 
 * the objects themselves created, for example, with ::psoCreateQueue and/or 
 * destroyed with ::psoDestroyObject.
 *
 * Note: the internal calls executed by the engine to satisfy this request
 * cannot fail. As such,
 * you cannot find yourself with an ugly situation where some operations
 * were committed and others not. If an error is returned by this function,
 * nothing was committed.
 *
 * \param[in] sessionHandle Handle to the current session.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoCommit( PSO_HANDLE sessionHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Create a new folder in shared memory.
 *
 * The creation of the folder only becomes permanent after a call to 
 * ::psoCommit.
 *
 * This function does not provide a handle to the newly created folder. Use
 * ::psoFolderOpen to get the handle.
 *
 * \param[in] sessionHandle Handle to the current session.
 * \param[in] objectName The fully qualified name of the object. 
 * \param[in] nameLengthInBytes The length of \em objectName (in bytes) not
 *            counting the null terminator.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoCreateFolder( PSO_HANDLE   sessionHandle,
                     const char * objectName,
                     psoUint32    nameLengthInBytes );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Create a new queue object in shared memory.
 *
 * The creation of the object only becomes permanent after a call to 
 * ::psoCommit.
 *
 * This function does not provide a handle to the newly created object. Use
 * ::psoQueueOpen and similar functions to get the handle.
 *
 * \param[in] sessionHandle Handle to the current session.
 * \param[in] objectName The fully qualified name of the object. 
 * \param[in] nameLengthInBytes The length of \em objectName (in bytes) not
 *            counting the null terminator.
 * \param[in] definition The basic information needed to create the object:
 *            the type of object to create, etc.
 * \param[in] dataDefHandle Handle to the definition of the data fields.
 *            It can be set to NULL when creating a Folder.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoCreateQueue( PSO_HANDLE            sessionHandle,
                    const char          * objectName,
                    psoUint32             nameLengthInBytes,
                    psoObjectDefinition * definition,
                    PSO_HANDLE            dataDefHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Create a new key-based object in shared memory.
 *
 * The currently supported types of key-based objects are hash maps and
 * read-only hash maps.
 *
 * The creation of the object only becomes permanent after a call to 
 * ::psoCommit.
 *
 * This function does not provide a handle to the newly created object. Use
 * ::psoHashMapOpen and similar functions to get the handle.
 *
 * \param[in] sessionHandle Handle to the current session.
 * \param[in] objectName The fully qualified name of the object. 
 * \param[in] nameLengthInBytes The length of \em objectName (in bytes) not
 *            counting the null terminator.
 * \param[in] definition The basic information needed to create the object:
 *            the type of object to create, etc.
 * \param[in] dataDefHandle Handle to the definition of the data fields.
 * \param[in] keyDefHandle Handle to the definition of the key.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoCreateMap( PSO_HANDLE            sessionHandle,
                  const char          * objectName,
                  psoUint32             nameLengthInBytes,
                  psoObjectDefinition * definition,
                  PSO_HANDLE            dataDefHandle,
                  PSO_HANDLE            keyDefHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Destroy an existing object in shared memory.
 *
 * The destruction of the object only becomes permanent after a call to 
 * ::psoCommit.
 *
 * \param[in]  sessionHandle Handle to the current session.
 * \param[in]  objectName The fully qualified name of the object. 
 * \param[in]  nameLengthInBytes The length of \em objectName (in bytes) not
 *             counting the null terminator (null-terminators are not used by
 *             the Photon engine).
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoDestroyObject( PSO_HANDLE   sessionHandle,
                      const char * objectName,
                      psoUint32    nameLengthInBytes );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Return the error message associated with the last error(s).
 *
 * If the length of the error message is greater than the length of the
 * provided buffer, the error message will be truncated to fit in the
 * provided buffer.
 *
 * Caveat, some basic errors cannot be captured, if the provided handles
 * (session handles or object handles) are incorrect (NULL, for example).
 * Without a proper handle, the code cannot know where to store
 * the error...
 *
 * \param[in]   sessionHandle Handle to the current session.
 * \param[out]  message Buffer for the error message. Memory allocation for 
 *              this buffer is the responsability of the caller.
 * \param[in]   msgLengthInBytes The length of \em message (in bytes). Must
 *              be at least 32 bytes.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoErrorMsg( PSO_HANDLE   sessionHandle,
                 char       * message,
                 psoUint32    msgLengthInBytes );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Terminate the current session. 
 *
 * An implicit call to ::psoRollback is executed by this function.
 *
 * Once this function is executed, attempts to use the session handle
 * or the handles of objects accessed using this session might lead to
 * memory violation (and, possibly, crashes).
 *
 * \param[in]  sessionHandle Handle to the current session.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoExitSession( PSO_HANDLE sessionHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Retrieve the data definition of the named object.
 *
 * To avoid memory leaks, you must close the handle that will be 
 * returned by this function (see ::psoDataDefClose).
 *
 * The handle might be set to NULL by this function if the object is a folder.
 *
 * \param[in]  sessionHandle Handle to the current session.
 * \param[in]  objectName The fully qualified name of the object. 
 * \param[in]  nameLengthInBytes The length of \em objectName (in bytes) not
 *             counting the null terminator (null-terminators are not used by
 *             the Photon engine).
 * \param[out] dataDefHandle Handle to the definition of the data fields (or
 *             NULL for folders).
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoGetDataDefinition( PSO_HANDLE   sessionHandle,
                          const char * objectName,
                          psoUint32    nameLengthInBytes,
                          PSO_HANDLE * dataDefHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Retrieve the object definition of the named object.
 *
 * \param[in]  sessionHandle Handle to the current session.
 * \param[in]  objectName The fully qualified name of the object. 
 * \param[in]  nameLengthInBytes The length of \em objectName (in bytes) not
 *             counting the null terminator.
 * \param[out] definition The definition of the object.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoGetDefinition( PSO_HANDLE            sessionHandle,
                      const char          * objectName,
                      psoUint32             nameLengthInBytes,
                      psoObjectDefinition * definition );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Return information on the current status of the shared memory.
 *
 * The fetched information is mainly about the current status of the memory 
 * allocator and information needed to provide better support (compiler, etc.).
 *
 * \param[in]  sessionHandle Handle to the current session.
 * \param[out] pInfo      A pointer to the ::psoInfo structure.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoGetInfo( PSO_HANDLE   sessionHandle,
                psoInfo    * pInfo );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Retrieve the key definition of the named object.
 *
 * To avoid memory leaks, you must close the handle that will be 
 * returned by this function (see ::psoKeyDefClose).
 *
 * The handle might be set to NULL by this function if the object does
 * not have keys (folders and queues, for example).
 *
 * \param[in]  sessionHandle Handle to the current session.
 * \param[in]  objectName The fully qualified name of the object. 
 * \param[in]  nameLengthInBytes The length of \em objectName (in bytes) not
 *             counting the null terminator (null-terminators are not used by
 *             the Photon engine).
 * \param[out] keyDefHandle Handle to the key definition (or NULL for folders,
 *             queues, etc.).
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoGetKeyDefinition( PSO_HANDLE   sessionHandle,
                         const char * objectName,
                         psoUint32    nameLengthInBytes,
                         PSO_HANDLE * keyDefHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Return the status of the named object.
 *
 * \param[in]  sessionHandle Handle to the current session.
 * \param[in]  objectName The fully qualified name of the object. 
 * \param[in]  nameLengthInBytes The length of \em objectName (in bytes) not
 *             counting the null terminator (null-terminators are not used by
 *             the Photon engine).
 * \param[out] pStatus A pointer to the psoObjStatus structure.
 *
 * \return 0 on success or a ::psoErrors on error.
 */

PHOTON_EXPORT
int psoGetStatus(  PSO_HANDLE     sessionHandle,
                   const char   * objectName,
                   psoUint32      nameLengthInBytes,
                   psoObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes a session. It takes one output argument, 
 * the session handle.
 * 
 * Upon successful completion, the session handle is set and the function
 * returns zero. Otherwise the error code is returned and the handle is set
 * to NULL.
 * 
 * This function will also initiate a new transaction.
 *
 * Upon normal termination, the current transaction is rolled back. You
 * MUST explicitly call psonCommit to save your changes.
 *
 * \param[out] sessionHandle The handle to the newly created session.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoInitSession( PSO_HANDLE * sessionHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Return the last error seen in previous calls (of the current session).
 *
 * Caveat, some basic errors cannot be captured, if the provided handles
 * (session handles or object handles) are incorrect (NULL, for example).
 * Without a proper handle, the code cannot know where to store
 * the error...
 *
 * \param[in] sessionHandle Handle to the current session.
 *
 * \return The last error.
 */
PHOTON_EXPORT
int psoLastError( PSO_HANDLE sessionHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Rollback all insertions and deletions (of the current session) executed 
 * since the previous call to psoCommit or psoRollback.
 *
 * Insertions and deletions subjected to this call include both data items
 * inserted and deleted from data containers (maps, etc.) and objects 
 * themselves created with ::psoCreateObj and/or destroyed with 
 * ::psoDestroyObj.
 *
 * Note: the internal calls executed by the engine to satisfy this request
 * cannot fail. As such,
 * you cannot find yourself with an ugly situation where some operations
 * were rollbacked and others not. If an error is returned by this function,
 * nothing was rollbacked.
 *
 * \param[in] sessionHandle Handle to the current session.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoRollback( PSO_HANDLE sessionHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*@}*/

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_SESSION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

