#ifndef KERNEL_INC_LOADER_H
#define KERNEL_INC_LOADER_H

#include <stdint.h>
#include <stddef.h>
#include <cap.h>

#include "utils.h"

#define LIMIT_ENTRIES 64

#define FOREACH_MEMMAP_TYPE(FUNC)         \
    FUNC(MEMMAP_USABLE)                   \
    FUNC(MEMMAP_RESERVED)                 \
    FUNC(MEMMAP_RECLAIMABLE)              \
    FUNC(MEMMAP_BAD_MEMORY)               \
    FUNC(MEMMAP_BOOTLOADER_RECLAIMABLE)   \
    FUNC(MEMMAP_KERNEL_AND_MODULES)       \
    FUNC(MEMMAP_FRAMEBUFFER)

typedef enum
{
    FOREACH_MEMMAP_TYPE(GENERATE_ENUM)
} memmap_type_t;

__attribute__((used)) static char const *mmap_type_str[] = 
{
    FOREACH_MEMMAP_TYPE(GENERATE_STRING)
};

typedef struct 
{
    uintptr_t base;
    size_t length;
    memmap_type_t type;
} memmap_t;

typedef struct
{
    memmap_t entries[LIMIT_ENTRIES];
    size_t length;
} memmaps_t;

typedef struct 
{
    void *ptr;
    size_t length;
} module_t;


memmaps_t *loader_get_memmap(void);
uint64_t loader_get_hhdm(void);
uint64_t loader_get_pbase(void);
uint64_t loader_get_vbase(void);
void *loader_get_rsdp(void);
module_t loader_get_module(char const *name);
void loader_boot_other_cpus(void);
fb_t loader_get_fb(void);

#endif /* !KERNEL_INC_LOADER_H */
