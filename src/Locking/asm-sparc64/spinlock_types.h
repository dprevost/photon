#ifndef __SPARC64_SPINLOCK_TYPES_H
#define __SPARC64_SPINLOCK_TYPES_H

#ifndef __LINUX_SPINLOCK_TYPES_H
# error "please don't include this file directly"
#endif

typedef struct {
	volatile unsigned char lock;
} raw_spinlock_t;

#define __RAW_SPIN_LOCK_UNLOCKED	{ 0 }

#endif
