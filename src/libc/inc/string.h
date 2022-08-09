#ifndef LIBC_INC_STRING_H
#define LIBC_INC_STRING_H

#include <stddef.h>
#include <stdint.h>


void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
size_t strlen(const char *s);
int memcmp(const void *s1, const void *s2, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
char *strrchr(const char *s, int c);
char *strdup(const char *s);
char *strndup(const char *s, size_t n);
int strcmp(const char *s1, const char *s2);
char *strtok(char *restrict str, const char *restrict delim);

#endif /* !LIBC_INC_STRING_H */
