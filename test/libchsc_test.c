#include "chsc_dev.h"

const char chsc_pre[] = "chsc_";

ssize_t chsc_write(int fd, void* p, size_t n)
{
    printf("###PID %d WRITE###\n", pid);
    MEptrace(PTRACE_SYSCALL, pid, 0, 0);
    waitpid(pid, 0, WUNTRACED);
    return n;
}
