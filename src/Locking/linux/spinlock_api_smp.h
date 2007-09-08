#ifndef __LINUX_SPINLOCK_API_SMP_H
#define __LINUX_SPINLOCK_API_SMP_H

#ifndef __LINUX_SPINLOCK_H
# error "please don't include this file directly"
#endif

/*
 * Locking/linux/spinlock_api_smp.h
 *
 * spinlock API declarations on SMP (and debug)
 * (implemented in kernel/spinlock.c)
 *
 * portions Copyright 2005, Red Hat, Inc., Ingo Molnar
 * Released under the General Public License (GPL).
 */

void __lockfunc _spin_lock(    spinlock_t *lock );
int  __lockfunc _spin_trylock( spinlock_t *lock );
void __lockfunc _spin_unlock(  spinlock_t *lock );

#endif /* __LINUX_SPINLOCK_API_SMP_H */

