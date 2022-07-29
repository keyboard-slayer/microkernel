#include <stdlib.h>
#include <string.h>
#include <unistd.h>

long atol(const char *nptr)
{
    long ret = 0;
    int sign = 1;

    if (*nptr == '-')
    {
        sign = -1;
        nptr++;
    }

    while (*nptr)
    {
        ret = ret * 10 + (*nptr - '0');
        nptr++;
    }

    return ret * sign;
}

#ifndef __kernel__

void *malloc(size_t size)
{
    return (void *) syscall(SYS_ALLOC, size);
}

void free(void *ptr)
{
    syscall(SYS_FREE, (uintptr_t) ptr);
}

void *realloc(void *ptr, size_t size)
{
    return (void *) syscall(SYS_REALLOC, (uintptr_t) ptr, size);
}

void *calloc(size_t nmemb, size_t size)
{
    void *ptr = malloc(nmemb * size);
    memset(ptr, 0, nmemb * size);
    return ptr;
}

#endif /* !__kernel__ */
