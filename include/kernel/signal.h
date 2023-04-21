#ifndef __KERNEL_SIGNAL_H
#define __KERNEL_SIGNAL_H

enum signals {
    SIGHUP = 1,
    SIGINT = 2
};

typedef void sig_handler(int num);

sig_handler *signal(int sig, sig_handler *handler);

#endif