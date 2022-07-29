#ifndef ARCH_X86_64_INC_VMM_H
#define ARCH_X86_64_INC_VMM_H

#include <stdint.h>
#include <stdbool.h>

#define FOUR_GIG 0x100000000

#define PMLX_GET_INDEX(addr, level)                                                                \
    (((uint64_t) addr & ((uint64_t) 0x1ff << (12 + level * 9))) >> (12 + level * 9))

typedef struct
{
    union 
    {
        uint64_t raw;

        struct
        {
            bool present : 1;
            bool read_write : 1;
            bool user : 1;
            bool caching : 1;
            bool caching_disable : 1;
            bool accessed : 1;
            bool dirty : 1;
            bool huge_page : 1;
            bool global_page : 1;
            uint8_t _available : 3;
            uint64_t physical : 52;
        } __attribute__((packed));
    };
} __attribute__((packed)) pml_entry_t;

static inline pml_entry_t pml_make_entry(uintptr_t physical, bool is_user)
{
    return (pml_entry_t){
        .physical = physical >> 12,
        .user = is_user,
        .read_write = true,
        .present = true,
        .caching = false,
        .caching_disable = false,
        .accessed = false,
        .dirty = false,
        .huge_page = false,
        .global_page = false,
        ._available = 0,
    };
}

typedef struct
{
    pml_entry_t entries[512];
} __attribute__((packed)) pml_t;

void vmm_init(void);
void vmm_switch_space(void *space);
void vmm_unmap(void *pml, uintptr_t vaddr);

#endif /* !ARCH_X86_64_INC_VMM_H */
