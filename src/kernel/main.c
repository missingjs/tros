#include "device/console.h"
#include "device/timer.h"
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
void fork_test(void);

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
   cls_screen();
//   console_put_str("[rabbit@localhost /]$ ");

   // process_execute(fork_test, "fork_test");
   // thread_exit(running_thread(), true);
   while (1) {
      thread_yield();
   }
   return 0;
}

void fork_test(void) {
   int pipefd[2], ret;
   ret = pipe(pipefd);
   if (ret < 0) {
      printf("pipe failed\n");
      exit(1);
   }

   pid_t pid = fork();
   if (pid != 0) {
      printf("I'm parent process: %d\n", getpid());
      close(pipefd[0]);
      char alphabet[] = "abcdefghijklmn";
      while (1) {
         for (int i = 0; i < (int)sizeof(alphabet) - 1; ++i) {
            write(pipefd[1], &alphabet[i], 1);
            // msleep(1000);
         }
      }
      close(pipefd[1]);

      int32_t status;
      pid_t child_pid = wait(&status);
      printf("[parent] child process %d exit with %d\n", child_pid, status);
      char buf[10];
      read(0, buf, 1);
   } else {
      printf("I'm child process: %d %x\n", getpid());
      close(pipefd[1]);
      char buf[1];
      int count = 0;
      while (1) {
         ret = read(pipefd[0], buf, 1);
         if (ret == 0) {
            break;
         }
         ++count;
         printf("[child] read %c %d\n", buf[0], count);
      }
      close(pipefd[0]);
   }
   exit(0);
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
      // my_shell();
      fork_test();
   }
   panic("init: should not be here");
}
