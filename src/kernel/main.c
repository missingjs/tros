#include "device/console.h"
#include "device/timer.h"
#include "fs/dir.h"
#include "fs/fs.h"
#include "kernel/debug.h"
#include "kernel/init.h"
#include "kernel/interrupt.h"
#include "kernel/memory.h"
#include "kernel/print.h"
#include "shell/shell.h"
#include "stdio.h"
#include "string.h"
#include "thread/thread.h"
#include "user/assert.h"
#include "user/syscall.h"
#include "userprog/process.h"
#include "userprog/syscall-init.h"

#include "device/ide.h"
#include "kernel/stdio-kernel.h"

void init(void);
void fork_test(void);
static const char *parse_index_line(const char *ptr, const char *end, char *f_path, int *f_offset, int *f_size);
static bool init_done = false;

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

   struct disk* sda = &channels[0].devices[0];
   void *buf = sys_malloc(SECTOR_SIZE);
   int size_file_seek = 500;
   ide_read(sda, size_file_seek, buf, 1);
   int32_t index_file_size = *(int32_t*)buf;
   int32_t pack_file_size = *(int32_t*)(buf + 4);
   sys_free(buf);
   printk("index file size: %d, package file size: %d\n", index_file_size, pack_file_size);

   int index_file_seek = 600;
   int index_file_sec_count = DIV_ROUND_UP(index_file_size, SECTOR_SIZE);
   void *idx_buf = sys_malloc(SECTOR_SIZE * index_file_sec_count);
   ide_read(sda, index_file_seek, idx_buf, index_file_sec_count);
   void *idx_content = sys_malloc(index_file_size + 1);
   memcpy(idx_content, idx_buf, index_file_size);
   sys_free(idx_buf);

   int pack_file_seek = 700;
   int pack_file_sec_count = DIV_ROUND_UP(pack_file_size, SECTOR_SIZE);
   buf = sys_malloc(SECTOR_SIZE * pack_file_sec_count);
   ide_read(sda, pack_file_seek, buf, pack_file_sec_count);

   char f_path[128];
   int f_offset, f_size;
   const char *ptr = (const char *)idx_content, *end = ptr + index_file_size;
   while ((ptr = parse_index_line(ptr, end, f_path, &f_offset, &f_size))) {
      ASSERT(strlen(f_path) > 0);
      ASSERT(f_path[0] == '/');
      ASSERT(f_offset >= 0);
      ASSERT(f_size >= 0);
      printk("%s %d %d\n", f_path, f_offset, f_size);
      sys_unlink(f_path);  // It doesn't matter if f_path not exist
      int32_t fd = sys_open(f_path, O_CREAT | O_RDWR);
      if (fd < 0) {
         panic("failed to create/open system file");
      }
      if (sys_write(fd, buf + f_offset, (uint32_t)f_size) < 0) {
         panic("failed to write system file");
      }
      sys_close(fd);
   }

   sys_free(idx_content);
   sys_free(buf);

   // cls_screen();
   init_done = true;

   while (1) {
      // thread_yield();
      thread_block(TASK_BLOCKED);
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
   while (!init_done) {
      yield();
   }
   
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
      // fork_test();
   }
   panic("init: should not be here");
}

static const char *parse_index_line(const char *ptr, const char *end, char *f_path, int *f_offset, int *f_size) {
   // char buffer[128];
   // memcpy(buffer, ptr, end-ptr);
   // buffer[end-ptr] = 0;
   // printk("%x, %x, line: [%s], %d\n", ptr, end, buffer, end-ptr);
   // for (const char *p = ptr; p != end; ++p) {
   //    printk("%d ", *p);
   // }
   // printk("\n");

   // parse path
   char *p = f_path;
   while (ptr != end && *ptr != ' ') {
      *p++ = *ptr++;
   }
   if (ptr == end) {
      return NULL;
   }
   *p = 0;
   ++ptr;  // skip ' '

   // parse offset
   int offset = 0;
   while (ptr != end && *ptr != ' ') {
      int val = *ptr - '0';
      offset = offset * 10 + val;
      ++ptr;
   }
   if (ptr == end) {
      return NULL;
   }
   *f_offset = offset;
   ++ptr;  // skip ' '

   // parse size
   int size = 0;
   while (ptr != end && *ptr != '\n') {
      int val = *ptr - '0';
      size = size * 10 + val;
      ++ptr;
   }
   if (ptr == end) {
      return NULL;
   }
   *f_size = size;
   return ptr + 1;
}
