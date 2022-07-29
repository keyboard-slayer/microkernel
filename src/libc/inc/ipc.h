#ifndef LIBC_INC_IPC_H
#define LIBC_INC_IPC_H 

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <vec.h>

typedef struct 
{
    pid_t src;
    pid_t dst;
    uint64_t identifier;
    uintptr_t payload;
    bool payload_require_free;
} __attribute__((packed)) ipc_t;

typedef vec(ipc_t *) ipc_mailbox_t;

#endif /* !LIBC_INC_IPC_H */
