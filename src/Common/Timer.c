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

#include "Timer.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdscInitTimer( vdscTimer* pTimer )
{
#if defined (WIN32)
   pTimer->highResolution = TRUE;
   pTimer->frequency      = 0;
   pTimer->beginCount     = 0;
   pTimer->endCount       = 0;
#endif
   memset( &pTimer->timeBegin, 0, sizeof(struct timeval) );
   memset( &pTimer->timeEnd,   0, sizeof(struct timeval) );

#if defined (WIN32)
   pTimer->highResolution = QueryPerformanceFrequency(pTimer->frequency);
#endif   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void 
vdscCalculateTimer( vdscTimer *pTimer,
                    unsigned long *pSecs,
                    unsigned long *pnanoSecs )
{
   unsigned long tmp;
   
#if defined (WIN32)
   if ( pTimer->highResolution == TRUE )
   {
      *pSecs = (pTimer->endCount.QuadPart - pTimer->beginCount.QuadPart)
         / pTimer->frequency.QuadPart;
      *pnanoSecs = pTimer->endCount.QuadPart - 
         (*pSecs * pTimer->frequency.QuadPart);
      *pnanoSecs = (*pnanoSecs * 1000000000)/pTimer->frequency.QuadPart;
   }
   else
#endif
   {
      tmp = (pTimer->timeEnd.tv_sec - pTimer->timeBegin.tv_sec) * 1000000 + 
         pTimer->timeEnd.tv_usec - pTimer->timeBegin.tv_usec;

      *pSecs = tmp/ 1000000;
      *pnanoSecs = (tmp - *pSecs * 1000000)* 1000;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

