/*
 * hash_32 - 32 bit Fowler/Noll/Vo hash code
 *
 * @(#) $Revision: 1.8 $
 * @(#) $Id: hash_32.c,v 1.8 2003/10/03 20:38:13 chongo Exp $
 * @(#) $Source: /usr/local/src/cmd/fnv/RCS/hash_32.c,v $
 *
 * hash_64 - 64 bit Fowler/Noll/Vo-0 hash code
 *
 * @(#) $Revision: 1.8 $
 * @(#) $Id: hash_64.c,v 1.8 2003/10/03 20:37:04 chongo Exp $
 * @(#) $Source: /usr/local/src/cmd/fnv/RCS/hash_64.c,v $
 *
 ***
 *
 * Fowler/Noll/Vo hash
 *
 * The basis of this hash algorithm was taken from an idea sent
 * as reviewer comments to the IEEE POSIX P1003.2 committee by:
 *
 *      Phong Vo (http://www.research.att.com/info/kpv/)
 *      Glenn Fowler (http://www.research.att.com/~gsf/)
 *
 * In a subsequent ballot round:
 *
 *      Landon Curt Noll (http://www.isthe.com/chongo/)
 *
 * improved on their algorithm.  Some people tried this hash
 * and found that it worked rather well.  In an EMail message
 * to Landon, they named it the ``Fowler/Noll/Vo'' or FNV hash.
 *
 * FNV hashes are designed to be fast while maintaining a low
 * collision rate. The FNV speed allows one to quickly hash lots
 * of data while maintaining a reasonable collision rate.  See:
 *
 *      http://www.isthe.com/chongo/tech/comp/fnv/index.html
 *
 * for more details as well as other forms of the FNV hash.
 ***
 *
 * To use the recommended FNV-1 hash, pass FNV1_INIT as the
 * hashval argument to fnv_buf().
 *
 ***
 *
 * Please do not copyright this code.  This code is in the public domain.
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * By:
 *	chongo <Landon Curt Noll> /\oo/\
 *      http://www.isthe.com/chongo/
 *
 * Share and Enjoy!	:-)
 */

#include "Common/Common.h"
#include "hash_fnv.h"

#if SIZEOF_VOID_P == 4

/*
 * 32 bit magic FNV-0 and FNV-1 prime
 */
#define FNV_32_PRIME ((size_t)0x01000193)

/*
 * fnv_32_buf - perform a 32 bit Fowler/Noll/Vo hash on a buffer
 *
 * input:
 *	buf	- start of buffer to hash
 *	len	- length of buffer in octets
 *	hval	- previous hash value or 0 if first call
 *
 * returns:
 *	32 bit hash as a static hash type
 *
 * NOTE: To use the recommended 32 bit FNV-1 hash, use FNV1_32_INIT as the hval
 *	 argument on the first call to either fnv_32_buf().
 */
size_t 
fnv_buf( void *buf, size_t len, size_t hval )
{
   unsigned char *bp = (unsigned char *)buf;	/* start of buffer */
   unsigned char *be = bp + len;		/* beyond end of buffer */

   /*
    * FNV-1 hash each octet in the buffer
    */
   while (bp < be) {

	/* multiply by the 32 bit FNV magic prime mod 2^32 */
#if defined(NO_FNV_GCC_OPTIMIZATION)
      hval *= FNV_32_PRIME;
#else
      hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
#endif

	/* xor the bottom with the current octet */
      hval ^= (size_t)*bp++;
   }

    /* return our new hash value */
   return hval;
}

#else /* compiling on 64 bits machine */

/*
 * 64 bit magic FNV-0 and FNV-1 prime
 */
#define FNV_64_PRIME ((Fnv64_t)0x100000001b3ULL)


/*
 * fnv_64_buf - perform a 64 bit Fowler/Noll/Vo hash on a buffer
 *
 * input:
 *	buf	- start of buffer to hash
 *	len	- length of buffer in octets
 *	hval	- previous hash value or 0 if first call
 *
 * returns:
 *	64 bit hash as a static hash type
 *
 * NOTE: To use the recommended 64 bit FNV-1 hash, use FNV1_64_INIT as the hval
 *	 argument on the first call to either fnv_64_buf().
 */
size_t
fnv_buf( void *buf, size_t len, size_t hval)
{
   unsigned char *bp = (unsigned char *)buf;	/* start of buffer */
   unsigned char *be = bp + len;		/* beyond end of buffer */

   /*
    * FNV-1 hash each octet of the buffer
    */
   while (bp < be) {

      /* multiply by the 64 bit FNV magic prime mod 2^64 */
#if defined(NO_FNV_GCC_OPTIMIZATION)
      hval *= FNV_64_PRIME;
#else
      hval += (hval << 1) + (hval << 4) + (hval << 5) +
         (hval << 7) + (hval << 8) + (hval << 40);
#endif

      /* xor the bottom with the current octet */
      hval ^= (size_t)*bp++;
   }

   /* return our new hash value */
   return hval;
}

#endif /* SIZEOF_VOID_P */

