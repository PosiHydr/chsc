#include "chsc_dev.h"

#include <merr/stdlib.h>

const char chsc_pre[] = "chsc_";

uid_t euid_tar;

int chsc_init(int argc, char** argv, char** envp)
{
    if (argc != 1)
    {
        fprintf(stderr, "libcheuid.so: Has only an argument\n");
        return EINVAL;
    }
    euid_tar = MERR_atoi(argv[0]);
    return 0;
}

int chsc_uninit()
{
    return 0;
}

uid_t chsc_geteuid()
{
    pid_t pid = CHSC_GETPID();
    MERR_ptrace(PTRACE_SYSCALL, pid, 0, 0);
    MERR_waitpid(pid, 0, WUNTRACED);
    return euid_tar;
}
