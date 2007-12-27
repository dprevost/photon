/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSW_VERIFY_COMMON_H
#define VDSW_VERIFY_COMMON_H

#include "Engine/MemoryHeader.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

extern vdseMemoryHeader * g_pMemoryAddress;
extern bool g_bTestAllocator;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct vdseLinkedList;
struct vdseFolder;
struct vdseHashMap;
struct vdseQueue;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswValidation 
{
   VDSW_OK = 0,
   VDSW_DELETE_OBJECT,
   VDSW_UNHANDLE_ERROR
   
};
typedef enum vdswValidation vdswValidation;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct vdswVerifyStruct
{
   bool   verbose;
   int    spaces;
   FILE * fp;
   bool   doRepair;
};
typedef struct vdswVerifyStruct vdswVerifyStruct;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdswEcho( vdswVerifyStruct * pVerify, char * message )
{
   int i;
   
   if ( pVerify->verbose ) {
      for ( i = 0; i < pVerify->spaces; ++i )
         fprintf( pVerify->fp, " " );
      fprintf( pVerify->fp, "%s\n", message );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool vdswVerifyOffset( vdswVerifyStruct * pVerify, ptrdiff_t offset )
{
   return (offset > 0 && 
      offset < (ptrdiff_t)((vdseMemoryHeader *)g_pBaseAddr)->totalLength );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswValidation 
vdswVerifyFolder( vdswVerifyStruct   * pVerify,
                  struct vdseFolder  * pFolder,
                  vdseSessionContext * pContext );
int
vdswVerifyHash( vdswVerifyStruct * pVerify,
                struct vdseHash  * pHash,
                ptrdiff_t          memObjOffset );
enum vdswValidation 
vdswVerifyList( vdswVerifyStruct      * pVerify,
                struct vdseLinkedList * pList );
enum vdswValidation 
vdswVerifyHashMap( vdswVerifyStruct   * pVerify,
                   struct vdseHashMap * pHashMap, 
                   vdseSessionContext * pContext  );
enum vdswValidation 
vdswVerifyQueue( vdswVerifyStruct * pVerify,
                 struct vdseQueue * pQueue );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSW_VERIFY_COMMON_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

