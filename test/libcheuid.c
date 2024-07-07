#include "chsc_dev.h"

const char chsc_pre[] = "chsc_";

uid_t euid_tar;

int chsc_init(int argc, char** argv, char** envp)
{
    if (argc != 1)
    {
        fprintf(stderr, "libcheuid.so: Has only an argument\n");
        return EINVAL;
    }
    euid_tar = MEatoi(argv[0]);
    return 0;
}

int chsc_uninit()
{
    return 0;
}

uid_t chsc_geteuid()
{
    MEptrace(PTRACE_SYSCALL, pid, 0, 0);
    MEwaitpid(pid, 0, WUNTRACED);
    return euid_tar;
}
