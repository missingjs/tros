#include "signal.h"
#include "stdio.h"
#include "string.h"
#include "syscall.h"

// void sig_int_handler(int signum) {
//     printf("handling signal %d\n");
// }

int main(void) {
    int pid = getpid();
    kill(pid, SIGINT);
    return 0;
}