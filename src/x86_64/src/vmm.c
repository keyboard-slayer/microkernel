#include <kernel/inc/loader.h>
#include <kernel/inc/logging.h>
#include <kernel/inc/klock.h>
#include <kernel/inc/pmm.h>
#include <kernel/inc/arch.h>

#include <libc/inc/string.h>

#include "../inc/vmm.h"

DECLARE_LOCK(vmm);
static pml_t *kernel_pml4;

static uint64_t vmm_get_pml_alloc(pml_t *pml, size_t index, bool user, bool alloc)
{
    pml_entry_t entry = pml->entries[index];

    if (entry.present)   
    {
        return (entry.physical << 12) + loader_get_hhdm();
    }
    else if (alloc)  
    {
        uint64_t new_entry = ((uint64_t) pmm_alloc(PAGE_SIZE));

        if (new_entry == 0)
        {
            klog(ERROR, "Failed to allocate new PML entry");
            halt();
        }

        memset((void *) (new_entry + loader_get_hhdm()), 0, PAGE_SIZE);
        pml->entries[index] = pml_make_entry(new_entry, user);

        return new_entry + loader_get_hhdm();
    }

    return 0;
}

static void vmm_map_page(pml_t *pml, uint64_t virt, uint64_t phys, bool user)
{
    LOCK(vmm);

    pml_t *last_entry = pml;

    for (size_t i = 3; i > 0; i--)
    {
        last_entry = (pml_t *) vmm_get_pml_alloc(last_entry, PMLX_GET_INDEX(virt, i), true, true);
    }

    last_entry->entries[PMLX_GET_INDEX(virt, 0)] = pml_make_entry(phys, user);

    UNLOCK(vmm);
}

void vmm_switch_space(void *space)
{
    LOCK(vmm);
    __asm__ volatile ("mov %0, %%cr3" :: "r"(((uintptr_t) space) - loader_get_hhdm()));
    UNLOCK(vmm);
}

void vmm_map(void *pmlptr, virtual_physical_map_t map, bool user)
{
    pml_t *pml = pmlptr;

    if (map.physical % PAGE_SIZE != 0 || map.virtual % PAGE_SIZE != 0 || map.length % PAGE_SIZE != 0)
    {
        klog(ERROR, "Can't map a non-page-aligned region");
        halt();
    }

    for (size_t i = 0; i < map.length / PAGE_SIZE; i++)
    {
        size_t physaddr = i * PAGE_SIZE + map.physical;
        size_t virtaddr = i * PAGE_SIZE + map.virtual;

        vmm_map_page(pml, virtaddr, physaddr, user);
    }
}

void vmm_init(void)
{
    klog(INFO, "Initializing VMM.");

    kernel_pml4 = (pml_t *) ((uintptr_t) pmm_alloc(PAGE_SIZE) + loader_get_hhdm());
    klog(INFO, "Allocated kernel PML at %p", kernel_pml4);

    if (kernel_pml4 == NULL)
    {
        klog(ERROR, "Failed to allocate kernel PML4");
        halt();
    }

    memset(kernel_pml4, 0, PAGE_SIZE);

    klog(INFO, "Mapping %p to %p", 0, loader_get_hhdm());
    vmm_map(kernel_pml4, (virtual_physical_map_t) {
        .physical = 0,
        .virtual = loader_get_hhdm(),
        .length = FOUR_GIG
    }, false);

    klog(INFO, "Mapping %p to %p", loader_get_pbase(), loader_get_vbase());
    vmm_map(kernel_pml4, (virtual_physical_map_t) {
        .physical = loader_get_pbase(),
        .virtual = loader_get_vbase(),
        .length = FOUR_GIG
    }, false);

    memmaps_t *memmaps = loader_get_memmap();

    for (size_t i = 0; i < memmaps->length; i++)
    {
        vmm_map(kernel_pml4, (virtual_physical_map_t) {
            .physical = memmaps->entries[i].base,
            .virtual = memmaps->entries[i].base + loader_get_hhdm(),
            .length = memmaps->entries[i].length
        }, 0);
    }

    klog(INFO, "Switing to kernel PML4");
    vmm_switch_space(kernel_pml4);
    klog(OK, "VMM initialized.");
    return;
}

void *vmm_create_space(void)
{
    pml_t *space = (pml_t *) ((uintptr_t) pmm_alloc(PAGE_SIZE) + loader_get_hhdm());
    memset(space, 0, PAGE_SIZE);

    for (size_t i = 255; i < 512; i++)
    {
        space->entries[i] = kernel_pml4->entries[i];
    }

    return (void *) space;
}

void *vmm_get_kernel_pml(void)
{
    return (void *) kernel_pml4;
}

void *vmm_get_current_pml(void)
{
    uint64_t cr3;

    LOCK(vmm);
    __asm__ volatile ("mov %%cr3, %0":"=r" (cr3));
    UNLOCK(vmm);

    return (void *) (cr3 + loader_get_hhdm());
}

bool vmm_is_mapped(void *pml, uint64_t virt)
{
    pml_t *last_entry = pml;

    for (size_t i = 3; i > 0; i--)
    {
        last_entry = (pml_t *) vmm_get_pml_alloc(last_entry, PMLX_GET_INDEX(virt, i), true, false);
        if (last_entry == NULL)
        {
            return false;
        }
    }

    return last_entry->entries[PMLX_GET_INDEX(virt, 0)].present;
}

void vmm_unmap(void *pml, uintptr_t vaddr)
{
    LOCK(vmm);
    pml_t *last_entry = pml;

    for (size_t i = 3; i > 0; i--)
    {
        last_entry = (pml_t *) vmm_get_pml_alloc(last_entry, PMLX_GET_INDEX(vaddr, i), true, false);
    }

    last_entry->entries[PMLX_GET_INDEX(vaddr, 0)] = (pml_entry_t) {0};

    UNLOCK(vmm);
}
