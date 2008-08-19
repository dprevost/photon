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

#ifndef PSN_FOLDER_H
#define PSN_FOLDER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/DataType.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/TreeNode.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/Hash.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psnFolderItem
{
   psnHashItem * pHashItem;

   ptrdiff_t   itemOffset;

   int status;
   
};

typedef struct psnFolderItem psnFolderItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Wrapper for the container holding the information on the tree of
 * shared objects.
 */
struct psnFolder
{
   /** The memory object struct. */
   struct psnMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct psnTreeNode  nodeObject;

   struct psnHash      hashObj;

   /** Variable size struct - always put at the end */
   struct psnBlockGroup blockGroup;

};

typedef struct psnFolder psnFolder;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
void psnFolderCommitEdit( psnFolder          * pFolder,
                           psnHashItem        * pHashItem, 
                           enum vdsObjectType    objectType,
                           psnSessionContext  * pContext );

/**
 * Creates an immediate child of the folder.
 */
VDSF_ENGINE_EXPORT
bool psnFolderCreateObject( psnFolder          * pFolder,
                             const char          * objectName,
                             size_t                nameLengthInBytes,
                             vdsObjectDefinition * pDefinition,
                             psnSessionContext  * pContext );

/**
 * Delete an object, recursively.
 */ 
VDSF_ENGINE_EXPORT
bool psnFolderDeleteObject( psnFolder         * pFolder,
                             const char         * objectName,
                             size_t               strLength, 
                             psnSessionContext * pContext );

/**
 * Destroy an immediate child of the folder.
 */
VDSF_ENGINE_EXPORT
bool psnFolderDestroyObject( psnFolder         * pFolder,
                              const char         * objectName,
                              size_t               nameLengthInBytes,
                              psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnFolderEditObject( psnFolder         * pFolder,
                           const char         * objectName,
                           size_t               strLength, 
                           enum vdsObjectType   objectType, 
                           psnFolderItem     * pFolderItem,
                           psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
void psnFolderFini( psnFolder         * pFolder,
                     psnSessionContext * pContext );


VDSF_ENGINE_EXPORT
bool psnFolderGetDefinition( psnFolder          * pFolder,
                              const char          * objectName,
                              size_t                strLength,
                              vdsObjectDefinition * pDefinition,
                              psnFieldDef       ** ppInternalDef,
                              psnSessionContext  * pContext );

VDSF_ENGINE_EXPORT
bool psnFolderGetFirst( psnFolder         * pFolder,
                         psnFolderItem     * pItem,
                         psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnFolderGetNext( psnFolder         * pFolder,
                        psnFolderItem     * pItem,
                        psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnFolderGetObject( psnFolder         * pFolder,
                          const char         * objectName,
                          size_t               strLength, 
                          enum vdsObjectType   objectType, 
                          psnFolderItem     * pFolderItem,
                          psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnFolderGetStatus( psnFolder         * pFolder,
                          const char         * objectName,
                          size_t               strLength, 
                          vdsObjStatus       * pStatus,
                          psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnFolderInit( psnFolder         * pFolder,
                     ptrdiff_t            parentOffset,
                     size_t               numberOfBlocks,
                     size_t               expectedNumOfChilds,
                     psnTxStatus       * pTxStatus,
                     size_t               origNameLength,
                     char               * origName,
                     ptrdiff_t            hashItemOffset,
                     psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnFolderInsertObject( psnFolder          * pFolder,
                             const char          * objectName,
                             const char          * originalName,
                             size_t                strLength,
                             vdsObjectDefinition * pDefinition,
                             size_t                numBlocks,
                             size_t                expectedNumOfChilds,
                             psnSessionContext  * pContext );

/* Retrieve the status of the current folder */
VDSF_ENGINE_EXPORT
void psnFolderMyStatus( psnFolder   * pFolder,
                         vdsObjStatus * pStatus );

VDSF_ENGINE_EXPORT
bool psnFolderRelease( psnFolder         * pFolder,
                        psnFolderItem     * pItem,
                        psnSessionContext * pContext );

/** 
 * This function does the actual removal of the entry in the list of the
 * folder object (once there are no pending transactions, no sessions
 * have this object open, etc...).
 */
VDSF_ENGINE_EXPORT
void psnFolderRemoveObject( psnFolder         * pFolder,
                             psnHashItem       * pHashItem,
                             psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
void psnFolderResize( psnFolder         * pFolder, 
                       psnSessionContext * pContext  );


VDSF_ENGINE_EXPORT
void psnFolderRollbackEdit( psnFolder          * pFolder,
                             psnHashItem        * pHashItem, 
                             enum vdsObjectType    objectType,
                             psnSessionContext  * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The next 7 functions should only be used by the API, to create, destroy,
 * open or close a memory object. Or to obtain its status or definition.
 */

VDSF_ENGINE_EXPORT
bool psnTopFolderCloseObject( psnFolderItem     * pDescriptor,
                               psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnTopFolderCreateObject( psnFolder          * pFolder,
                                const char          * objectName,
                                size_t                nameLengthInBytes,
                                vdsObjectDefinition * pDefinition,
                                psnSessionContext  * pContext );

VDSF_ENGINE_EXPORT
bool psnTopFolderDestroyObject( psnFolder         * pFolder,
                                 const char         * objectName,
                                 size_t               nameLengthInBytes,
                                 psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnTopFolderEditObject( psnFolder         * pFolder,
                              const char         * objectName,
                              size_t               nameLengthInBytes,
                              enum vdsObjectType   objectType, 
                              psnFolderItem     * pFolderItem,
                              psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnTopFolderGetDef( psnFolder          * pFolder,
                          const char          * objectName,
                          size_t                nameLengthInBytes,
                          vdsObjectDefinition * pDefinition,
                          psnFieldDef       ** ppInternalDef,
                          psnSessionContext  * pContext );

VDSF_ENGINE_EXPORT
bool psnTopFolderGetStatus( psnFolder         * pFolder,
                             const char         * objectName,
                             size_t               nameLengthInBytes,
                             vdsObjStatus       * pFolderItem,
                             psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnTopFolderOpenObject( psnFolder         * pFolder,
                              const char         * objectName,
                              size_t               nameLengthInBytes,
                              enum vdsObjectType   objectType, 
                              psnFolderItem     * pFolderItem,
                              psnSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_FOLDER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
