#include "stdio.h"
#include "user/syscall.h"

int main(int argc, char *argv[] __attribute__ ((unused))) {
    if (argc != 1) {
        printf("clear: no argument support!\n");
        return 1;
    }
    clear();
    return 0;
}
