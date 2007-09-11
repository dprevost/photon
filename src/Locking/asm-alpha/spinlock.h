#ifndef _ALPHA_SPINLOCK_H
#define _ALPHA_SPINLOCK_H

/* For the definition of test_and_set_bit() */
#include <asm/bitops.h>

/* For mb(); */
#include <asm/barrier.h> 

/*
 * Simple spin lock operations.  There are two variants, one clears IRQ's
 * on the local processor, one does not.
 *
 * We make no fairness assumptions. They have a cost.
 */

#define __raw_spin_is_locked(x)	((x)->lock != 0)

static inline void __raw_spin_unlock(raw_spinlock_t * lock)
{
	mb();
	lock->lock = 0;
}

static inline void __raw_spin_lock(raw_spinlock_t * lock)
{
	long tmp;

	__asm__ __volatile__(
	"1:	ldl_l	%0,%1\n"
	"	bne	%0,2f\n"
	"	lda	%0,1\n"
	"	stl_c	%0,%1\n"
	"	beq	%0,2f\n"
	"	mb\n"
	".subsection 2\n"
	"2:	ldl	%0,%1\n"
	"	bne	%0,2b\n"
	"	br	1b\n"
	".previous"
	: "=&r" (tmp), "=m" (lock->lock)
	: "m"(lock->lock) : "memory");
}

static inline int __raw_spin_trylock(raw_spinlock_t *lock)
{
	return !test_and_set_bit(0, &lock->lock);
}

#endif /* _ALPHA_SPINLOCK_H */

