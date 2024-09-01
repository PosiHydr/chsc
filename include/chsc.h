#ifndef _CHSC_H_
#define _CHSC_H_

#include <elf.h>
#include <sys/uio.h>
#include <sys/user.h>
#include <linux/ptrace.h>

#include <merr/dlfcn.h>
#include <merr/signal.h>
#include <merr/stdlib.h>
#include <merr/string.h>
#include <merr/unistd.h>
#include <merr/pthread.h>
#include <merr/sys/wait.h>
#include <merr/semaphore.h>
#include <merr/sys/ptrace.h>

#include "../arch/arch.h"

#define SCRN_LEN    64
#define SCRN_PRE    "chsc_pre"
#define SCIN_SUF    "init"
#define SCUIN_SUF   "uninit"

typedef struct
{
    pid_t       pid;
    pthread_t   tid;
    long        inbak;
    struct iovec iov;
    struct user_regs_struct regs;
    struct ptrace_syscall_info info;
} chsc_args;

typedef REG_TYPE (*chsc_syscall)(REG_TYPE, REG_TYPE, REG_TYPE, REG_TYPE, REG_TYPE, REG_TYPE);

volatile int chsc_threads;
sem_t chsc_threads_sem;
sem_t chsc_finished_sem;

void* chsc_handle;
char* chsc_prefix;
size_t chsc_prefix_len;

void chsc_detach(chsc_args* argsp);
void* chsc_thread(chsc_args* argsp);

#endif
