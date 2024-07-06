#ifndef _CHSC_H
#define _CHSC_H

#include <elf.h>
#include <dlfcn.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/uio.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/ptrace.h>

#include "arch.h"
#include "merr.h"

#define SCRN_LEN    64
#define SCRN_PRE    "chsc_pre"
#define SCRR_PID    "pid"
#define SCIN_SUF    "init"
#define SCUIN_SUF   "uninit"

typedef struct
{
    pid_t		pid;
    pthread_t	tid;
    long		inbak;
    struct iovec iov;
    struct user_regs_struct regs;
    struct __ptrace_syscall_info info;
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
