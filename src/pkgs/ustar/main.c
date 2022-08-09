#include "unistd.h"
#include <idl/inc/vfs.h>
#include <stdlib.h>

static int ustar_open(char *path, int flags)
{
    return 0;
}

static int ustar_read(int fd, void *buf, size_t count)
{
    return 0;
}

__attribute__((noreturn)) int _start(void)
{
    if (vfs_register_filesystem("ustar") != 0)
    {
        // TODO: debug: couldn't register ustar filesystem
        for (;;);
    }

    ipc_t *ipc = ipc_receive_sync();

    for (;;);
    
    __builtin_unreachable();
}
