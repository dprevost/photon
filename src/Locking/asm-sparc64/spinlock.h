/* spinlock.h: 64-bit Sparc spinlock support.
 *
 * Copyright (C) 1997 David S. Miller (davem@caip.rutgers.edu)
 */

#ifndef __SPARC64_SPINLOCK_H
#define __SPARC64_SPINLOCK_H

/* To get debugging spinlocks which detect and catch
 * deadlock situations, set CONFIG_DEBUG_SPINLOCK
 * and rebuild your kernel.
 */

/* All of these locking primitives are expected to work properly
 * even in an RMO memory model, which currently is what the kernel
 * runs in.
 *
 * There is another issue.  Because we play games to save cycles
 * in the non-contention case, we need to be extra careful about
 * branch targets into the "spinning" code.  They live in their
 * own section, but the newer V9 branches have a shorter range
 * than the traditional 32-bit sparc branch variants.  The rule
 * is that the branches that go into and out of the spinner sections
 * must be pre-V9 branches.
 */

#define __raw_spin_is_locked(lp)	((lp)->lock != 0)

static inline void __raw_spin_lock(raw_spinlock_t *lock)
{
	unsigned long tmp;

	__asm__ __volatile__(
"1:	ldstub		[%1], %0\n"
"	membar		#StoreLoad | #StoreStore\n"
"	brnz,pn		%0, 2f\n"
"	 nop\n"
"	.subsection	2\n"
"2:	ldub		[%1], %0\n"
"	membar		#LoadLoad\n"
"	brnz,pt		%0, 2b\n"
"	 nop\n"
"	ba,a,pt		%%xcc, 1b\n"
"	.previous"
	: "=&r" (tmp)
	: "r" (lock)
	: "memory");
}

static inline int __raw_spin_trylock(raw_spinlock_t *lock)
{
	unsigned long result;

	__asm__ __volatile__(
"	ldstub		[%1], %0\n"
"	membar		#StoreLoad | #StoreStore"
	: "=r" (result)
	: "r" (lock)
	: "memory");

	return (result == 0UL);
}

static inline void __raw_spin_unlock(raw_spinlock_t *lock)
{
	__asm__ __volatile__(
"	membar		#StoreStore | #LoadStore\n"
"	stb		%%g0, [%0]"
	: /* No outputs */
	: "r" (lock)
	: "memory");
}

#endif /* !(__SPARC64_SPINLOCK_H) */

