#include "signal.h"
#include "stdio.h"
#include "string.h"
#include "syscall.h"

static void sig_int_handler(int signum) {
    printf("handling signal %d\n", signum);
    uint32_t ebp;
    asm ("mov %%ebp, %0" : "=g" (ebp));
    printf("return address: %x\n", *(uint32_t*)(ebp + 4));
}

int values[20];

int main(void) {
    for (int i = 0; i < sizeof(values)/sizeof(int); ++i) {
        if (values[i] != 0) {
            printf("values[%d] != 0\n", i);
        }
    }
    printf("address of void sig_int_handler(int): %x\n", &sig_int_handler);
    signal(SIGINT, sig_int_handler);

    int pid = getpid();
    kill(pid, SIGINT);
    printf("after kill\n");
    return 0;
}