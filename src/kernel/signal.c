#include "kernel/debug.h"
#include "kernel/interrupt.h"
#include "kernel/signal.h"
#include "kernel/stdio-kernel.h"
#include "thread/thread.h"

sighandler_t sys_signal(int sig, sighandler_t handler) {
    ASSERT(sig > 0 && sig < MAX_SIGNALS);
    struct task_struct *p = running_thread();
    sighandler_t old = p->sighandlers[sig];
    p->sighandlers[sig] = handler;
    return old;
}

int sys_kill(int pid, int signum) {
    ASSERT(signum > 0 && signum < MAX_SIGNALS);
    struct task_struct *proc = pid2thread(pid);
    if (!proc) {
        return -1;
    }
    proc->signal_bits |= (1 << signum);
    return 0;
}

void check_signal() {
    // ASSERT(intr_get_status() == INTR_OFF);
    enum intr_status old_status = intr_disable();
    struct task_struct *current = running_thread();
    for (int i = 0; i < MAX_SIGNALS; ++i) {
        if (current->signal_bits & (1 << i)) {
            // TODO
            printk("signal %d detected\n", i);
            current->signal_bits &= ~(1 << i);
        }
    }
    intr_set_status(old_status);
}