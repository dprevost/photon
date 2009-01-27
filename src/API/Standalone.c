/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Common/ErrorHandler.h"
#include <photon/psoErrors.h>
#include "Common/MemoryFile.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaStandalone( const char       * address,
                    char             * path,
                    size_t           * memorySizekb,
                    psocErrorHandler * errorHandler )
{
   int errcode = 0;
#if ! HAVE_STAT
   FILE* fp = NULL;
#endif
#if HAVE__STAT
   struct _stat info;
#elif HAVE_STAT
   struct stat info;
#else
   size_t length = 0;
#endif

   PSO_PRE_CONDITION( address      != NULL );
   PSO_PRE_CONDITION( path         != NULL );
   PSO_PRE_CONDITION( memorySizekb != NULL );
   PSO_PRE_CONDITION( errorHandler != NULL );
   
   strncpy( path, &address[strlen("file:")], PATH_MAX-1 );
   
#if HAVE_STAT
   errcode = stat( path, &info );
   if ( errcode != 0 ) goto error;
   *memorySizekb = info.st_size/1024;
#elif HAVE__STAT
   errcode = _stat( path, &info );
   if ( errcode != 0 ) goto error;
   *memorySizekb = info.st_size/1024;
#else
   fp = fopen( path, "rb" );
   if ( fp == NULL ) goto error;
   if (fseek(fp, 0, SEEK_END) == 0 ) {
      length = ftell(fp);
   }
   fclose(fp);
   if ( length == EOF ) *memorySizekb = 0;
   else  *memorySizekb = length/1024;
#endif

   return 0;
   
 error:
   return PSO_BACKSTORE_FILE_MISSING;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

