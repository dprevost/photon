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
#include "Engine/DataType.h"
#include "Engine/MemoryObject.h"
#include "Engine/TreeNode.h"
#include "Engine/BlockGroup.h"
#include "Engine/Hash.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct vdseFolderItem
{
   vdseHashItem * pHashItem;

   ptrdiff_t   itemOffset;

   int status;
   
};

typedef struct vdseFolderItem vdseFolderItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Wrapper for the container holding the information on the tree of
 * shared objects.
 */
struct vdseFolder
{
   /** The memory object struct. */
   struct vdseMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct vdseTreeNode  nodeObject;

   struct vdseHash      hashObj;

   /** Variable size struct - always put at the end */
   struct vdseBlockGroup blockGroup;

};

typedef struct vdseFolder vdseFolder;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
void vdseFolderCommitEdit( vdseFolder          * pFolder,
                           vdseHashItem        * pHashItem, 
                           enum vdsObjectType    objectType,
                           vdseSessionContext  * pContext );

/**
 * Creates an immediate child of the folder.
 */
VDSF_ENGINE_EXPORT
bool vdseFolderCreateObject( vdseFolder          * pFolder,
                             const char          * objectName,
                             size_t                nameLengthInBytes,
                             vdsObjectDefinition * pDefinition,
                             vdseSessionContext  * pContext );

/**
 * Delete an object, recursively.
 */ 
VDSF_ENGINE_EXPORT
bool vdseFolderDeleteObject( vdseFolder         * pFolder,
                             const char         * objectName,
                             size_t               strLength, 
                             vdseSessionContext * pContext );

/**
 * Destroy an immediate child of the folder.
 */
VDSF_ENGINE_EXPORT
bool vdseFolderDestroyObject( vdseFolder         * pFolder,
                              const char         * objectName,
                              size_t               nameLengthInBytes,
                              vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool vdseFolderEditObject( vdseFolder         * pFolder,
                           const char         * objectName,
                           size_t               strLength, 
                           enum vdsObjectType   objectType, 
                           vdseFolderItem     * pFolderItem,
                           vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseFolderFini( vdseFolder         * pFolder,
                     vdseSessionContext * pContext );


VDSF_ENGINE_EXPORT
bool vdseFolderGetDefinition( vdseFolder          * pFolder,
                              const char          * objectName,
                              size_t                strLength,
                              vdsObjectDefinition * pDefinition,
                              vdseFieldDef       ** ppInternalDef,
                              vdseSessionContext  * pContext );

VDSF_ENGINE_EXPORT
bool vdseFolderGetFirst( vdseFolder         * pFolder,
                         vdseFolderItem     * pItem,
                         vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool vdseFolderGetNext( vdseFolder         * pFolder,
                        vdseFolderItem     * pItem,
                        vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool vdseFolderGetObject( vdseFolder         * pFolder,
                          const char         * objectName,
                          size_t               strLength, 
                          enum vdsObjectType   objectType, 
                          vdseFolderItem     * pFolderItem,
                          vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool vdseFolderGetStatus( vdseFolder         * pFolder,
                          const char         * objectName,
                          size_t               strLength, 
                          vdsObjStatus       * pStatus,
                          vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool vdseFolderInit( vdseFolder         * pFolder,
                     ptrdiff_t            parentOffset,
                     size_t               numberOfBlocks,
                     size_t               expectedNumOfChilds,
                     vdseTxStatus       * pTxStatus,
                     size_t               origNameLength,
                     char               * origName,
                     ptrdiff_t            hashItemOffset,
                     vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool vdseFolderInsertObject( vdseFolder          * pFolder,
                             const char          * objectName,
                             const char          * originalName,
                             size_t                strLength,
                             vdsObjectDefinition * pDefinition,
                             size_t                numBlocks,
                             size_t                expectedNumOfChilds,
                             vdseSessionContext  * pContext );

/* Retrieve the status of the current folder */
VDSF_ENGINE_EXPORT
void vdseFolderMyStatus( vdseFolder   * pFolder,
                         vdsObjStatus * pStatus );

VDSF_ENGINE_EXPORT
bool vdseFolderRelease( vdseFolder         * pFolder,
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
void vdseFolderRollbackEdit( vdseFolder          * pFolder,
                             vdseHashItem        * pHashItem, 
                             enum vdsObjectType    objectType,
                             vdseSessionContext  * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The next 7 functions should only be used by the API, to create, destroy,
 * open or close a memory object. Or to obtain its status or definition.
 */

VDSF_ENGINE_EXPORT
bool vdseTopFolderCloseObject( vdseFolderItem     * pDescriptor,
                               vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool vdseTopFolderCreateObject( vdseFolder          * pFolder,
                                const char          * objectName,
                                size_t                nameLengthInBytes,
                                vdsObjectDefinition * pDefinition,
                                vdseSessionContext  * pContext );

VDSF_ENGINE_EXPORT
bool vdseTopFolderDestroyObject( vdseFolder         * pFolder,
                                 const char         * objectName,
                                 size_t               nameLengthInBytes,
                                 vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool vdseTopFolderEditObject( vdseFolder         * pFolder,
                              const char         * objectName,
                              size_t               nameLengthInBytes,
                              enum vdsObjectType   objectType, 
                              vdseFolderItem     * pFolderItem,
                              vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool vdseTopFolderGetDef( vdseFolder          * pFolder,
                          const char          * objectName,
                          size_t                nameLengthInBytes,
                          vdsObjectDefinition * pDefinition,
                          vdseFieldDef       ** ppInternalDef,
                          vdseSessionContext  * pContext );

VDSF_ENGINE_EXPORT
bool vdseTopFolderGetStatus( vdseFolder         * pFolder,
                             const char         * objectName,
                             size_t               nameLengthInBytes,
                             vdsObjStatus       * pFolderItem,
                             vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool vdseTopFolderOpenObject( vdseFolder         * pFolder,
                              const char         * objectName,
                              size_t               nameLengthInBytes,
                              enum vdsObjectType   objectType, 
                              vdseFolderItem     * pFolderItem,
                              vdseSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_FOLDER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
