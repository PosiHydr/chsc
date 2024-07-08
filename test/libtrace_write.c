#include "chsc_dev.h"

const char chsc_pre[] = "chsc_";

ssize_t chsc_write(int fd, void* p, size_t n)
{
    struct __ptrace_syscall_info info;

    MEptrace(PTRACE_SYSCALL, pid, 0, 0);
    MEwaitpid(pid, 0, WUNTRACED);
    MEptrace(PTRACE_GET_SYSCALL_INFO, pid, sizeof(struct __ptrace_syscall_info), &info);
    fprintf(stderr, "### PID %d write syscall returned %ld\n", pid, info.exit.rval);
    return info.exit.rval;
}
