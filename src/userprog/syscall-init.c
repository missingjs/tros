#include "device/console.h"
#include "device/timer.h"
#include "device/tty.h"
#include "fs/fs.h"
#include "ipc/pipe.h"
#include "kernel/interrupt.h"
#include "kernel/memory.h"
#include "kernel/print.h"
#include "stdint.h"
#include "string.h"
#include "thread/thread.h"
#include "user/syscall.h"
#include "userprog/exec.h"
#include "userprog/fork.h"
#include "userprog/syscall-init.h"
#include "userprog/wait_exit.h"

#define syscall_nr 64
typedef void* syscall;
syscall syscall_table[syscall_nr];

void sys_yield(void);

void sys_msleep(uint32_t millis) {
   mtime_sleep(millis);
}

void sys_yield(void) {
   thread_yield();
}

/* 初始化系统调用 */
void syscall_init(void) {
   put_str("syscall_init start\n");
   syscall_table[SYS_GETPID]	    = sys_getpid;
   syscall_table[SYS_WRITE]	    = sys_write;
   syscall_table[SYS_MALLOC]	    = sys_malloc;
   syscall_table[SYS_FREE]	    = sys_free;
   syscall_table[SYS_FORK]	    = sys_fork;
   syscall_table[SYS_READ]	    = sys_read;
   syscall_table[SYS_PUTCHAR]	    = sys_putchar;
   syscall_table[SYS_CLEAR]	    = cls_screen;
   syscall_table[SYS_GETCWD]	    = sys_getcwd;
   syscall_table[SYS_OPEN]	    = sys_open;
   syscall_table[SYS_CLOSE]	    = sys_close;
   syscall_table[SYS_LSEEK]	    = sys_lseek;
   syscall_table[SYS_UNLINK]	    = sys_unlink;
   syscall_table[SYS_MKDIR]	    = sys_mkdir;
   syscall_table[SYS_OPENDIR]	    = sys_opendir;
   syscall_table[SYS_CLOSEDIR]	    = sys_closedir;
   syscall_table[SYS_CHDIR]	    = sys_chdir;
   syscall_table[SYS_RMDIR]	    = sys_rmdir;
   syscall_table[SYS_READDIR]	    = sys_readdir;
   syscall_table[SYS_REWINDDIR]	    = sys_rewinddir;
   syscall_table[SYS_STAT]	    = sys_stat;
   syscall_table[SYS_PS]	    = sys_ps;
   syscall_table[SYS_EXECVE]	    = sys_execve;
   syscall_table[SYS_EXIT]	    = sys_exit;
   syscall_table[SYS_WAIT]	    = sys_wait;
   syscall_table[SYS_PIPE]	    = sys_pipe;
   syscall_table[SYS_FD_REDIRECT]   = sys_fd_redirect;
   syscall_table[SYS_HELP]	    = sys_help;
   syscall_table[SYS_MSLEEP]   = sys_msleep;
   syscall_table[SYS_YIELD]    = sys_yield;
   syscall_table[SYS_SIGNAL]   = sys_signal;
   syscall_table[SYS_KILL]     = sys_kill;
   syscall_table[SYS_SIGRETURN] = sys_sigreturn;
   syscall_table[SYS_SET_FG_PID] = sys_set_fg_pid;
   put_str("syscall_init done\n");
}
