/*
 * The original source from this function was taken from:
 * http://www.opensource.apple.com/darwinsource/10.3/gcc_os-1256/libiberty/memcmp.c
 * 
 * It was slightly modified for inclusion into Photon by
 * D. Prevost, May 2004.
 */

/* memcmp -- compare two memory regions.
   This function is in the public domain.  */

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

int
memcmp( const void * str1, const void * str2, size_t count)
{
   register const unsigned char * s1 = (const unsigned char *)str1;
   register const unsigned char * s2 = (const unsigned char *)str2;

   while (count-- > 0) {
      if (*s1++ != *s2++) {
         return s1[-1] < s2[-1] ? -1 : 1;
      }
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

