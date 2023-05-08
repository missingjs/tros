#include "kernel/debug.h"
#include "kernel/interrupt.h"
#include "kernel/signal.h"
#include "kernel/stdio-kernel.h"
#include "string.h"
#include "thread/thread.h"
#include "user/syscall.h"
#include "userprog/wait_exit.h"

static void setup_frame(struct intr_stack *stk, int signum, sighandler_t handler);
static void handle_signal_default(int signum);

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
    uint32_t ebp;
    struct intr_stack *stk;
    enum intr_status old_status = intr_disable();
    struct task_struct *current = running_thread();
    for (int i = 0; i < MAX_SIGNALS; ++i) {
        if (current->signal_bits & (1 << i)) {
            sighandler_t handler = current->sighandlers[i];
            if (handler == SIG_IGN) {
                continue;
            } else if (handler == SIG_DFL) {
                handle_signal_default(i);
                continue;
            }
            asm ("mov %%ebp, %0" : "=g" (ebp));
            // skip 'last ebp' and 'return address'
            stk = (struct intr_stack *)(ebp + 8);
            setup_frame(stk, i, handler);
            current->signal_bits &= ~(1 << i);
            break;
        }
    }
    intr_set_status(old_status);
}

static inline void write_byte(void *ptr, uint8_t val) {
    *(uint8_t*)ptr = val;
}

static inline void write_uint16(void *ptr, uint16_t val) {
    *(uint16_t*)ptr = val;
}

static inline void write_uint32(void *ptr, uint32_t val) {
    *(uint32_t*)ptr = val;
}

static void setup_frame(struct intr_stack *stk, int signum, sighandler_t handler) {
    void *uesp = stk->esp;  // esp of user stack

    // backup interrupt stack to the stack in user space
    uesp -= sizeof(struct intr_stack);
    memcpy(uesp, stk, sizeof(struct intr_stack));

    // write instruction on user stack: int $0x80
    uesp -= 2;
    write_uint16(uesp, (uint16_t)0x80cd);   // int $0x80

    // write instruction on user stack: mov $SYS_SIGRETURN, %eax
    uesp -= 4;
    write_uint32(uesp, SYS_SIGRETURN);
    uesp -= 1;
    write_byte(uesp, 0xb8);

    void *instr_addr = uesp;

    // setup signum as parameter of signal handler
    uesp -= 4;
    write_uint32(uesp, (uint32_t) signum);

    // return address in signal handler's frame
    uesp -= 4;
    write_uint32(uesp, (uint32_t) instr_addr);

    stk->esp = uesp;
    stk->eip = (void(*)(void)) handler;
}

uint32_t sys_sigreturn(void) {
    uint32_t ebp;
    asm ("mov %%ebp, %0" : "=g" (ebp));

    // skip 'last ebp', 'return address', p1, p2, p3
    // p1/p2/p3 is parameter of sys call function
    struct intr_stack *stk = (struct intr_stack *)(ebp + 20);

    // restore interrupt stack
    void *backup = (void*) stk->eip;
    memcpy(stk, backup, sizeof(struct intr_stack));
    return stk->eax;
}

static void handle_signal_default(int signum) {
    printk("process %d terminate by signal %d\n", sys_getpid(), signum);
    sys_exit(-1);
}