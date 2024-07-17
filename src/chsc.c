#include "../include/chsc.h"

#include "../arch/scname.c"
#include "chsc_detach.c"
#include "chsc_thread.c"

int main(int argc, char** argv, char** envp)
{
/*
argc = 3;
char* nargv[] = {"/workspace/chsc/.c_bin/chsc", "test/libtrace_wrtie.so", "test/hwx", 0};
argv = nargv;
*/

/*
argc = 5;
char* nargv[] = {"/workspace/chsc/.c_bin/chsc", "1", ":", "/workspace/chsc/test/libchuid.so", "/usr/bin/whoami", 0};
argv = nargv;
*/

    pid_t pid;
    chsc_args* argsp;
    char scin[SCRN_LEN];
    int (*uninitp)();
    int (*initp)(int, char**, char**);

    int ret = 0;
    size_t sep_ind = 0;

    for (int i = 1; i < argc; i++)
    {
        if (! MEstrcmp(argv[i], ":"))
        {
            sep_ind = i;
            break;
        }
    }

    if (argc < sep_ind + 3)
    {
        fprintf(stderr, "Usage: %s [LIB_ARGS :] LIB EXEC [EXEC_ARGS]\n", argv[0]);
        return EINVAL;
    }

    pid = MEfork();
    if (! pid)
    {
        MEptrace(PTRACE_TRACEME, 0, 0, 0);
        MEexecve(argv[sep_ind + 2], argv + sep_ind + 2, envp);
    }

    chsc_handle = MEdlopen(argv[sep_ind + 1], RTLD_NOW);
    chsc_prefix = MEdlsym(chsc_handle, SCRN_PRE);
    chsc_prefix_len = MEstrlen(chsc_prefix);
    MEstrcpy(scin, chsc_prefix);

    MEstrcpy(scin + chsc_prefix_len, SCIN_SUF);
    initp = dlsym(chsc_handle, scin);
    if (initp)
    {
        ret = initp(sep_ind == 0 ? 0 : (sep_ind - 1), sep_ind < 2 ? 0 : (argv + 1), envp);
        if (ret)
        {
            MEkill(pid, SIGKILL);
            MEdlclose(chsc_handle);
            return ret;
        }
    }

    chsc_threads = 0;
    MEsem_init(&chsc_threads_sem, 0, 1);
    MEsem_init(&chsc_finished_sem, 0, 0);

    argsp = MEmalloc(sizeof(chsc_args));
    argsp -> pid = pid;
    (argsp -> iov).iov_base = &(argsp -> regs);
    (argsp -> iov).iov_len = sizeof(struct user_regs_struct);
    MEwaitpid(pid, 0, WUNTRACED);
    chsc_detach(argsp);

    MEpthread_create(&(argsp -> tid), 0, (void* (*)(void*))chsc_thread, argsp);

    MEsem_wait(&chsc_finished_sem);
    MEsem_wait(&chsc_threads_sem);
    MEsem_destroy(&chsc_threads_sem);
    MEsem_destroy(&chsc_finished_sem);

    MEstrcpy(scin + chsc_prefix_len, SCUIN_SUF);
    uninitp = dlsym(chsc_handle, scin);
    if (uninitp)
        ret = uninitp();

    MEdlclose(chsc_handle);

    return ret;
}
