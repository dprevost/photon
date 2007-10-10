/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include <stdio.h>
#include "genError.h"
#include "testError.h"

int main()
{
   int err;
   char msg[MYERR_MAX_COMMENTS];

   err = myErrGetErrMessage( VDS_NOT_ENOUGH_MEMORY, msg, MYERR_MAX_COMMENTS );
   if ( err != 0 ) return -1;
   fprintf( stderr, "error = %d msg = %s\n", VDS_NOT_ENOUGH_MEMORY, msg );

   err = myErrGetErrMessage( VDS_OBJECT_ALREADY_PRESENT, msg, 
                             MYERR_MAX_COMMENTS );
   if ( err != 0 ) return -1;
   fprintf( stderr, "error = %d msg = %s\n", VDS_OBJECT_ALREADY_PRESENT, msg );

   err = myErrGetErrMessage( VDS_INVALID_WATCHDOG_ADDRESS, msg, 
                             MYERR_MAX_COMMENTS );
   if ( err != 0 ) return -1;
   fprintf( stderr, "error = %d msg = %s\n", 
            VDS_INVALID_WATCHDOG_ADDRESS, msg );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
