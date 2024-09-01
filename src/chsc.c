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
        if (! MERR_strcmp(argv[i], ":"))
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

    pid = MERR_fork();
    if (! pid)
    {
        MERR_ptrace(PTRACE_TRACEME, 0, 0, 0);
        MERR_execve(argv[sep_ind + 2], argv + sep_ind + 2, envp);
    }

    chsc_handle = MERR_dlopen(argv[sep_ind + 1], RTLD_NOW);
    chsc_prefix = MERR_dlsym(chsc_handle, SCRN_PRE);
    chsc_prefix_len = MERR_strlen(chsc_prefix);
    MERR_strcpy(scin, chsc_prefix);

    MERR_strcpy(scin + chsc_prefix_len, SCIN_SUF);
    initp = dlsym(chsc_handle, scin);
    if (initp)
    {
        ret = initp(sep_ind == 0 ? 0 : (sep_ind - 1), sep_ind < 2 ? 0 : (argv + 1), envp);
        if (ret)
        {
            MERR_kill(pid, SIGKILL);
            MERR_dlclose(chsc_handle);
            return ret;
        }
    }

    chsc_threads = 0;
    MERR_sem_init(&chsc_threads_sem, 0, 1);
    MERR_sem_init(&chsc_finished_sem, 0, 0);

    argsp = MERR_malloc(sizeof(chsc_args));
    argsp -> pid = pid;
    (argsp -> iov).iov_base = &(argsp -> regs);
    (argsp -> iov).iov_len = sizeof(struct user_regs_struct);
    MERR_waitpid(pid, 0, WUNTRACED);
    chsc_detach(argsp);

    MERR_pthread_create(&(argsp -> tid), 0, (void* (*)(void*))chsc_thread, argsp);

    MERR_sem_wait(&chsc_finished_sem);
    MERR_sem_wait(&chsc_threads_sem);
    MERR_sem_destroy(&chsc_finished_sem);
    MERR_sem_destroy(&chsc_threads_sem);

    MERR_strcpy(scin + chsc_prefix_len, SCUIN_SUF);
    uninitp = dlsym(chsc_handle, scin);
    if (uninitp)
        ret = uninitp();

    MERR_dlclose(chsc_handle);

    return ret;
}
