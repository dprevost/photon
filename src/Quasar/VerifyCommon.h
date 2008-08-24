/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSW_VERIFY_COMMON_H
#define VDSW_VERIFY_COMMON_H

#include "Nucleus/MemoryHeader.h"
#include "Nucleus/MemBitmap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

extern psnMemoryHeader * g_pMemoryAddress;
extern bool g_bTestAllocator;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psnLinkedList;
struct psnFolder;
struct psnHashMap;
struct psnMap;
struct psnQueue;
struct psnMemObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswRecoverError 
{
   VDSWR_OK = 0,
   VDSWR_CHANGES,
   VDSWR_START_ERRORS = 100,
   /* Object added but not committed or object deleted and committed */
   VDSWR_DELETED_OBJECT,
   VDSWR_UNHANDLED_ERROR,
   VDSWR_UNRECOVERABLE_ERROR
};
typedef enum vdswRecoverError vdswRecoverError;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct vdswVerifyStruct
{
   bool   verbose;
   int    spaces;
   FILE * fp;
   bool   doRepair;
   size_t numObjectsOK;
   size_t numObjectsRepaired;
   size_t numObjectsDeleted;
   size_t numObjectsError;
   psnMemBitmap * pBitmap;
};
typedef struct vdswVerifyStruct vdswVerifyStruct;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdswEcho( vdswVerifyStruct * pVerify, char * message )
{
   int i;
   
   if ( pVerify->verbose ) {
      for ( i = 0; i < pVerify->spaces; ++i ) {
         fprintf( pVerify->fp, " " );
      }
      fprintf( pVerify->fp, "%s\n", message );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdswResetBitmap( psnMemBitmap * pBitmap )
{
   size_t length;
   
   length = ( (pBitmap->lengthInBits - 1 ) >> 3 ) + 1;
   memset( pBitmap->bitmap, -1, length );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool vdswVerifyOffset( vdswVerifyStruct * pVerify, ptrdiff_t offset )
{
   return psnIsBufferFree( pVerify->pBitmap, offset );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void 
vdswPopulateBitmap( struct vdswVerifyStruct   * pVerify,
                    struct psnMemObject      * pMemObj,
                    struct psnSessionContext * pContext );

enum vdswRecoverError
vdswVerifyFastMap( vdswVerifyStruct   * pVerify,
                   struct psnMap     * pHashMap, 
                   psnSessionContext * pContext  );

enum vdswRecoverError
vdswVerifyFolder( vdswVerifyStruct   * pVerify,
                  struct psnFolder  * pFolder,
                  psnSessionContext * pContext );

enum vdswRecoverError
vdswVerifyHash( vdswVerifyStruct * pVerify,
                struct psnHash  * pHash,
                ptrdiff_t          memObjOffset );

enum vdswRecoverError
vdswVerifyList( vdswVerifyStruct      * pVerify,
                struct psnLinkedList * pList );

enum vdswRecoverError
vdswVerifyHashMap( vdswVerifyStruct   * pVerify,
                   struct psnHashMap * pHashMap, 
                   psnSessionContext * pContext  );

enum vdswRecoverError
vdswVerifyMemObject( struct vdswVerifyStruct   * pVerify,
                     struct psnMemObject      * pMemObj,
                     struct psnSessionContext * pContext );

enum vdswRecoverError
vdswVerifyQueue( vdswVerifyStruct   * pVerify,
                 struct psnQueue   * pQueue, 
                 psnSessionContext * pContext  );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSW_VERIFY_COMMON_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

