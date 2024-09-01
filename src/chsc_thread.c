#include "../include/chsc.h"

void* chsc_thread(chsc_args* argsp)
{
    int stat;
    chsc_syscall scrp;
    chsc_args* nargsp;
    char scrn[SCRN_LEN];

    pid_t pid = argsp -> pid;

    MERR_ptrace(PTRACE_ATTACH, pid, 0, 0);

    MERR_sem_wait(&chsc_threads_sem);
    chsc_threads++;
    MERR_sem_post(&chsc_threads_sem);

    MERR_strcpy(scrn, chsc_prefix);

    MERR_pthread_detach(argsp -> tid);

    MERR_waitpid(pid, 0, WUNTRACED);
    MERR_ptrace(PTRACE_POKETEXT, pid, (argsp -> regs).REG_PC, argsp -> inbak);
    MERR_ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &(argsp -> iov));
    MERR_ptrace(PTRACE_SETOPTIONS, pid, 0,PTRACE_O_TRACEEXIT | PTRACE_O_TRACECLONE | PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK | PTRACE_O_TRACESYSGOOD);

    while (1)
    {
        MERR_ptrace(PTRACE_SYSCALL, pid, 0, 0);
        MERR_waitpid(pid, &stat, WUNTRACED);
        if (stat>>8 == (SIGTRAP | (PTRACE_EVENT_EXIT<<8)))
        {
            MERR_ptrace(PTRACE_DETACH, pid, 0, 0);
            break;
        }
        if ((stat>>8 == (SIGTRAP | (PTRACE_EVENT_CLONE<<8))) || (stat>>8 == (SIGTRAP | (PTRACE_EVENT_FORK<<8))) || (stat>>8 == (SIGTRAP | (PTRACE_EVENT_VFORK<<8))))
        {
            nargsp = MERR_malloc(sizeof(chsc_args));
            MERR_ptrace(PTRACE_GETEVENTMSG, pid, 0, &(nargsp -> pid));
            (nargsp -> iov).iov_base = &(nargsp -> regs);
            (nargsp -> iov).iov_len = sizeof(struct user_regs_struct);
            MERR_waitpid(nargsp -> pid, 0, WUNTRACED);
            chsc_detach(nargsp);
            MERR_pthread_create(&(nargsp -> tid), 0, (void* (*)(void*))chsc_thread, nargsp);
        }
        MERR_ptrace(PTRACE_GET_SYSCALL_INFO, pid, sizeof(struct ptrace_syscall_info), &(argsp -> info));
        if ((WSTOPSIG(stat) & 0x80) && ((argsp -> info).op == PTRACE_SYSCALL_INFO_ENTRY) && ((argsp -> info).entry.nr < SC_MAX) && (scname[(argsp -> info).entry.nr] != 0))
        {
            MERR_strcpy(scrn + chsc_prefix_len, scname[(argsp -> info).entry.nr]);
            scrp = dlsym(chsc_handle, scrn);
            if (scrp)
            {
                errno = pid;
                REG_TYPE ret = scrp((argsp -> info).entry.args[0], (argsp -> info).entry.args[1], (argsp -> info).entry.args[2], (argsp -> info).entry.args[3], (argsp -> info).entry.args[4], (argsp -> info).entry.args[5]);
                MERR_ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &(argsp -> iov));
                (argsp -> regs).REG_RET = ret;
                MERR_ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &(argsp -> iov));
            }
        }
    }

    MERR_free(argsp);

    MERR_sem_wait(&chsc_threads_sem);
    chsc_threads--;
    if (! chsc_threads)
        MERR_sem_post(&chsc_finished_sem);
    MERR_sem_post(&chsc_threads_sem);

    return 0;
}
