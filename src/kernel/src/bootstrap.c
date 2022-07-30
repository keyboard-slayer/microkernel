#include "../inc/arch.h"
#include "../inc/loader.h"
#include "../inc/pmm.h"
#include "../inc/logging.h"
#include "../inc/elf.h"
#include "../inc/sched.h"

#include <assert.h>

int _start(void)
{
    arch_init();
    sched_init();

    void *executable = loader_get_module("/bin/echo.elf");
    assert(executable != NULL);

    task_t *client = loader_binary(executable, "/bin/echo.elf");
    task_t *server = loader_binary(executable, "/bin/echo.elf");

    sched_push(client);
    sched_push(server);

    for (;;);

    __builtin_unreachable();
}
