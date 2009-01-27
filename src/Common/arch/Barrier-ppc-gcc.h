/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSOC_ARCH_BARRIER_H
#define PSOC_ARCH_BARRIER_H

/*
 * Barrier for the Power PC when using gcc or a compiler
 * understanding the gcc inline assembly.
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline void psocMemoryBarrier()
{
   __asm__ __volatile__("sync" : : : "memory");
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline void psocReadMemoryBarrier()
{
   /* 
    * Normally, lwsync should work on newer 64 bits cpus. There are
    * however some cases on older cpus where an illegal instruction was
    * thown (instead of issuing a sync). This was discussed for the linux
    * kernel.
    *
    * The problematic cpu seems to be the E500, still current but 32 bits.
    * (http://en.wikipedia.org/wiki/PowerPC_e500
    *
    * We may want to change this to lwsync, if needed, but for the moment 
    * leave it as is.
    */
   __asm__ __volatile__("sync" : : : "memory");
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline void psocWriteMemoryBarrier()
{
   __asm__ __volatile__("eieio" : : : "memory");
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOC_ARCH_BARRIER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

