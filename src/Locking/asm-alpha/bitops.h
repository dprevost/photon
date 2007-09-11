#ifndef _ALPHA_BITOPS_H
#define _ALPHA_BITOPS_H

/*
 * Copyright 1994, Linus Torvalds.
 */

static inline int
test_and_set_bit(unsigned long nr, volatile void *addr)
{
	unsigned long oldbit;
	unsigned long temp;
	int *m = ((int *) addr) + (nr >> 5);

	__asm__ __volatile__(
	"1:	ldl_l %0,%4\n"
	"	and %0,%3,%2\n"
	"	bne %2,2f\n"
	"	xor %0,%3,%0\n"
	"	stl_c %0,%1\n"
	"	beq %0,3f\n"
	"2:\n"
#ifdef CONFIG_SMP
	"	mb\n"
#endif
	".subsection 2\n"
	"3:	br 1b\n"
	".previous"
	:"=&r" (temp), "=m" (*m), "=&r" (oldbit)
	:"Ir" (1UL << (nr & 31)), "m" (*m) : "memory");

	return oldbit != 0;
}

#endif /* _ALPHA_BITOPS_H */
