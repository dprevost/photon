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

#ifndef PSON_HASH_COMMON_H
#define PSON_HASH_COMMON_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psonHashResizeEnum
{
   PSON_HASH_NO_RESIZE,
   PSON_HASH_TIME_TO_GROW,
   PSON_HASH_TIME_TO_SHRINK
   
};

typedef enum psonHashResizeEnum psonHashResizeEnum;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The array length (for the hash) will double or shrink by a factor two
 * using precomputed lengths (all the lengths are prime numbers - using
 * prime numbers help reduces the risk of collision.
 *
 * To make it simple, we use the first prime number less than 2^n (except 
 * for 29 to make the jump from 13 to 61 smoother).
 *
 * The list of prime numbers was obtained from http://primes.utm.edu/,
 * specifically the page http://primes.utm.edu/lists/2small/0bit.html
 * (which list the first 10 prime numbers less than 2^n for n up to 400).
 */
 
#if SIZEOF_VOID_P == 4
#  define PSON_PRIME_NUMBER_ARRAY_LENGTH 28
PHOTON_ENGINE_EXPORT
extern size_t g_psonArrayLengths[PSON_PRIME_NUMBER_ARRAY_LENGTH];
#else
#  define PSON_PRIME_NUMBER_ARRAY_LENGTH 60
PHOTON_ENGINE_EXPORT
extern size_t g_psonArrayLengths[PSON_PRIME_NUMBER_ARRAY_LENGTH];
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_HASH_COMMON_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

