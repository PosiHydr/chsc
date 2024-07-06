#ifndef _MERR_H
#define _MERR_H

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MEatoi(...)     atoi(__VA_ARGS__)
#define MEstrcat(...)   strcat(__VA_ARGS__)
#define MEstrcmp(...)   strcmp(__VA_ARGS__)
#define MEstrcpy(...)   strcpy(__VA_ARGS__)
#define MEstrlen(...)   strlen(__VA_ARGS__)

#define MERR_UNIX(expr, type) \
    ({\
        type _merr_ret = expr;\
        if (_merr_ret == -1)\
        {\
            fprintf(stderr, "****************\nError: %s\nfile: %s\nline: %d\nfunc: %s\nexpr: %s = (%s)%lld\n",\
              strerror(errno), __FILE__, __LINE__, __FUNCTION__, #expr, #type, (long long)_merr_ret);\
            _Exit(errno);\
        }\
        _merr_ret;\
    })

#define MEexecve(...)       MERR_UNIX(execve(__VA_ARGS__), int)
#define MEfork(...)         MERR_UNIX(fork(__VA_ARGS__), pid_t)
#define MEkill(...)         MERR_UNIX(kill(__VA_ARGS__), int)
#define MEptrace(...)       MERR_UNIX(ptrace(__VA_ARGS__), long)
#define MEsem_destroy(...)  MERR_UNIX(sem_destroy(__VA_ARGS__), int)
#define MEsem_init(...)     MERR_UNIX(sem_init(__VA_ARGS__), int)
#define MEsem_post(...)     MERR_UNIX(sem_post(__VA_ARGS__), int)
#define MEsem_wait(...)     MERR_UNIX(sem_wait(__VA_ARGS__), int)
#define MEsigaction(...)    MERR_UNIX(sigaction(__VA_ARGS__), int)
#define MEsigfillset(...)   MERR_UNIX(sigfillset(__VA_ARGS__), int)
#define MEwaitpid(...)      MERR_UNIX(waitpid(__VA_ARGS__), pid_t)

#define MERR_POSIX(expr, type) \
    ({\
        type _merr_ret = expr;\
        if (_merr_ret)\
        {\
            fprintf(stderr, "****************\nError: %s\nfile: %s\nline: %d\nfunc: %s\nexpr: %s = (%s)%lld\n",\
              strerror(_merr_ret), __FILE__, __LINE__, __FUNCTION__, #expr, #type, (long long)_merr_ret);\
            _Exit(_merr_ret);\
        }\
        _merr_ret;\
    })

#define MEpthread_create(...)   MERR_POSIX(pthread_create(__VA_ARGS__), int)
#define MEpthread_detach(...)   MERR_POSIX(pthread_detach(__VA_ARGS__), int)
#define MEpthread_exit(...)     pthread_exit(__VA_ARGS__)
#define MEpthread_self(...)     pthread_self(__VA_ARGS__)

#define MERR_NULL(expr, type) \
    ({\
        type _merr_ret = expr;\
        if (! _merr_ret)\
        {\
            fprintf(stderr, "****************\nError: %s\nfile: %s\nline: %d\nfunc: %s\nexpr: %s = (%s)%lld\n",\
              strerror(errno), __FILE__, __LINE__, __FUNCTION__, #expr, #type, (long long)_merr_ret);\
            _Exit(errno);\
        }\
        _merr_ret;\
    })

#define MEmalloc(...)   MERR_NULL(malloc(__VA_ARGS__), void*)
#define MEfree(...)     free(__VA_ARGS__)

#define MERR_DLFCN_NULL(expr, type) \
    ({\
        type _merr_ret = expr;\
        if (! _merr_ret)\
        {\
            fprintf(stderr, "****************\nError: %s\nfile: %s\nline: %d\nfunc: %s\nexpr: %s = (%s)%lld\n",\
              dlerror(), __FILE__, __LINE__, __FUNCTION__, #expr, #type, (long long)_merr_ret);\
            _Exit(-1);\
        }\
        _merr_ret;\
    })

#define MEdlopen(...)   MERR_DLFCN_NULL(dlopen(__VA_ARGS__), void*)
#define MEdlsym(...)    MERR_DLFCN_NULL(dlsym(__VA_ARGS__), void*)

#define MERR_DLFCN_NONZERO(expr, type) \
    ({\
        type _merr_ret = expr;\
        if (_merr_ret)\
        {\
            fprintf(stderr, "****************\nError: %s\nfile: %s\nline: %d\nfunc: %s\nexpr: %s = (%s)%lld\n",\
              dlerror(), __FILE__, __LINE__, __FUNCTION__, #expr, #type, (long long)_merr_ret);\
            _Exit(_merr_ret);\
        }\
        _merr_ret;\
    })

#define MEdlclose(...)  MERR_DLFCN_NONZERO(dlclose(__VA_ARGS__), int)

#endif
