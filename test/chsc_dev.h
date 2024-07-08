#ifndef _CHSC_DEV_H
#define _CHSC_DEV_H

#include "../include/merr.h"

#include <elf.h>
#include <sys/uio.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ptrace.h>

__thread pid_t pid;

#endif
