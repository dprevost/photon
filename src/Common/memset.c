/*
 * The original source from this function was taken from:
 * http://www.opensource.apple.com/darwinsource/10.3/gcc_os-1256/libiberty/memcmp.c
 * 
 * It was slightly modified for inclusion into Photon by
 * D. Prevost, May 2004.
 */

/* memset
   This implementation is in the public domain.  */

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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void *
memset( void* dest, int val, size_t len)
{
   register unsigned char *ptr = (unsigned char*)dest;

   while (len-- > 0) {
      *ptr++ = val;
   }
   
   return dest;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

