#include "chsc_dev.h"

const char chsc_pre[] = "chsc_";

ssize_t chsc_write(int fd, void* p, size_t n)
{
    pid_t pid = CHSC_GETPID();
    struct ptrace_syscall_info info;

    MERR_ptrace(PTRACE_SYSCALL, pid, 0, 0);
    MERR_waitpid(pid, 0, WUNTRACED);
    MERR_ptrace(PTRACE_GET_SYSCALL_INFO, pid, sizeof(struct ptrace_syscall_info), &info);
    fprintf(stderr, "### PID %d write syscall returned %lld\n", pid, info.exit.rval);
    return info.exit.rval;
}
