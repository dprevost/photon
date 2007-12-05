#ifndef __ASM_SPINLOCK_H
#define __ASM_SPINLOCK_H

#ifdef CONFIG_SMP
#define LOCK_PREFIX \
		".section .smp_locks,\"a\"\n"	\
		"  .align 8\n"			\
		"  .quad 661f\n" /* address */	\
		".previous\n"			\
	       	"661:\n\tlock; "

#else /* ! CONFIG_SMP */
#define LOCK_PREFIX ""
#endif

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

static inline int __raw_spin_is_locked(raw_spinlock_t *lock)
{
	return *(volatile signed int *)(&(lock)->slock) <= 0;
}

static inline void __raw_spin_lock(raw_spinlock_t *lock)
{
	asm volatile(
		"\n1:\t"
		LOCK_PREFIX " ; decl %0\n\t"
		"jns 2f\n"
		"3:\n"
		"rep;nop\n\t"
		"cmpl $0,%0\n\t"
		"jle 3b\n\t"
		"jmp 1b\n"
		"2:\t" : "=m" (lock->slock) : : "memory");
}

static inline int __raw_spin_trylock(raw_spinlock_t *lock)
{
	int oldval;

	asm volatile(
		"xchgl %0,%1"
		:"=q" (oldval), "=m" (lock->slock)
		:"0" (0) : "memory");

	return oldval > 0;
}

static inline void __raw_spin_unlock(raw_spinlock_t *lock)
{
	asm volatile("movl $1,%0" :"=m" (lock->slock) :: "memory");
}

#endif /* __ASM_SPINLOCK_H */

