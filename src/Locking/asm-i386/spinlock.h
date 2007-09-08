#ifndef __ASM_SPINLOCK_H
#define __ASM_SPINLOCK_H

#include "Locking/asm/alternative.h"

/*
 * Your basic SMP spinlocks, allowing only a single CPU anywhere
 *
 * Simple spin lock operations.  There are two variants, one clears IRQ's
 * on the local processor, one does not.
 *
 * We make no fairness assumptions. They have a cost.
 *
 * (the type definitions are in asm/spinlock_types.h)
 */

static inline int __raw_spin_is_locked(raw_spinlock_t *x)
{
	return *(volatile signed char *)(&(x)->slock) <= 0;
}

static inline void __raw_spin_lock(raw_spinlock_t *lock)
{
	asm volatile("\n1:\t"
		     LOCK_PREFIX " ; decb %0\n\t"
		     "jns 3f\n"
		     "2:\t"
		     "rep;nop\n\t"
		     "cmpb $0,%0\n\t"
		     "jle 2b\n\t"
		     "jmp 1b\n"
		     "3:\n\t"
		     : "+m" (lock->slock) : : "memory");
}

static inline int __raw_spin_trylock(raw_spinlock_t *lock)
{
	char oldval;
	asm volatile(
		"xchgb %b0,%1"
		:"=q" (oldval), "+m" (lock->slock)
		:"0" (0) : "memory");
	return oldval > 0;
}

/*
 * __raw_spin_unlock based on writing $1 to the low byte.
 * This method works. Despite all the confusion.
 * (except on PPro SMP or if we are using OOSTORE, so we use xchgb there)
 * (PPro errata 66, 92)
 */

#if !defined(CONFIG_X86_OOSTORE) && !defined(CONFIG_X86_PPRO_FENCE)

static inline void __raw_spin_unlock(raw_spinlock_t *lock)
{
	asm volatile("movb $1,%0" : "+m" (lock->slock) :: "memory");
}

#else

static inline void __raw_spin_unlock(raw_spinlock_t *lock)
{
	char oldval = 1;

	asm volatile("xchgb %b0, %1"
		     : "=q" (oldval), "+m" (lock->slock)
		     : "0" (oldval) : "memory");
}

#endif

#endif /* __ASM_SPINLOCK_H */

