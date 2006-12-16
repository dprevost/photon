/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "PageGroup.h"
#include "EngineTestCommon.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdsePageGroup *pGroup;
   vdsErrors errcode;
   unsigned char* ptr;
   
   initTest( expectedToPass );

   ptr = malloc( PAGESIZE*10 );
   if (ptr == NULL )
      ERROR_EXIT( expectedToPass, NULL, );
   g_pBaseAddr = ptr;
   
   /* This "100" (non-zero) offset should mark this page group 
    * as the first page group of a MemObject.
    */
   pGroup = (vdsePageGroup*) (ptr + 100);
   
   vdsePageGroupInit( NULL,
                      SET_OFFSET(ptr),
                      10 );

   ERROR_EXIT( expectedToPass, NULL, );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
