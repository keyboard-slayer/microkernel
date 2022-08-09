#ifndef LIBC_INC_VEC_H
#define LIBC_INC_VEC_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void vec_expand_(char **data, size_t *length, size_t *capacity, int memsz);

#define vec(T)                                                                 \
    struct                                                                     \
    {                                                                          \
        T *data;                                                               \
        size_t length;                                                         \
        size_t capacity;                                                       \
    }

#define vec_init(v) memset((v), 0, sizeof(*(v)))

#define vec_clear(v) ((v)->length = 0)

#define vec_push(v, val)                                                       \
    vec_expand_((char **) &(v)->data, &(v)->length, &(v)->capacity,            \
                sizeof(*(v)->data));                                           \
    (v)->data[(v)->length++] = (val)

#define vec_foreach(v, t, i)                                                    \
    if ((v)->length > 0)                                                        \
        for ((i) = 0;                                                           \
             (i) < (v)->length && (((t) = (v)->data[(i)]), 1);                  \
             ++(i))

#define vec_free(v) free((v)->data)
#define vec_pop(v) \
    (v)->data[--((v)->length)]

typedef vec(char) vec_char_t;
typedef vec(char *) vec_str_t;

vec_str_t vec_split(char *str, char *delim);

#endif /* !LIBC_INC_VEC_H */
