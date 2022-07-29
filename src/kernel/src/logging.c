#include "../inc/com.h"
#include "../inc/logging.h"
#include "../inc/klock.h"

#include <stdio.h>

#ifdef __x86_64__
#include <x86_64/inc/madt.h>
#define cpu_id lapic_current_cpu
#endif /* !__x86_64__ */

DECLARE_LOCK(logging);

static const char *log[] = {
    "", "[ \033[35mOK    \033[39m] ", "[ \033[34mINFO  \033[39m] ", "[ \033[31mERROR \033[39m] "
};

void klog_impl(log_level_t level, char const *filename, size_t lineno, char const *format, ...)
{
    LOCK(logging);
    va_list args;
    char buf[512] = {0};

    if (level != NONE)
    {
        sprintf(buf, "%s\033[33mcore %d %s:%ld\033[0m ", log[level], cpu_id(), filename, lineno);
    }

    com_puts(buf);

    va_start(args, format);
    vsprintf(buf, format, args);
    com_puts(buf);
    com_putc('\n');
    va_end(args);
    UNLOCK(logging);
}

void klog_lock(void)
{
    LOCK(logging);
}

void klog_unlock(void)
{
    UNLOCK(logging);
}
