#include "device/ioqueue.h"
#include "fs/file.h"
#include "fs/fs.h"
#include "ipc/pipe.h"
#include "kernel/debug.h"
#include "kernel/global.h"
#include "kernel/memory.h"
#include "kernel/stdio-kernel.h"
#include "thread/sync.h"
#include "thread/thread.h"

struct pipe_struct {
    struct lock lock;
    struct ioqueue q;
    int has_reader;
    int has_writer;
};

static int32_t anon_pipe_read(struct file *, char *, uint32_t);
static int32_t anon_pipe_write(struct file *, const char *, uint32_t);
static int32_t anon_pipe_release_reader(struct file *);
static int32_t anon_pipe_release_writer(struct file *);

static struct file_operations pipe_reader_ops = {
   .llseek  = no_llseek_fn,
   .read    = anon_pipe_read,
   .write   = no_write_fn,
   .release = anon_pipe_release_reader,
};

static struct file_operations pipe_writer_ops = {
   .llseek  = no_llseek_fn,
   .read    = no_read_fn,
   .write   = anon_pipe_write,
   .release = anon_pipe_release_writer,
};

static struct pipe_struct *create_pipe(void) {
   struct pipe_struct *p = (struct pipe_struct *) get_kernel_pages(1);
   if (p) {
      lock_init(&p->lock);
      ioqueue_init(&p->q);
      p->has_reader = p->has_writer = 0;
   }
   return p;
}

static void release_pipe(struct pipe_struct *p) {
   mfree_page(PF_KERNEL, p, 1);
}

/* 创建管道,成功返回0,失败返回-1 */
int32_t sys_pipe(int32_t pipefd[2]) {
   struct pipe_struct *pp = create_pipe();
   if (!pp) {
      return -1;
   }

   int32_t reader_gfd = get_free_slot_in_global();
   if (reader_gfd == -1) {
      printk("exceed max open files\n");
      release_pipe(pp);
      return -1;
   }

   int32_t writer_gfd = get_free_slot_in_global();
   if (writer_gfd == -1) {
      printk("exceed max open files\n");
      release_free_slot_in_global(reader_gfd);
      release_pipe(pp);
      return -1;
   }

   struct file *rfi = &file_table[reader_gfd];
   struct file *wfi = &file_table[writer_gfd];

   init_file_struct(rfi);
   atomic_inc(&rfi->count);
   rfi->fd_flag = O_RDWR;
   rfi->op = &pipe_reader_ops;
   rfi->private_data = pp;
   pp->has_reader = 1;

   init_file_struct(wfi);
   atomic_inc(&wfi->count);
   wfi->fd_flag = O_RDWR;
   wfi->op = &pipe_writer_ops;
   wfi->private_data = pp;
   pp->has_writer = 1;

   pipefd[0] = pcb_fd_install(reader_gfd);
   pipefd[1] = pcb_fd_install(writer_gfd);
   return 0;
}

/* 将文件描述符old_local_fd重定向为new_local_fd */
void sys_fd_redirect(uint32_t old_local_fd, uint32_t new_local_fd) {
   struct task_struct* cur = running_thread();
   uint32_t old_gfd = fd_local2global(old_local_fd);
   uint32_t new_gfd = fd_local2global(new_local_fd);
   struct file *old_filp = &file_table[old_gfd];
   struct file *new_filp = &file_table[new_gfd];
   ASSERT(old_filp != NULL);
   ASSERT(new_filp != NULL);

   /* 针对恢复标准描述符 */
   if (new_local_fd < 3) {
      cur->fd_table[old_local_fd] = new_local_fd;
   } else {
      uint32_t new_global_fd = cur->fd_table[new_local_fd];
      cur->fd_table[old_local_fd] = new_global_fd;
   }

   atomic_dec(&old_filp->count);
   atomic_inc(&new_filp->count);
}

static int32_t anon_pipe_read(struct file* filp, char* buffer, uint32_t size) {
   struct pipe_struct *pp = (struct pipe_struct *) filp->private_data;
   ASSERT(pp != NULL);
   // lock_acquire(&pp->lock);

   char *ptr = buffer;
   int n = 0;
   while (n < (int) size) {
      int ch = ioq_getchar(&pp->q);
      if (ch >= 0) {
         *ptr++ = (char) ch;
         ++n;
      } else {
         break;
      }
   }

   // lock_release(&pp->lock);
   return n;
}

static int32_t anon_pipe_write(struct file* filp, const char* content, uint32_t size) {
   struct pipe_struct *pp = (struct pipe_struct *) filp->private_data;
   const char *ptr = content;
   const char *end = ptr + size;

   ASSERT(pp != NULL);
   lock_acquire(&pp->lock);
   while (ptr != end) {
      if (ioq_putchar(&pp->q, *ptr)) {
         ++ptr;
      } else {
         break;
      }
   }
   lock_release(&pp->lock);
   return ptr - content;
}

static int32_t anon_pipe_release_reader(struct file *filp) {
   struct pipe_struct *pp = (struct pipe_struct *) filp->private_data;
   ASSERT(pp != NULL);

   bool destroy = false;
   lock_acquire(&pp->lock);
   pp->has_reader = 0;
   ioq_close(&pp->q);
   if (pp->has_writer == 0) {
      destroy = true;
   }
   lock_release(&pp->lock);
   if (destroy) {
      release_pipe(pp);
   }
   return 0;
}

static int32_t anon_pipe_release_writer(struct file *filp) {
   struct pipe_struct *pp = (struct pipe_struct *) filp->private_data;
   ASSERT(pp != NULL);

   bool destroy = false;
   lock_acquire(&pp->lock);
   pp->has_writer = 0;
   ioq_close(&pp->q);
   if (pp->has_reader == 0) {
      destroy = true;
   }
   lock_release(&pp->lock);
   if (destroy) {
      release_pipe(pp);
   }
   return 0;
}
