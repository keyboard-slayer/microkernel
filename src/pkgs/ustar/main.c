#include <idl/inc/vfs.h>

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
    vfs_register_filesystem((filesystem_t) {
        .name = "ustar",
        .open = ustar_open,
        .read = ustar_read,
    });

    for (;;);

    __builtin_unreachable();
}
