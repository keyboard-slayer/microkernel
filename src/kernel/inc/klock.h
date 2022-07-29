#ifndef KERNEL_INC_LOCK_H
#define KERNEL_INC_LOCK_H

#define DECLARE_LOCK(name) volatile int name ## Locked
#define LOCK(name) \
	__asm__ volatile("cli"); \
	while (!__sync_bool_compare_and_swap(& name ## Locked, 0, 1)); \
	__sync_synchronize();
#define UNLOCK(name) \
	__asm__ volatile("sti"); \
	__sync_synchronize(); \
	name ## Locked = 0;

#endif /* !KERNEL_INC_LOCK_H */
