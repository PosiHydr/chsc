#include <unistd.h>
#include <pthread.h>

void* thd(void* arg)
{
    pthread_detach(pthread_self());    
    write(1, "Hello, World!\n", 14);
    return 0;
}

int main() {
    pthread_t tid;

    if (! fork())
    {
        vfork();
        write(1, "Hello, World!\n", 14);
    }
    else
    {
        pthread_create(&tid, 0, thd, 0);
        write(1, "Hello, World!\n", 14);
    }

    pthread_exit(0);

    return 0;
}
