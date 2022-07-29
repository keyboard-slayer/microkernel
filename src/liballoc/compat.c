#include "kernel/inc/logging.h"
#include <stdint.h>

#include <kernel/inc/arch.h>
#include <kernel/inc/klock.h>
#include <kernel/inc/pmm.h>
#include <kernel/inc/loader.h>
#include <kernel/inc/sched.h>

DECLARE_LOCK(liballoc);

int liballoc_lock(void)
{
    LOCK(liballoc);
    return 0;
}

int liballoc_unlock(void)
{
    UNLOCK(liballoc);
    return 0;
}

void *liballoc_alloc(int pages)
{
    void *space;
    void *buf = pmm_alloc(pages * PAGE_SIZE);
    uintptr_t vaddr = (uintptr_t) buf + loader_get_hhdm();

    if (sched_is_running())
    {
        space = sched_current()->space;
        // vaddr = space == vmm_get_kernel_pml() ? (uintptr_t) buf + loader_get_hhdm() : (uintptr_t) buf;
    }
    else  
    {
        space = vmm_get_kernel_pml();
    }

    if (buf == NULL)
    {
        return NULL;
    }

    vmm_map(space, (virtual_physical_map_t) {
        .physical = (uintptr_t) buf,
        .virtual = vaddr,
        .length = pages * PAGE_SIZE
    }, true);

    return (void *) vaddr;
}

int liballoc_free(void* ptr, int pages)
{
    void *space;
    uintptr_t physaddr = (uintptr_t) ptr - loader_get_hhdm();

    if (sched_is_running())
    {
        space = sched_current()->space;
    }
    else  
    {
        space = vmm_get_kernel_pml();
    }

    pmm_free(physaddr, pages * PAGE_SIZE);
    vmm_unmap(space, (uint64_t) ptr);
    return 0;
}
