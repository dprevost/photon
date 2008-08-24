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
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Common/DirAccess.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   pscDirIterator iterator;

   pscInitDir( &iterator );

   if ( iterator.initialized != PSC_DIR_ACCESS_SIGNATURE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
#if defined(WIN32)
   if ( iterator.handle != PSO_INVALID_HANDLE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( iterator.dirName[0] != '\0' ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
#else
   if ( iterator.pDir != NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
#endif

   pscFiniDir( &iterator );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

