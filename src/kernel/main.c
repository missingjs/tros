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

#include "device/ide.h"
#include "kernel/stdio-kernel.h"

void init(void);

int main(void) {
   put_str("I am kernel\n");
   init_all();

/*************    写入应用程序    *************/
//   uint32_t file_size = 11988;
//   uint32_t sec_cnt = DIV_ROUND_UP(file_size, 512);
//   struct disk* sda = &channels[0].devices[0];
//   void* prog_buf = sys_malloc(sec_cnt * 512);
//   ide_read(sda, 300, prog_buf, sec_cnt);
//   int32_t fd = sys_open("/cat", O_CREAT|O_RDWR);
//   if (fd != -1) {
//      if(sys_write(fd, prog_buf, file_size) == -1) {
//         printk("file write error!\n");
//         while(1);
//      }
//   }
/*************    写入应用程序结束   *************/
   // cls_screen();
//   console_put_str("[rabbit@localhost /]$ ");
  thread_exit(running_thread(), true);
   // while (1) {
   //     thread_yield();
   // }
   return 0;
}

/* init进程 */
void init(void)
{
   uint32_t ret_pid = fork();
   if (ret_pid)
   { // 父进程
      int status;
      int child_pid;
      /* init在此处不停的回收僵尸进程 */
      while (1)
      {
         child_pid = wait(&status);
         printf("I`m init, My pid is 1, I recieve a child, It`s pid is %d, status is %d\n", child_pid, status);
      }
   }
   else
   { // 子进程
      my_shell();
   }
   panic("init: should not be here");
}
