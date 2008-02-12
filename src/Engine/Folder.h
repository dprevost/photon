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

#ifndef VDSE_FOLDER_H
#define VDSE_FOLDER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine/Engine.h"
#include "Engine/MemoryObject.h"
#include "Engine/TreeNode.h"
#include "Engine/BlockGroup.h"
#include "Engine/Hash.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdseFolderItem
{
   vdseHashItem * pHashItem;

   size_t      bucket;

   ptrdiff_t   itemOffset;

   int status;
   
} vdseFolderItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Wrapper for the container holding the information on the tree of
 * shared objects.
 */	

typedef struct vdseFolder
{
   /** The memory object struct. */
   struct vdseMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct vdseTreeNode  nodeObject;

   struct vdseHash      hashObj;

   /** Variable size struct - always put at the end */
   struct vdseBlockGroup blockGroup;

} vdseFolder;
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
int vdseFolderDeleteObject( vdseFolder         * pFolder,
                            const vdsChar_T    * objectName,
                            size_t               strLength, 
                            vdseSessionContext * pContext );
                            
VDSF_ENGINE_EXPORT
void vdseFolderFini( vdseFolder         * pFolder,
                     vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseFolderGetFirst( vdseFolder         * pFolder,
                        vdseFolderItem     * pItem,
                        vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseFolderGetNext( vdseFolder         * pFolder,
                       vdseFolderItem     * pItem,
                       vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseFolderGetObject( vdseFolder         * pFolder,
                         const vdsChar_T    * objectName,
                         size_t               strLength, 
                         vdseFolderItem     * pFolderItem,
                         vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseFolderGetStatus( vdseFolder         * pFolder,
                         const vdsChar_T    * objectName,
                         size_t               strLength, 
                         vdsObjStatus       * pStatus,
                         vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseFolderInit( vdseFolder         * pFolder,
                    ptrdiff_t            parentOffset,
                    size_t               numberOfBlocks,
                    size_t               expectedNumOfChilds,
                    vdseTxStatus       * pTxStatus,
                    size_t               origNameLength,
                    vdsChar_T          * origName,
                    ptrdiff_t            keyOffset,
                    vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseFolderInsertObject( vdseFolder         * pFolder,
                            const vdsChar_T    * objectName,
                            const vdsChar_T    * originalName,
                            size_t               strLength, 
                            enum vdsObjectType   objectType,
                            size_t               numBlocks,
                            size_t               expectedNumOfChilds,
                            vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseFolderRelease( vdseFolder         * pFolder,
                       vdseFolderItem     * pItem,
                       vdseSessionContext * pContext );

/** 
 * This function does the actual removal of the entry in the list of the
 * folder object (once there are no pending transactions, no sessions
 * have this object open, etc...).
 */
VDSF_ENGINE_EXPORT
void vdseFolderRemoveObject( vdseFolder         * pFolder,
                             vdseHashItem       * pHashItem,
                             vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseFolderResize( vdseFolder         * pFolder, 
                       vdseSessionContext * pContext  );

VDSF_ENGINE_EXPORT
void vdseFolderStatus( vdseFolder   * pFolder,
                       vdsObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The next 5 functions should only be used by the API, to create, destroy,
 * open or close a memory object. Or to obtain its status.
 */

VDSF_ENGINE_EXPORT
int vdseTopFolderCloseObject( vdseFolderItem     * pDescriptor,
                              vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseTopFolderCreateObject( vdseFolder         * pFolder,
                               const char         * objectName,
                               size_t               nameLengthInBytes,
                               enum vdsObjectType   objectType,
                               vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseTopFolderDestroyObject( vdseFolder         * pFolder,
                                const char         * objectName,
                                size_t               nameLengthInBytes,
                                vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseTopFolderGetStatus( vdseFolder         * pFolder,
                            const char         * objectName,
                            size_t               nameLengthInBytes,
                            vdsObjStatus       * pFolderItem,
                            vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseTopFolderOpenObject( vdseFolder         * pFolder,
                             const char         * objectName,
                             size_t               nameLengthInBytes,
                             vdseFolderItem     * pFolderItem,
                             vdseSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_FOLDER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
