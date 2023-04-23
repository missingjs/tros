#include "kernel/signal.h"
#include "stdio.h"
#include "string.h"
#include "user/syscall.h"

static void child_sigint_handler(int signum) {
    printf("child process %d is handling signal %d\n", getpid(), signum);
}

int main(void) {
    signal(SIGINT, child_sigint_handler);

    int pid = fork();
    if (pid == 0) {
        while (1);
    } else {
        kill(pid, SIGINT);
        msleep(5000);
        kill(pid, SIGHUP);
        int st;
        wait(&st);
        printf("[parent] detect child process exit\n");
    }

    return 0;
}