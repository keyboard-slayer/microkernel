#ifndef LIBC_INC_UNISTD_H
#define LIBC_INC_UNISTD_H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <ipc.h>

enum syscall
{
    SYS_LOG,
    SYS_GETPID,
    SYS_SENDIPC,
    SYS_RECVIPC_SYNC,
    SYS_ALLOC,
    SYS_FREE,
    SYS_REALLOC
};

void syslog(char const *message);
uintptr_t ipc_receive_sync(void);
void ipc_send(ipc_t *ipc);
pid_t getpid(void);
uint64_t syscall_impl(uint64_t syscall_id, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4);
#define __syscall(id, a1, a2, a3, a4, ...) syscall_impl(id, a1, a2, a3, a4)
#define syscall(...) __syscall(__VA_ARGS__, 0, 0, 0, 0)


#endif /* !LIBC_INC_UNISTD_H */
