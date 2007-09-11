/* spinlock.h: 32-bit Sparc spinlock support.
 *
 * Copyright (C) 1997 David S. Miller (davem@caip.rutgers.edu)
 */

#ifndef __SPARC_SPINLOCK_H
#define __SPARC_SPINLOCK_H

#define __raw_spin_is_locked(lock) (*((volatile unsigned char *)(lock)) != 0)

static inline void __raw_spin_lock(raw_spinlock_t *lock)
{
	__asm__ __volatile__(
	"\n1:\n\t"
	"ldstub	[%0], %%g2\n\t"
	"orcc	%%g2, 0x0, %%g0\n\t"
	"bne,a	2f\n\t"
	" ldub	[%0], %%g2\n\t"
	".subsection	2\n"
	"2:\n\t"
	"orcc	%%g2, 0x0, %%g0\n\t"
	"bne,a	2b\n\t"
	" ldub	[%0], %%g2\n\t"
	"b,a	1b\n\t"
	".previous\n"
	: /* no outputs */
	: "r" (lock)
	: "g2", "memory", "cc");
}

static inline int __raw_spin_trylock(raw_spinlock_t *lock)
{
	unsigned int result;
	__asm__ __volatile__("ldstub [%1], %0"
			     : "=r" (result)
			     : "r" (lock)
			     : "memory");
	return (result == 0);
}

static inline void __raw_spin_unlock(raw_spinlock_t *lock)
{
	__asm__ __volatile__("stb %%g0, [%0]" : : "r" (lock) : "memory");
}

#endif /* __SPARC_SPINLOCK_H */

