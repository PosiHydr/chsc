#ifndef _CHSC_DEV_H_
#define _CHSC_DEV_H_

#include <elf.h>
#include <sys/uio.h>
#include <sys/user.h>
#include <linux/ptrace.h>

#include <merr/sys/wait.h>
#include <merr/sys/ptrace.h>

#define CHSC_GETPID() errno

#endif
