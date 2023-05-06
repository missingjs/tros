#ifndef __KERNEL_SIGNAL_H
#define __KERNEL_SIGNAL_H

#define MAX_SIGNALS 16

enum signals {
    SIGHUP = 1,
    SIGINT = 2
};

typedef void (*sighandler_t)(int num);

sighandler_t sys_signal(int sig, sighandler_t handler);
int sys_kill(int pid, int signum);
void sys_sigreturn(void);

#define SIG_IGN ((sighandler_t)0x01)
#define SIG_DFL ((sighandler_t)0x02)

void check_signal(void);

#endif