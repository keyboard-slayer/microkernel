#include <kernel/inc/logging.h>
#include <kernel/inc/sched.h>
#include <kernel/inc/com.h>
#include <kernel/inc/pmm.h>
#include <kernel/inc/arch.h>
#include <kernel/inc/utils.h>
#include <kernel/inc/loader.h>

#include <stdint.h>
#include <unistd.h>
#include <ipc.h>

#include "../inc/syscall.h"
#include "../inc/gdt.h"
#include "../inc/regs.h"
#include "../inc/asm.h"

typedef int64_t (*syscall_t)(regs_t *);

void syscall_init(void)
{
    klog(INFO, "Initializing system calls");
    asm_write_msr(MSR_EFER, asm_read_msr(MSR_EFER) | 1);
    asm_write_msr(MSR_STAR, ((uint64_t)(GDT_KERNEL_CODE * 8) << STAR_KCODE_OFFSET) | ((uint64_t) (((GDT_USER_DATA - 1) * 8) | 3 ) << STAR_UCODE_OFFSET));
    asm_write_msr(MSR_LSTAR, (uint64_t) syscall_handle);
    asm_write_msr(MSR_SYSCALL_FLAG_MASK, 0xfffffffe);
    klog(OK, "System calls initialized");
}

int64_t syscall_log(regs_t *regs)
{
    klog_lock();
    com_puts((char const *) regs->rbx);
    klog_unlock();
    return 0;
}

int64_t syscall_getpid(__attribute__((unused)) regs_t *regs)
{
    return sched_current()->pid;
}

int64_t syscall_alloc(regs_t *regs)
{
    return (uintptr_t) malloc(regs->rbx);
}

int64_t syscall_realloc(regs_t *regs)
{
    return (uintptr_t) realloc((void *) regs->rbx, regs->rcx);
}

int64_t syscall_free(regs_t *regs)
{
    free((void *) regs->rbx);
    return 0;
}

int64_t syscall_sendipc(regs_t *regs)
{
    ipc_t *ipc = (ipc_t *) regs->rbx;
    ipc->src = sched_current()->pid;
    task_t *dest;

    dest = sched_by_ident(ipc->identifier);

    if (dest == NULL)
    {
        dest = sched_by_pid(ipc->dst);
    }

    vmm_map(dest->space, (virtual_physical_map_t) {
        .physical = ALIGN_DOWN((uintptr_t) ipc - loader_get_hhdm(), PAGE_SIZE),
        .virtual = ALIGN_DOWN((uintptr_t) ipc, PAGE_SIZE),
        .length = ALIGN_UP(sizeof(ipc_t), PAGE_SIZE)
    }, true);

    vec_push(&dest->mailbox, ipc);
    return 0;
}

int64_t syscall_recvipc_sync(__attribute__((unused)) regs_t *regs)
{
    task_t *current = sched_current();

    while (current->mailbox.length == 0)
    {
        __asm__ ("pause");
    }

    ipc_t *ipc = vec_pop(&current->mailbox);

    return (uint64_t) ipc;
}

int64_t syscall_set_ident(regs_t *regs)
{
    task_t *maybe_task = sched_by_ident(regs->rbx);
    task_t *task = sched_current();

    if (maybe_task == NULL)
    {
        task->ident = regs->rbx;
        return 0;
    }

    return 1;
}

syscall_t syscall_matrix[] = {
    [SYS_LOG] = syscall_log,
    [SYS_GETPID] = syscall_getpid,
    [SYS_ALLOC] = syscall_alloc,
    [SYS_FREE] = syscall_free,
    [SYS_REALLOC] = syscall_realloc,
    [SYS_SENDIPC] = syscall_sendipc,
    [SYS_RECVIPC_SYNC] = syscall_recvipc_sync,
    [SYS_SETIDENT] = syscall_set_ident
};

int64_t syscall_handler(regs_t *regs)
{
    return syscall_matrix[regs->rax](regs);
}
