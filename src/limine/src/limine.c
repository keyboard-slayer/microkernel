#include <kernel/inc/loader.h>
#include <kernel/inc/logging.h>

#include <libc/inc/string.h>

#include <x86_64/inc/cpu.h>
#include <x86_64/inc/madt.h>

#include <stddef.h>
#include <stdatomic.h>

#include "../inc/limine.h"

volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0,
};

volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0,
};

volatile struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0,
};

volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0,
};

volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0,
};

volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
};

static memmaps_t memmaps = {0};

memmaps_t *loader_get_memmap(void)
{
    if (memmap_request.response == NULL)
    {
        return NULL;
    }

    if (memmaps.length > 0)
    {
        return &memmaps;
    }

    memmaps.length = memmap_request.response->entry_count;

    for (size_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        memmap_t *entry = &memmaps.entries[i];
        entry->base = memmap_request.response->entries[i]->base;
        entry->length = memmap_request.response->entries[i]->length;
        entry->type = memmap_request.response->entries[i]->type;
    }

    return &memmaps;
}

uint64_t loader_get_hhdm(void)
{
    if (hhdm_request.response == NULL)
    {
        return 0;
    }

    return hhdm_request.response->offset;
}

uint64_t loader_get_pbase(void)
{
    if (kernel_address_request.response == NULL)
    {
        return 0;
    }

    return kernel_address_request.response->physical_base;
}

uint64_t loader_get_vbase(void)
{
    if (kernel_address_request.response == NULL)
    {
        return 0;
    }

    return kernel_address_request.response->virtual_base;
}

void *loader_get_rsdp(void)
{
    if (rsdp_request.response == NULL)
    {
        return NULL;
    }

    return rsdp_request.response->address;
}

void *loader_get_module(char const *name)
{
    if (module_request.response == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < module_request.response->module_count; i++)
    {
        if (memcmp(module_request.response->modules[i]->path, name, strlen(name)) == 0)
        {
            return module_request.response->modules[i]->address;
        }
    }

    return NULL;
}

void loader_boot_other_cpus(void)
{
    if (smp_request.response == NULL)
    {
        return;
    }

    init_cpus(smp_request.response->cpu_count);

    for (size_t i = 0; i < smp_request.response->cpu_count; i++)
    {
        if (smp_request.response->cpus[i]->processor_id != (uint32_t) lapic_current_cpu())
        {
            smp_request.response->cpus[i]->goto_address = (limine_goto_address) _start_cpus;
        }
    }

    while (cpu_get_ready() < cpu_get_count())
    {
        __asm__ volatile("pause");
    }

    klog(OK, "All CPUs booted");
}

fb_t loader_get_fb(void)
{
    if (framebuffer_request.response == NULL)
    {
        return (fb_t) {0};
    }

    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

    return (fb_t) {
        .width = fb->width,
        .height = fb->height,
        .pitch = fb->pitch,
        .bpp = fb->bpp,
        .addr = (uintptr_t) fb->address,
    };
}
