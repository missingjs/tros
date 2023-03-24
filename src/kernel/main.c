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
#include "device/ioqueue.h"
#include "kernel/condition.h"

void init(void);

int x_value;
int max_value = 3;
struct lock xlock;
struct condition_variable cv_has_space;  // x_value < max_value
struct condition_variable cv_has_value;  // x_value > 0

void inc_value() {
    lock_acquire(&xlock);
    while (x_value == max_value) {
        cv_wait(&cv_has_space);
    }
    ++x_value;
    cv_notify_one(&cv_has_value);
    lock_release(&xlock);
}

void dec_value() {
    lock_acquire(&xlock);
    while (x_value == 0) {
        cv_wait(&cv_has_value);
    }
    --x_value;
    cv_notify_one(&cv_has_space);
    lock_release(&xlock);
}

void value_producer(void* arg) {
    int id = *(int*) arg;
    int cnt = 0;
    while (1) {
        inc_value();
        printk("p%d %d\n", id, ++cnt);
    }
}

void value_consumer(void* arg) {
    int id = *(int*) arg;
    int cnt = 0;
    while (1) {
        dec_value();
        printk("c%d %d\n", id, ++cnt);
    }
}

// struct ioqueue ioq;
// 
// int c1 = 1, c2 = 2, c3 = 3, c4 = 4;
// int p1 = 1, p2 = 2, p3 = 3, p4 = 4;
// 
// void consumer_thread(void* arg) {
//     int id = *(int*) arg;
//     int count = 0;
//     while (1) {
//         enum intr_status status = intr_disable();
//         char ch = ioq_getchar(&ioq);
//         intr_set_status(status);
//         ++count;
//         printk("consumer %d - %c %d\n", id, ch, count);
//     }
// }
// 
// void producer_thread(void* arg) {
//     int id = *(int*) arg;
//     int count = 0;
//     while (1) {
//         enum intr_status status = intr_disable();
//         ioq_putchar(&ioq, 'A' + id - 1);
//         intr_set_status(status);
//         mtime_sleep(50000);
//     }
// }

int main(void) {
   put_str("I am kernel\n");
   init_all();

   lock_init(&xlock);
   cv_init(&cv_has_space, &xlock);
   cv_init(&cv_has_value, &xlock);

   int c1 = 1;
   thread_start("c1", 10, value_consumer, &c1);

   int c2 = 2;
   thread_start("c2", 10, value_consumer, &c2);

   int c3 = 3;
   thread_start("c3", 10, value_consumer, &c3);

   int p1 = 1;
   thread_start("p1", 10, value_producer, &p1);

   int p2 = 2;
   thread_start("p2", 10, value_producer, &p2);

   int p3 = 3;
   thread_start("p3", 10, value_producer, &p3);

//   ioqueue_init(&ioq);
//
//   thread_start("c1", 20, consumer_thread, &c1);
//   thread_start("c2", 20, consumer_thread, &c2);
//   thread_start("c3", 20, consumer_thread, &c3);
//   thread_start("c4", 20, consumer_thread, &c4);
//
//   thread_start("p1", 20, producer_thread, &p1);
//   thread_start("p2", 20, producer_thread, &p2);
//   thread_start("p3", 20, producer_thread, &p3);
//   thread_start("p4", 20, producer_thread, &p4);

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
//   thread_exit(running_thread(), true);
   while (1) {
       thread_yield();
   }
   return 0;
}

/* init进程 */
void init(void) {
   uint32_t ret_pid = fork();
   if(ret_pid) {  // 父进程
      int status;
      int child_pid;
       /* init在此处不停的回收僵尸进程 */
       while(1) {
	  child_pid = wait(&status);
	  printf("I`m init, My pid is 1, I recieve a child, It`s pid is %d, status is %d\n", child_pid, status);
       }
   } else {	  // 子进程
      my_shell();
   }
   panic("init: should not be here");
}
