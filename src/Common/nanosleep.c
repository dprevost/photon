/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#if defined WIN32
#  include "config-win32.h"
#endif
#if defined WIN32
#  include <winsock2.h>
#endif
#include "Common.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Note: this version of nanosleep is fundamentally incorrect since the
 * the remaining time is not calculated... but for our purposes, it 
 * really does not matter! We are not attempting to rewrite libc here.

[EINTR]
    The nanosleep() function was interrupted by a signal. 
[EINVAL]
    The rqtp argument specified a nanosecond value less than zero or greater than or equal to 1000 million. 
[ENOSYS]
    The nanosleep() function is not supported by this implementation.
 */
int  
nanosleep( const struct timespec * pRequest, 
           struct timespec       * pRemain )
{
   struct timeval req;
   int ret;
   
   pRemain = pRemain;
   req.tv_sec  = pRequest->tv_sec;
   req.tv_usec = pRequest->tv_nsec / 1000;
   if ( req.tv_sec *1000000 + req.tv_usec < 0  ||
      req.tv_sec *1000000 + req.tv_usec >= 1000000000 ) {
      fprintf( stderr, "%u %u\n", req.tv_sec, req.tv_usec );
      errno = EINVAL;
      return -1;
   }
   
   
   ret = select (1, 0, 0, 0, &req );

   return ret;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

