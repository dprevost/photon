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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common.h"
#include "DirAccess.h"

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main()
{
   vdscDirIterator iterator;

   vdscInitDir( &iterator );

   vdscFiniDir( &iterator );

   if ( iterator.initialized == VDSC_DIR_ACCESS_SIGNATURE )
      return -1;
   
#if defined(WIN32)
   if ( iterator.handle != VDS_INVALID_HANDLE ) return -1;
   if ( iterator.dirName[0] != '\0' ) return -1;
#else
   if ( iterator.pDir != NULL ) return -1;
#endif

   return 0;
}


