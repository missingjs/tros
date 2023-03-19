#include "device/console.h"
#include "fs/dir.h"
#include "fs/fs.h"
#include "kernel/init.h"
#include "kernel/interrupt.h"
#include "kernel/memory.h"
#include "kernel/print.h"
#include "stdio.h"
#include "thread/thread.h"
#include "user/syscall.h"
#include "userprog/process.h"
#include "userprog/syscall-init.h"

void init(void);

int main(void) {
   put_str("I am kernel\n");
   init_all();
/********  测试代码  ********/
/********  测试代码  ********/
   while(1);
   return 0;
}

/* init进程 */
void init(void) {
   uint32_t ret_pid = fork();
   if(ret_pid) {
      printf("i am father, my pid is %d, child pid is %d\n", getpid(), ret_pid);
   } else {
      printf("i am child, my pid is %d, ret pid is %d\n", getpid(), ret_pid);
   }
   while(1);
}
