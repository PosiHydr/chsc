#include <unistd.h>

int main() {
    fork();
    fork();
    write(1, "Hello, World!\n", 14);
    return 0;
}
