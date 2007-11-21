/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDS_FOLDER_H
#define VDS_FOLDER_H

#include <vdsf/vdsCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 * This file provides the API needed to access a VDSF folder.
 */
/**
 * \defgroup vdsFolder_c API functions for vdsf folders.
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
 * \param[in] objectHandle The handle to the folder (see ::vdsFolderOpen).
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsFolderClose( VDS_HANDLE objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Iterate through the folder - no data items are removed from the folder
 * by this function.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by the iterator. Likewise, destroyed data items (even if
 * not yet committed) are invisible.
 *
 * \param[in]   objectHandle The handle to the folder (see ::vdsFolderOpen).
 * \param[out]  pEntry The data structure provided by the user to hold the 
 *              content of each item in the folder. Memory allocation for 
 *              this buffer is the responsability of the caller.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsFolderGetFirst( VDS_HANDLE       objectHandle,
                       vdsFolderEntry * pEntry );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Iterate through the folder.
 *
 * Data items which were added by another session and are not yet committed 
 * will not be seen by the iterator. Likewise, destroyed data items (even if
 * not yet committed) are invisible.
 *
 * Evidently, you must call ::vdsFolderGetFirst to initialize the iterator. 
 *
 * \param[in]   objectHandle The handle to the folder (see ::vdsFolderOpen).
 * \param[out]  pEntry The data structure provided by the user to hold the 
 *              content of each item in the folder. Memory allocation for 
 *              this buffer is the responsability of the caller.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsFolderGetNext( VDS_HANDLE       objectHandle,
                      vdsFolderEntry * pEntry );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Open an existing folder (see ::vdsCreateObject to create a new folder).
 *
 * \param[in]  sessionHandle The handle to the current session.
 * \param[in]  folderName The fully qualified name of the folder. 
 * \param[in]  nameLengthInBytes The length of \em folderName (in bytes) not
 *             counting the null terminator (null-terminators are not used by
 *             the vdsf engine).
 * \param[out] objectHandle The handle to the folder, allowing us access to
 *             the folder in shared memory. On error, this handle will be set
 *             to zero (NULL) unless the objectHandle pointer itself is NULL.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsFolderOpen( VDS_HANDLE   sessionHandle,
                   const char * folderName,
                   size_t       nameLengthInBytes,
                   VDS_HANDLE * objectHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Return the status of the folder.
 *
 * \param[in]  objectHandle The handle to the folder (see ::vdsFolderOpen).
 * \param[out] pStatus      A pointer to the status structure.
 *
 * \return 0 on success or a ::vdsErrors on error.
 */
VDSF_EXPORT
int vdsFolderStatus( VDS_HANDLE     objectHandle,
                     vdsObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*@}*/

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDS_FOLDER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

