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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#if defined WIN32
#  include "ConfigHeaders/config-win32.h"
#endif
#include <stdio.h>
#if HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#if STDC_HEADERS
#  include <stdlib.h>
#  include <stddef.h>
#else
#  if HAVE_STDLIB_H
#    include <stdlib.h>
#  endif
#endif
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif

/*
 * Note: this version of nanosleep is fundamentally incorrect since the
 * the remaining time is not calculated... but for our purposes, it 
 * really does not matter! We are not attempting to rewrite libc here.
 */
int  
nanosleep(const struct timespec * pRequest, 
          struct timespec * pRemain )
{
   struct timeval req;
   int ret;
   
   req.tv_sec  = pRequest->tv_sec;
   req.tv_usec = pRequest->tv_nsec / 1000;
   
   ret = select (1, 0, 0, 0, &req );

   return ret;
}
