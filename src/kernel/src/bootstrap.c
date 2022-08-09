#include "../inc/arch.h"
#include "../inc/loader.h"
#include "../inc/pmm.h"
#include "../inc/logging.h"
#include "../inc/elf.h"
#include "../inc/sched.h"

#include <assert.h>

int _start(void)
{
    arch_init();
    sched_init();

    module_t vfs = loader_get_module("/boot/mod/vfs.elf");
    module_t initfs = loader_get_module("/boot/initrd.img");
    module_t ustar = loader_get_module("/boot/mod/ustar.elf");

    assert(vfs.ptr != NULL);
    assert(initfs.ptr != NULL);
    assert(ustar.ptr != NULL);

    task_t *vfs_task = loader_binary(vfs.ptr, "/boot/mod/vfs.elf");
    task_t *ustar_task = loader_binary(ustar.ptr, "/boot/mod/ustar.elf");

    vmm_map(ustar_task->space, (virtual_physical_map_t) {
        .physical = (uintptr_t) initfs.ptr,
        .virtual = (uintptr_t) initfs.ptr,
        .length = vfs.length
    }, true);

    sched_push(vfs_task);
    sched_push(ustar_task);

    for (;;);

    __builtin_unreachable();
}
