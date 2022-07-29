#ifndef KERNEL_INC_TASK_H
#define KERNEL_INC_TASK_H

#include <libc/inc/vec.h>

#include <sys/types.h>

#include "arch.h"
#include "ipc.h"

#define UNIX_PATH_LIMIT 4096

enum task_state
{
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_ZOMBIE,
    TASK_DEAD,
};

typedef struct 
{
    pid_t pid;
    char path[UNIX_PATH_LIMIT];
    void *space;
    uint8_t *stack;
    uint64_t ident;
    context_t context;
    enum task_state state;
    ipc_mailbox_t mailbox;
} task_t;

typedef vec(task_t *) vec_task_t;
task_t *task_create(void *space, char const *name, uintptr_t ip);

#endif /* !KERNEL_INC_TASK_H */
