#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

void puts(char const *s)
{
    syslog(s);
}

int printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    char buf[512] = {0};
    vsprintf(buf, format, args);
    puts(buf);

    va_end(args);

    return 0;
}

__attribute__((noreturn)) void abort(void)
{
    printf("Abort\n");
    for (;;);
}
