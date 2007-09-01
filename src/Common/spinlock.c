/*
 * Copyright (2004) Linus Torvalds
 *
 * Author: Zwane Mwaikambo <zwane@fsmlabs.com>
 *
 * Copyright (2004, 2005) Ingo Molnar
 *
 * This file contains the spinlock/rwlock implementations for the
 * SMP and the DEBUG_SPINLOCK cases. (UP-nondebug inlines them)
 *
 * Note that some architectures have special knowledge about the
 * stack frames of these functions in their profile_pc. If you
 * change anything significant here that could change the stack
 * frame contact the architecture maintainers.
 */

#include <mylinux/linkage.h>
#include <mylinux/preempt.h>
#include <mylinux/spinlock.h>

int __lockfunc _spin_trylock(spinlock_t *lock)
{
	preempt_disable();
	if (_raw_spin_trylock(lock)) {
		spin_acquire(&lock->dep_map, 0, 1, _RET_IP_);
		return 1;
	}
	
	preempt_enable();
	return 0;
}

/*
 * If lockdep is enabled then we use the non-preemption spin-ops
 * even on CONFIG_PREEMPT, because lockdep assumes that interrupts are
 * not re-enabled during lock-acquire (which the preempt-spin-ops do):
 */
#if !defined(CONFIG_PREEMPT) || !defined(CONFIG_SMP) || \
	defined(CONFIG_DEBUG_LOCK_ALLOC)

void __lockfunc _spin_lock(spinlock_t *lock)
{
	preempt_disable();
	spin_acquire(&lock->dep_map, 0, 0, _RET_IP_);
	_raw_spin_lock(lock);
}

#else /* CONFIG_PREEMPT: */

/*
 * This could be a long-held lock. We both prepare to spin for a long
 * time (making _this_ CPU preemptable if possible), and we also signal
 * towards that other CPU that it should break the lock ASAP.
 *
 * (We do this in a function because inlining it would be excessive.)
 */

#define BUILD_LOCK_OPS(op, locktype)					\
void __lockfunc _##op##_lock(locktype##_t *lock)			\
{									\
	for (;;) {							\
		preempt_disable();					\
		if (likely(_raw_##op##_trylock(lock)))			\
			break;						\
		preempt_enable();					\
									\
		if (!(lock)->break_lock)				\
			(lock)->break_lock = 1;				\
		while (!op##_can_lock(lock) && (lock)->break_lock)	\
			_raw_##op##_relax(&lock->raw_lock);		\
	}								\
	(lock)->break_lock = 0;						\
}									

/*
 * Build preemption-friendly versions of the following
 * lock-spinning functions:
 *
 *         _[spin|read|write]_lock()
 */
BUILD_LOCK_OPS(spin, spinlock);

#endif /* CONFIG_PREEMPT */

#ifdef CONFIG_DEBUG_LOCK_ALLOC

void __lockfunc _spin_lock_nested(spinlock_t *lock, int subclass)
{
	preempt_disable();
	spin_acquire(&lock->dep_map, subclass, 0, _RET_IP_);
	_raw_spin_lock(lock);
}

#endif

void __lockfunc _spin_unlock(spinlock_t *lock)
{
	spin_release(&lock->dep_map, 1, _RET_IP_);
	_raw_spin_unlock(lock);
	preempt_enable();
}

