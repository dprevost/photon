/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef PSO_FOLDER_H
#define PSO_FOLDER_H

#include <photon/psoCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 * This file provides the API needed to access a VDSF folder.
 */
/**
 * \defgroup psoFolder_c API functions for psof folders.
 *
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
VDSF_EXPORT
int psoFolderClose( PSO_HANDLE objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Create a new object in shared memory as a child of the current folder.
 *
 * The creation of the object only becomes permanent after a call to 
 * ::psoCommit.
 *
 * This function does not provide a handle to the newly created object. Use
 * psoQueueOpen and similar functions to get the handle.
 *
 * \param[in] folderHandle Handle to the current folder.
 * \param[in] objectName The name of the object. 
 * \param[in] nameLengthInBytes The length of \em objectName (in bytes) not
 *            counting the null terminator (null-terminators are not used by
 *            the psof engine).
 * \param[in] pDefinition The type of object to create (folder, queue, etc.)
 *            and the "optional" definition.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
VDSF_EXPORT
int psoFolderCreateObject( PSO_HANDLE            folderHandle,
                           const char    *       objectName,
                           size_t                nameLengthInBytes,
                           psoObjectDefinition * pDefinition );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Create a new object in shared memory as a child of the current folder.
 *
 * The creation of the object only becomes permanent after a call to 
 * ::psoCommit.
 *
 * This function does not provide a handle to the newly created object. Use
 * psoQueueOpen and similar functions to get the handle.
 *
 * \param[in] folderHandle Handle to the current folder.
 * \param[in] xmlBuffer    The XML buffer (string) containing all the required
 *                         information. 
 * \param[in] lengthInBytes The length of \em xmlBuffer (in bytes) not
 *                          counting the null terminator.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
VDSF_EXPORT
int psoFolderCreateObjectXML( PSO_HANDLE   folderHandle,
                              const char * xmlBuffer,
                              size_t       lengthInBytes );

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
 *             the psof engine).
 *
 * \return 0 on success or a ::psoErrors on error.
 */
VDSF_EXPORT
int psoFolderDestroyObject( PSO_HANDLE   folderHandle,
                            const char * objectName,
                            size_t       nameLengthInBytes );

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
VDSF_EXPORT
int psoFolderGetFirst( PSO_HANDLE       objectHandle,
                       psoFolderEntry * pEntry );

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
VDSF_EXPORT
int psoFolderGetNext( PSO_HANDLE       objectHandle,
                      psoFolderEntry * pEntry );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Open an existing folder (see ::psoCreateObject to create a new folder).
 *
 * \param[in]  sessionHandle The handle to the current session.
 * \param[in]  folderName The fully qualified name of the folder. 
 * \param[in]  nameLengthInBytes The length of \em folderName (in bytes) not
 *             counting the null terminator (null-terminators are not used by
 *             the psof engine).
 * \param[out] objectHandle The handle to the folder, allowing us access to
 *             the folder in shared memory. On error, this handle will be set
 *             to zero (NULL) unless the objectHandle pointer itself is NULL.
 *
 * \return 0 on success or a ::psoErrors on error.
 */
VDSF_EXPORT
int psoFolderOpen( PSO_HANDLE   sessionHandle,
                   const char * folderName,
                   size_t       nameLengthInBytes,
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
VDSF_EXPORT
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

