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

#include "ErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void printError( vdscErrorHandler* pError )
{
   char* msg = NULL;
   size_t length = 0;

   if ( pError == NULL )
      fprintf( stderr, "Null ErrorHandler...can't help you!\n" );
   else
   {
      if ( vdscAnyErrors( pError ) )
      {
         length = vdscGetErrorMsgLength( pError );
         if ( length == 0 )
            fprintf( stderr, "Error in errorHandler... a bit ironic!\n" );
         else
         {
            msg = (char*)malloc( length+1 );
            if ( msg == NULL )
            {
               fprintf( stderr, "Malloc error in printError\n" );
            }
            else
            {
               vdscGetErrorMsg( pError, msg, length+1 );
               fprintf( stderr, "Error message: %s\n", msg );
            }
         }
      }
   }
   if ( msg != NULL ) free(msg);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define ERROR_EXIT(RC,PERROR,SPECIAL_CLEANUP_CODE) \
{\
   fprintf( stderr, "Abnormal error at line %d\n", __LINE__ );\
   if ( PERROR != NULL )\
      printError(PERROR);\
   SPECIAL_CLEANUP_CODE;\
   return RC;\
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

