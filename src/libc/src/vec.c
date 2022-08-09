#include <assert.h>
#include <stdlib.h>

#include "../inc/vec.h"

void vec_expand_(char **data, size_t *length, size_t *capacity, int memsz)
{
    if (*length + 1 > *capacity)
    {
        void *ptr;
        size_t n = (*capacity == 0) ? 1 : *capacity << 1;
        ptr = realloc(*data, n * memsz);

        assert(ptr != NULL);

        *data = ptr;
        *capacity = n;
    }
}

vec_str_t vec_split(char *str, char *delim)
{
    vec_str_t vec;
    vec_init(&vec);

    char *p = strtok(str, delim);

    while (p)
    {
        vec_push(&vec, p);
        p = strtok(NULL, delim);
    }

    return vec;
}
