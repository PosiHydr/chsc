#ifndef _MERR_H
#define _MERR_H

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MERR_MODEL(expr, type, judge, errstr, exit_ret, init)\
    ({\
        init;\
        type _merr_ret = expr;\
        if (judge)\
        {\
            fprintf(stderr, "\n****************\nError: %s\nfile: %s\nline: %d\nfunc: %s\nexpr: %s = (%s)%lld\n****************\n\n",\
              errstr, __FILE__, __LINE__, __FUNCTION__, #expr, #type, (long long)_merr_ret);\
            _Exit(exit_ret);\
        }\
        _merr_ret;\
    })

#define MERR_UNIX(expr, type)   MERR_MODEL(expr, type, _merr_ret == -1, strerror(errno), errno, )
#define MERR_POSIX(expr, type)  MERR_MODEL(expr, type, _merr_ret, strerror(_merr_ret), _merr_ret, )
#define MERR_ZERO(expr, type)   MERR_MODEL(expr, type, ! _merr_ret, strerror(errno), errno, )
#define MERR_DLFCN_ZERO(expr, type)     MERR_MODEL(expr, type, ! _merr_ret, dlerror(), -1, )
#define MERR_DLFCN_NONZERO(expr, type)  MERR_MODEL(expr, type, _merr_ret, dlerror(), _merr_ret, )
#define MERR_UNIX_ERRNO(expr, type)     MERR_MODEL(expr, type, errno, strerror(errno), errno, errno = 0)

#define MEfree(...)         free(__VA_ARGS__)
#define MEpthread_exit(...) pthread_exit(__VA_ARGS__)
#define MEpthread_self(...) pthread_self(__VA_ARGS__)
#define MEstrcat(...)       strcat(__VA_ARGS__)
#define MEstrcmp(...)       strcmp(__VA_ARGS__)
#define MEstrcpy(...)       strcpy(__VA_ARGS__)
#define MEstrlen(...)       strlen(__VA_ARGS__)

#define MEatoi(...) MERR_MODEL(atoi(__VA_ARGS__), int, ! _merr_ret, "atoi() error", -1, )

#define MEexecve(...)       MERR_UNIX(execve(__VA_ARGS__), int)
#define MEfork(...)         MERR_UNIX(fork(__VA_ARGS__), pid_t)
#define MEkill(...)         MERR_UNIX(kill(__VA_ARGS__), int)
#define MEsem_destroy(...)  MERR_UNIX(sem_destroy(__VA_ARGS__), int)
#define MEsem_init(...)     MERR_UNIX(sem_init(__VA_ARGS__), int)
#define MEsem_post(...)     MERR_UNIX(sem_post(__VA_ARGS__), int)
#define MEsem_wait(...)     MERR_UNIX(sem_wait(__VA_ARGS__), int)
#define MEsigaction(...)    MERR_UNIX(sigaction(__VA_ARGS__), int)
#define MEsigfillset(...)   MERR_UNIX(sigfillset(__VA_ARGS__), int)
#define MEwaitpid(...)      MERR_UNIX(waitpid(__VA_ARGS__), pid_t)

#define MEpthread_create(...)   MERR_POSIX(pthread_create(__VA_ARGS__), int)
#define MEpthread_detach(...)   MERR_POSIX(pthread_detach(__VA_ARGS__), int)

#define MEmalloc(...)   MERR_ZERO(malloc(__VA_ARGS__), void*)

#define MEdlopen(...)   MERR_DLFCN_ZERO(dlopen(__VA_ARGS__), void*)
#define MEdlsym(...)    MERR_DLFCN_ZERO(dlsym(__VA_ARGS__), void*)

#define MEdlclose(...)  MERR_DLFCN_NONZERO(dlclose(__VA_ARGS__), int)

#define MEptrace(...)   MERR_UNIX_ERRNO(ptrace(__VA_ARGS__), long)

#endif
