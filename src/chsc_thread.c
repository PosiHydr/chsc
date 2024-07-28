#include "../include/chsc.h"

void* chsc_thread(chsc_args* argsp)
{
    int stat;
    void* scrp;
    chsc_args* nargsp;
    char scrn[SCRN_LEN];

    pid_t pid = argsp -> pid;

    MEptrace(PTRACE_ATTACH, pid, 0, 0);

    MEsem_wait(&chsc_threads_sem);
    chsc_threads++;
    MEsem_post(&chsc_threads_sem);

    MEstrcpy(scrn, chsc_prefix);

    MEpthread_detach(argsp -> tid);

    MEwaitpid(pid, 0, WUNTRACED);
    MEptrace(PTRACE_POKETEXT, pid, (argsp -> regs).REG_PC, argsp -> inbak);
    MEptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &(argsp -> iov));
    MEptrace(PTRACE_SETOPTIONS, pid, 0,PTRACE_O_TRACEEXIT | PTRACE_O_TRACECLONE | PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK | PTRACE_O_TRACESYSGOOD);

    while (1)
    {
        MEptrace(PTRACE_SYSCALL, pid, 0, 0);
        MEwaitpid(pid, &stat, WUNTRACED);
        if (stat>>8 == (SIGTRAP | (PTRACE_EVENT_EXIT<<8)))
        {
            MEptrace(PTRACE_DETACH, pid, 0, 0);
            break;
        }
        if ((stat>>8 == (SIGTRAP | (PTRACE_EVENT_CLONE<<8))) || (stat>>8 == (SIGTRAP | (PTRACE_EVENT_FORK<<8))) || (stat>>8 == (SIGTRAP | (PTRACE_EVENT_VFORK<<8))))
        {
            nargsp = MEmalloc(sizeof(chsc_args));
            MEptrace(PTRACE_GETEVENTMSG, pid, 0, &(nargsp -> pid));
            (nargsp -> iov).iov_base = &(nargsp -> regs);
            (nargsp -> iov).iov_len = sizeof(struct user_regs_struct);
            MEwaitpid(nargsp -> pid, 0, WUNTRACED);
            chsc_detach(nargsp);
            MEpthread_create(&(nargsp -> tid), 0, (void* (*)(void*))chsc_thread, nargsp);
        }
        MEptrace(PTRACE_GET_SYSCALL_INFO, pid, sizeof(struct ptrace_syscall_info), &(argsp -> info));
        if ((WSTOPSIG(stat) & 0x80) && ((argsp -> info).op == PTRACE_SYSCALL_INFO_ENTRY) && ((argsp -> info).entry.nr < SC_MAX) && (scname[(argsp -> info).entry.nr] != 0))
        {
            MEstrcpy(scrn + chsc_prefix_len, scname[(argsp -> info).entry.nr]);
            scrp = dlsym(chsc_handle, scrn);
            if (scrp)
            {
                errno = pid;
                REG_TYPE ret = ((chsc_syscall)scrp)((argsp -> info).entry.args[0], (argsp -> info).entry.args[1], (argsp -> info).entry.args[2], (argsp -> info).entry.args[3], (argsp -> info).entry.args[4], (argsp -> info).entry.args[5]);
                MEptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &(argsp -> iov));
                (argsp -> regs).REG_RET = ret;
                MEptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &(argsp -> iov));
            }
        }
    }

    MEfree(argsp);

    MEsem_wait(&chsc_threads_sem);
    chsc_threads--;
    if (! chsc_threads)
        MEsem_post(&chsc_finished_sem);
    MEsem_post(&chsc_threads_sem);

    return 0;
}
