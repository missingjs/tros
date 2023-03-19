#include "device/console.h"
#include "fs/dir.h"
#include "fs/fs.h"
#include "kernel/init.h"
#include "kernel/interrupt.h"
#include "kernel/memory.h"
#include "kernel/print.h"
#include "shell/shell.h"
#include "stdio.h"
#include "thread/thread.h"
#include "user/assert.h"
#include "user/syscall.h"
#include "userprog/process.h"
#include "userprog/syscall-init.h"

void init(void);

int main(void) {
   put_str("I am kernel\n");
   init_all();
   cls_screen();
   console_put_str("[rabbit@localhost /]$ ");
   while(1);
   return 0;
}

/* init进程 */
void init(void) {
   uint32_t ret_pid = fork();
   if(ret_pid) {  // 父进程
      while(1);
   } else {	  // 子进程
      my_shell();
   }
   panic("init: should not be here");
}
