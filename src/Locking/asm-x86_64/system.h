#ifndef __ASM_SYSTEM_H
#define __ASM_SYSTEM_H

#ifdef CONFIG_SMP
#define smp_mb()	mb()
#define smp_rmb()	rmb()
#define smp_wmb()	wmb()
#define smp_read_barrier_depends()	do {} while(0)
#else
#define smp_mb()	barrier()
#define smp_rmb()	barrier()
#define smp_wmb()	barrier()
#define smp_read_barrier_depends()	do {} while(0)
#endif

/*
 * Force strict CPU ordering.
 * And yes, this is required on UP too when we're talking
 * to devices.
 */
#define mb() 	asm volatile("mfence":::"memory")
#define rmb()	asm volatile("lfence":::"memory")

#ifdef CONFIG_UNORDERED_IO
#  define wmb()	asm volatile("sfence" ::: "memory")
#else
#  define wmb()	asm volatile("" ::: "memory")
#endif

#define read_barrier_depends()	do {} while(0)

#endif /* __ASM_SYSTEM_H */

