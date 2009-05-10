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

#ifndef PSO_FOLDER_H
#define PSO_FOLDER_H

#include <photon/psoCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 * This file provides the API needed to access a Photon folder.
 */
/**
 * \defgroup psoFolder_c API functions for Photon folders.
 *
 * Folders allow the organization of the data containers in a tree-like
 * structure (similar to a filesystem).
 *
 * If a flat structure is preferred, simply make all your data containers
 * children of the top folder ("\") - for example, "/MyQueue", "/MyMap", etc.
 * 
 */
/*@{*/

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Close a folder.
 *
 * This function terminates the current access to the folder in shared memory
 * (the folder itself is untouched).
 *
 * \param[in] objectHandle The handle to the folder (see ::psoFolderOpen).
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFolderClose( PSO_HANDLE objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Create a new folder in shared memory as a child of the current folder.
 *
 * The creation of the folder only becomes permanent after a call to 
 * ::psoCommit.
 *
 * This function does not provide a handle to the newly created folder. Use
 * psoFolderOpen to get the handle.
 *
 * \param[in] folderHandle Handle to the current folder.
 * \param[in] objectName The name of the object. 
 * \param[in] nameLengthInBytes The length of \em objectName (in bytes) not
 *            counting the null terminator.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFolderCreateFolder( PSO_HANDLE   folderHandle,
                           const char * objectName,
                           psoUint32    nameLengthInBytes );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Create a new queue in shared memory as a child of the current folder.
 *
 * The creation of the object only becomes permanent after a call to 
 * ::psoCommit.
 *
 * This function does not provide a handle to the newly created object. Use
 * ::psoQueueOpen and similar functions to get the handle.
 *
 * \param[in] folderHandle Handle to the current folder.
 * \param[in] objectName The name of the object. 
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
int psoFolderCreateQueue( PSO_HANDLE            folderHandle,
                          const char          * objectName,
                          psoUint32             nameLengthInBytes,
                          psoObjectDefinition * definition,
                          PSO_HANDLE            dataDefHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Create a new key-based object in shared memory as a child of the 
 * current folder.
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
 * \param[in] folderHandle Handle to the current folder.
 * \param[in] objectName The name of the object. 
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
int psoFolderCreateMap( PSO_HANDLE            folderHandle,
                        const char          * objectName,
                        psoUint32             nameLengthInBytes,
                        psoObjectDefinition * definition,
                        PSO_HANDLE            dataDefHandle,
                        PSO_HANDLE            keyDefHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Destroy an object, child of the current folder, in shared memory.
 *
 * The destruction of the object only becomes permanent after a call to 
 * ::psoCommit.
 *
 * \param[in]  folderHandle Handle to the current folder.
 * \param[in]  objectName The name of the object. 
 * \param[in]  nameLengthInBytes The length of \em objectName (in bytes) not
 *             counting the null terminator (null-terminators are not used by
 *             the Photon engine).
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFolderDestroyObject( PSO_HANDLE   folderHandle,
                            const char * objectName,
                            psoUint32    nameLengthInBytes );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Retrieve the data definition of the named object.
 *
 * To avoid memory leaks, you must close the handle that will be 
 * returned by this function (see ::psoDataDefClose).
 *
 * The handle might be set to NULL by this function if the object is a folder.
 *
 * \param[in]  folderHandle Handle to the current folder.
 * \param[in]  objectName The name of the object. 
 * \param[in]  nameLengthInBytes The length of \em objectName (in bytes) not
 *             counting the null terminator.
 * \param[out] dataDefHandle Handle to the definition of the data fields.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFolderGetDataDefinition( PSO_HANDLE   folderHandle,
                                const char * objectName,
                                psoUint32    nameLengthInBytes,
                                PSO_HANDLE * dataDefHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \brief Retrieve the object definition of the named object.
 *
 * \param[in]  folderHandle Handle to the current folder.
 * \param[in]  objectName The name of the object. 
 * \param[in]  nameLengthInBytes The length of \em objectName (in bytes) not
 *             counting the null terminator.
 * \param[out] definition The definition of the object.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFolderGetDefinition( PSO_HANDLE            folderHandle,
                            const char          * objectName,
                            psoUint32             nameLengthInBytes,
                            psoObjectDefinition * definition );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Iterate through the folder - no data items are removed from the folder
 * by this function.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by the iterator. Likewise, destroyed data items (even if
 * not yet committed) are invisible.
 *
 * \param[in]   objectHandle The handle to the folder (see ::psoFolderOpen).
 * \param[out]  pEntry The data structure provided by the user to hold the 
 *              content of each item in the folder. Memory allocation for 
 *              this buffer is the responsability of the caller.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFolderGetFirst( PSO_HANDLE       objectHandle,
                       psoFolderEntry * pEntry );

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
 * \param[in]  folderHandle Handle to the current folder.
 * \param[in]  objectName The name of the object. 
 * \param[in]  nameLengthInBytes The length of \em objectName (in bytes) not
 *             counting the null terminator.
 * \param[out] keyDefHandle Handle to the key definition(or NULL for folders,
 *             queues, etc.).
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFolderGetKeyDefinition( PSO_HANDLE   folderHandle,
                               const char * objectName,
                               psoUint32    nameLengthInBytes,
                               PSO_HANDLE * keyDefHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Iterate through the folder.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by the iterator. Likewise, destroyed data items (even if
 * not yet committed) are invisible.
 *
 * Evidently, you must call ::psoFolderGetFirst to initialize the iterator. 
 *
 * \param[in]   objectHandle The handle to the folder (see ::psoFolderOpen).
 * \param[out]  pEntry The data structure provided by the user to hold the 
 *              content of each item in the folder. Memory allocation for 
 *              this buffer is the responsability of the caller.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFolderGetNext( PSO_HANDLE       objectHandle,
                      psoFolderEntry * pEntry );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Open an existing folder (see ::psoCreateFolder to create a new folder).
 *
 * \param[in]  sessionHandle The handle to the current session.
 * \param[in]  folderName The fully qualified name of the folder. 
 * \param[in]  nameLengthInBytes The length of \em folderName (in bytes) not
 *             counting the null terminator (null-terminators are not used by
 *             the Photon engine).
 * \param[out] objectHandle The handle to the folder, allowing us access to
 *             the folder in shared memory. On error, this handle will be set
 *             to zero (NULL) unless the objectHandle pointer itself is NULL.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFolderOpen( PSO_HANDLE   sessionHandle,
                   const char * folderName,
                   psoUint32    nameLengthInBytes,
                   PSO_HANDLE * objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Return the status of the folder.
 *
 * \param[in]  objectHandle The handle to the folder (see ::psoFolderOpen).
 * \param[out] pStatus      A pointer to the status structure.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoFolderStatus( PSO_HANDLE     objectHandle,
                     psoObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*@}*/

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_FOLDER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

