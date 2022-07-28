#include <unistd.h>

#error "Userspace locks are not implemented yet"

#ifndef __kernel__

#define PAGE_SIZE 4096

int liballoc_lock(void)
{
    return 0;
}

int liballoc_unlock(void)
{
    return 0;
}

void *liballoc_alloc(int pages)
{
    return (void *) syscall(SYS_ALLOC, pages * PAGE_SIZE);
}

int liballoc_free(void *ptr, int pages)
{
    syscall(SYS_FREE, (uint64_t) ptr, pages * PAGE_SIZE);
    return 0;
}

#endif /* !__kernel__ */
