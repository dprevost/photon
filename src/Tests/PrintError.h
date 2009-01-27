/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Common/ErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void printError( psocErrorHandler* pError )
{
   char* msg = NULL;
   size_t length = 0;

   if ( pError == NULL ) {
      fprintf( stderr, "Null ErrorHandler...can't help you!\n" );
   }
   else {
      if ( psocAnyErrors( pError ) ) {

         length = psocGetErrorMsgLength( pError );
         if ( length == 0 ) {
            fprintf( stderr, "Error in errorHandler... a bit ironic!\n" );
         }
         else {
            msg = (char*)malloc( length+1 );
            if ( msg == NULL ) {
               fprintf( stderr, "Malloc error in printError\n" );
            }
            else {
               psocGetErrorMsg( pError, msg, length+1 );
               fprintf( stderr, "Error message: %s\n", msg );
            }
         }
      }
   }
   if ( msg != NULL ) free(msg);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(WIN32)
#  pragma warning(disable:4127)
#  pragma warning(disable:4702)
#endif

#define ERROR_EXIT(EXPECTED_TO_PASS,PERROR,SPECIAL_CLEANUP_CODE) \
{\
   fprintf( stderr, "Abnormal error at line %d\n", __LINE__ );\
   if ( PERROR != NULL ) {\
      printError(PERROR);\
   }\
   SPECIAL_CLEANUP_CODE;\
   if (EXPECTED_TO_PASS) {\
      return 1;\
   }\
   else {\
      return 0;\
   }\
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

