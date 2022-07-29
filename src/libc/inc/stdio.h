#ifndef LIBC_INC_STDIO_H
#define LIBC_INC_STDIO_H

#include <stdarg.h>

int sprintf(char *restrict str, const char *restrict format, ...);
int vsprintf(char *restrict str, const char *restrict format, va_list ap);
int printf(const char *format, ...);

#endif /* !LIBC_INC_STDIO_H */
