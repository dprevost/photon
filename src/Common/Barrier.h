/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSC_BARRIER_H
#define VDSC_BARRIER_H

#if defined(CONFIG_KERNEL_HEADERS)
#  include "Locking/linux/spinlock.h"
#  include "Locking/linux/compiler.h"
#endif

/*
 * Currently, these are simply wrappers to the equivalent calls found in
 * the linux kernel (win32 being the exception)
 *
 * This may change in the future (different compilers/architectures?).
 *
 * A note on win32: declaring a static variable inside an inline function
 * may produce some side effects. 
 */
static inline void vdscCompilerBarrier()
{ 
#if defined (WIN32)
#else
   barrier();
#endif
}

static inline void vdscMemoryBarrier()
{
#if defined (WIN32)
   static LONG dummy = 0;
   InterlockedIncrement( &dummy );
#else
   smp_mb();
#endif
}

static inline void vdscReadMemoryBarrier()
{
#if defined (WIN32)
   static LONG dummy = 0;
   InterlockedIncrement( &dummy );
#else
   smp_rmb();
#endif
}

static inline void vdscWriteMemoryBarrier()
{
#if defined (WIN32)
   static LONG dummy = 0;
   InterlockedIncrement( &dummy );
#else
   smp_wmb();
#endif
}

#endif /* VDSC_BARRIER_H */
