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

#ifndef PSOQ_VERIFY_COMMON_H
#define PSOQ_VERIFY_COMMON_H

#include "Nucleus/MemoryHeader.h"
#include "Nucleus/MemBitmap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

extern psonMemoryHeader * g_pMemoryAddress;
extern bool g_bTestAllocator;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonLinkedList;
struct psonFolder;
struct psonHashMap;
struct psonMap;
struct psonQueue;
struct psonMemObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoqRecoverError 
{
   PSOQ_REC_OK = 0,
   PSOQ_REC_CHANGES,
   PSOQ_REC_START_ERRORS = 100,
   /* Object added but not committed or object deleted and committed */
   PSOQ_REC_DELETED_OBJECT,
   PSOQ_REC_UNHANDLED_ERROR,
   PSOQ_REC_UNRECOVERABLE_ERROR
};
typedef enum psoqRecoverError psoqRecoverError;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psoqVerifyStruct
{
   bool   verbose;
   int    spaces;
   FILE * fp;
   bool   doRepair;
   size_t numObjectsOK;
   size_t numObjectsRepaired;
   size_t numObjectsDeleted;
   size_t numObjectsError;
   psonMemBitmap * pBitmap;
};
typedef struct psoqVerifyStruct psoqVerifyStruct;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psoqEcho( psoqVerifyStruct * pVerify, char * message )
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
void psoqResetBitmap( psonMemBitmap * pBitmap )
{
   size_t length;
   
   length = ( (pBitmap->lengthInBits - 1 ) >> 3 ) + 1;
   memset( pBitmap->bitmap, -1, length );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool psoqVerifyOffset( psoqVerifyStruct * pVerify, ptrdiff_t offset )
{
   return psonIsBufferFree( pVerify->pBitmap, offset );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void 
psoqPopulateBitmap( struct psoqVerifyStruct   * pVerify,
                    struct psonMemObject      * pMemObj,
                    struct psonSessionContext * pContext );

enum psoqRecoverError
psoqVerifyFastMap( psoqVerifyStruct   * pVerify,
                   struct psonMap     * pHashMap, 
                   psonSessionContext * pContext  );

enum psoqRecoverError
psoqVerifyFolder( psoqVerifyStruct   * pVerify,
                  struct psonFolder  * pFolder,
                  psonSessionContext * pContext );

enum psoqRecoverError
psoqVerifyHash( psoqVerifyStruct * pVerify,
                struct psonHash  * pHash,
                ptrdiff_t          memObjOffset );

enum psoqRecoverError
psoqVerifyList( psoqVerifyStruct      * pVerify,
                struct psonLinkedList * pList );

enum psoqRecoverError
psoqVerifyHashMap( psoqVerifyStruct   * pVerify,
                   struct psonHashMap * pHashMap, 
                   psonSessionContext * pContext  );

enum psoqRecoverError
psoqVerifyMemObject( struct psoqVerifyStruct   * pVerify,
                     struct psonMemObject      * pMemObj,
                     struct psonSessionContext * pContext );

enum psoqRecoverError
psoqVerifyQueue( psoqVerifyStruct   * pVerify,
                 struct psonQueue   * pQueue, 
                 psonSessionContext * pContext  );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOQ_VERIFY_COMMON_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

