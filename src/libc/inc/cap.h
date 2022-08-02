#ifndef LIBC_INC_CAP_H
#define LIBC_INC_CAP_H

#include <stdint.h>

typedef struct 
{
    uintptr_t addr;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint64_t bpp;
} fb_t;

#endif /* !LIBC_INC_CAP_H */
