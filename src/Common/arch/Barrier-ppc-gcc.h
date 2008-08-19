/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#ifndef PSC_ARCH_BARRIER_H
#define PSC_ARCH_BARRIER_H

/*
 * Barrier for the Power PC when using gcc or a compiler
 * understanding the gcc inline assembly.
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline void pscMemoryBarrier()
{
   __asm__ __volatile__("sync" : : : "memory");
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline void pscReadMemoryBarrier()
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

static inline void pscWriteMemoryBarrier()
{
   __asm__ __volatile__("eieio" : : : "memory");
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSC_ARCH_BARRIER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

