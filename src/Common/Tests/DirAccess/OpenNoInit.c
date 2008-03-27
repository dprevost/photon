/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Common/Common.h"
#include "Common/DirAccess.h"
#include "Tests/PrintError.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   int errcode;
   vdscDirIterator* pIterator;
   vdscErrorHandler errorHandler;
   
   vdscInitErrorDefs();

   pIterator = (vdscDirIterator*) malloc( sizeof(vdscDirIterator) );
   memset( pIterator, 0x7, sizeof(vdscDirIterator) );
   vdscInitErrorHandler( &errorHandler );
   
   errcode = vdscOpenDir( pIterator, "..", &errorHandler );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   
   vdscCloseDir( pIterator );

   vdscFiniDir( pIterator );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

