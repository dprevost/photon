/*
 * Copyright (2004) Linus Torvalds
 *
 * Author: Zwane Mwaikambo <zwane@fsmlabs.com>
 *
 * Copyright (2004, 2005) Ingo Molnar
 *
 * This file contains the spinlock implementations for the
 * SMP and the DEBUG_SPINLOCK cases.
 *
 * Note that some architectures have special knowledge about the
 * stack frames of these functions in their profile_pc. If you
 * change anything significant here that could change the stack
 * frame contact the architecture maintainers.
 */

#include "Locking/linux/linkage.h"
#include "Locking/linux/spinlock.h"

int __lockfunc _spin_trylock(spinlock_t *lock)
{
   if (_raw_spin_trylock(lock))
      return 1;
	return 0;
}

void __lockfunc _spin_lock(spinlock_t *lock)
{
   _raw_spin_lock(lock);
}

void __lockfunc _spin_unlock(spinlock_t *lock)
{
   _raw_spin_unlock(lock);
}

