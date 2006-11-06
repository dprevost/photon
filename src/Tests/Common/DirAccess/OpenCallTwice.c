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

int main( int argc, char* argv[] )
{
   /* 
    * This test handles the precondition that pDir must be NULL on 
    * entry to the Open function. This is done so that we don't 
    * loose our "handle" and therefore our access to the data. It
    * would also waste OS resources. 
    *
    * This test is a bit useless on Windows but... it does not hurt!
    */

   int errcode;
   vdscDirIterator iterator;
   vdscErrorHandler errorHandler;
   const char* str;

   vdscInitErrorDefs();
   vdscInitDir( &iterator );
   vdscInitErrorHandler( &errorHandler );
   
   errcode = vdscOpenDir( &iterator, "..", &errorHandler );

   if ( errcode != 0 )    return 0;
   if ( vdscAnyErrors( &errorHandler ) ) return 0;

   errcode = vdscOpenDir( &iterator, "..", &errorHandler );

   vdscCloseDir( &iterator );

   vdscFiniDir( &iterator );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;
}
