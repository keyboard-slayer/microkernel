#ifndef KERNEL_INC_ARCH_H
#define KERNEL_INC_ARCH_H

#ifdef __x86_64__
#include <x86_64/inc/regs.h>
#define PAGE_SIZE 4096
#define STACK_SIZE (0x4000)
#endif /* !__x86_64__ */

#define USER_STACK_BASE (0xc0000000)

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <vec.h>

typedef struct 
{
    uintptr_t physical;
    uintptr_t virtual;
    uintptr_t length;
} virtual_physical_map_t;

typedef struct 
{
    uintptr_t syscall_kernel_stack;
    uintptr_t syscall_user_stack;
    uintptr_t syscall_kernel_bstack;

    regs_t regs;
} context_t;

void arch_init(void);

void cpu_enable_int(void);
size_t cpu_get_count(void);

void *vmm_create_space(void);
void vmm_map(void *pml, virtual_physical_map_t map, bool user);
void vmm_switch_space(void *pml);
bool vmm_is_mapped(void *pml, uint64_t virt);
void *vmm_get_kernel_pml(void);
void *vmm_get_current_pml(void);
void vmm_unmap(void *space, uint64_t vaddr);

context_t context_create(uintptr_t ip);
void context_switch(context_t *ctx, regs_t *regs);
void context_save(context_t *ctx, regs_t *regs);

#endif /* !KERNEL_INC_ARCH_H */
