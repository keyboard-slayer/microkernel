#include <stdint.h>

#ifdef __kernel__
#include <kernel/inc/arch.h>
#include <kernel/inc/klock.h>
#include <kernel/inc/pmm.h>
#include <kernel/inc/loader.h>

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
    void *space = vmm_get_kernel_pml();
    void *buf = pmm_alloc(pages * PAGE_SIZE);

    if (buf == NULL)
    {
        return NULL;
    }

    vmm_map(space, (virtual_physical_map_t) {
        .physical = (uintptr_t) buf,
        .virtual = (uintptr_t) buf + loader_get_hhdm(),
        .length = pages * PAGE_SIZE
    }, true);

    return (void *) (((uintptr_t) buf) + loader_get_hhdm());
}

int liballoc_free(void* ptr, int pages)
{
    void *space = vmm_get_kernel_pml();
    pmm_free((uint64_t) ptr - loader_get_hhdm(), pages * PAGE_SIZE);
    vmm_unmap(space, (uint64_t) ptr);
    return 0;
}

#endif  /* !__kernel__ */
