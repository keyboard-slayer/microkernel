#include <stdio.h>
#include <stdarg.h>
#include <stb_sprintf.h>

int sprintf(char *restrict str, const char *restrict format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vsprintf(str, format, args);
    va_end(args);
    return ret;
}

int vsprintf(char *restrict str, const char *restrict format, va_list args)
{
    return stbsp_vsprintf(str, format, args);
}
