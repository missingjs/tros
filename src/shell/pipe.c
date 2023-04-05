#include "device/ioqueue.h"
#include "fs/file.h"
#include "fs/fs.h"
#include "kernel/memory.h"
#include "shell/pipe.h"
#include "thread/sync.h"
#include "thread/thread.h"

struct pipe_struct {
    struct lock lock;
    struct ioqueue q;
    int reader;
    int writer;
};

static int32_t anon_pipe_read(struct file *, char *, uint32_t);
static int32_t anon_pipe_write(struct file *, const char *, uint32_t);
static int32_t anon_pipe_release_reader(struct file *);
static int32_t anon_pipe_release_writer(struct file *);
static int32_t anon_pipe_lseek(struct file *filp, int32_t offset, int32_t whence);

static int32_t no_pipe_read(struct file *, char *, uint32_t);
static int32_t no_pipe_write(struct file *, const char *, uint32_t);

static struct file_operations pipe_reader_ops = {
   .llseek  = anon_pipe_lseek,
   .read    = anon_pipe_read,
   .write   = no_pipe_write,
   .release = anon_pipe_release_reader,
};

static struct file_operations pipe_writer_ops = {
   .llseek  = anon_pipe_lseek,
   .read    = no_pipe_read,
   .write   = anon_pipe_write,
   .release = anon_pipe_release_writer,
};

static struct pipe_struct *create_pipe() {
   struct pipe_struct *p = (struct pipe_struct *) get_kernel_pages(1);
   if (p) {
      lock_init(&p->lock);
      ioqueue_init(&p->q);
      p->reader = p->writer = 0;
   }
   return p;
}

static void release_pipe(struct pipe_struct *p) {
   mfree_page(PF_KERNEL, p, 1);
}

// /* 判断文件描述符local_fd是否是管道 */
// bool is_pipe(uint32_t local_fd) {
//    uint32_t global_fd = fd_local2global(local_fd);
//    return file_table[global_fd].fd_flag == PIPE_FLAG;
// }

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
   pp->reader = 1;

   init_file_struct(wfi);
   atomic_inc(&wfi->count);
   wfi->fd_flag = O_RDWR;
   wfi->op = &pipe_writer_ops;
   wfi->private_data = pp;
   pp->writer = 1;

   pipefd[0] = pcb_fd_install(reader_gfd);
   pipefd[1] = pcb_fd_install(writer_gfd);
   return 0;
}
// int32_t sys_pipe(int32_t pipefd[2]) {
//    int32_t global_fd = get_free_slot_in_global();

//    /* 申请一页内核内存做环形缓冲区 */
//    file_table[global_fd].fd_inode = get_kernel_pages(1);

//    /* 初始化环形缓冲区 */
//    ioqueue_init((struct ioqueue*)file_table[global_fd].fd_inode);
//    if (file_table[global_fd].fd_inode == NULL) {
//       return -1;
//    }

//    /* 将fd_flag复用为管道标志 */
//    file_table[global_fd].fd_flag = PIPE_FLAG;

//    /* 将fd_pos复用为管道打开数 */
//    file_table[global_fd].fd_pos = 2;
//    pipefd[0] = pcb_fd_install(global_fd);
//    pipefd[1] = pcb_fd_install(global_fd);
//    return 0;
// }

// /* 从管道中读数据 */
// uint32_t pipe_read(int32_t fd, void* buf, uint32_t count) {
//    char* buffer = buf;
//    uint32_t bytes_read = 0;
//    uint32_t global_fd = fd_local2global(fd);

//    /* 获取管道的环形缓冲区 */
//    struct ioqueue* ioq = (struct ioqueue*)file_table[global_fd].fd_inode;

//    /* 选择较小的数据读取量,避免阻塞 */
//    uint32_t ioq_len = ioq_length(ioq);
//    uint32_t size = ioq_len > count ? count : ioq_len;
//    while (bytes_read < size) {
//       *buffer = ioq_getchar(ioq);
//       bytes_read++;
//       buffer++;
//    }
//    return bytes_read;
// }

// /* 往管道中写数据 */
// uint32_t pipe_write(int32_t fd, const void* buf, uint32_t count) {
//    uint32_t bytes_write = 0;
//    uint32_t global_fd = fd_local2global(fd);
//    struct ioqueue* ioq = (struct ioqueue*)file_table[global_fd].fd_inode;

//    /* 选择较小的数据写入量,避免阻塞 */
//    uint32_t ioq_left = bufsize - ioq_length(ioq);
//    uint32_t size = ioq_left > count ? count : ioq_left;

//    const char* buffer = buf;
//    while (bytes_write < size) {
//       ioq_putchar(ioq, *buffer);
//       bytes_write++;
//       buffer++;
//    }
//    return bytes_write;
// }

/* 将文件描述符old_local_fd重定向为new_local_fd */
void sys_fd_redirect(uint32_t old_local_fd, uint32_t new_local_fd) {
   struct task_struct* cur = running_thread();
   /* 针对恢复标准描述符 */
   if (new_local_fd < 3) {
      cur->fd_table[old_local_fd] = new_local_fd;
   } else {
      uint32_t new_global_fd = cur->fd_table[new_local_fd];
      cur->fd_table[old_local_fd] = new_global_fd;
   }
}

static int32_t anon_pipe_read(struct file* filp, char* buffer, uint32_t size) {
   struct pipe_struct *pp = (struct pipe_struct *) filp->private_data;
   ASSERT(pp != NULL);
   lock_acquire(&pp->lock);

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

   lock_release(&pp->lock);
   return n;
}

static int32_t anon_pipe_write(struct file* filp, const char* content, uint32_t size) {
   struct pipe_struct *pp = (struct pipe_struct *) filp->private_data;
   ASSERT(pp != NULL);
   lock_acquire(&pp->lock);

   const char *ptr = content;
   const char *end = ptr + size;
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
   int ref = atomic_dec(&filp->count);
   if (ref == 0) {
      lock_acquire(&pp->lock);
      pp->reader = 0;
      ioq_close(&pp->q);
      if (pp->writer == 0) {
         destroy = true;
      }
      lock_release(&pp->lock);
   }
   if (destroy) {
      release_pipe(pp);
   }
   return 0;
}

static int32_t anon_pipe_release_writer(struct file *filp) {
   struct pipe_struct *pp = (struct pipe_struct *) filp->private_data;
   ASSERT(pp != NULL);

   bool destroy = false;
   int ref = atomic_dec(&filp->count);
   if (ref == 0) {
      lock_acquire(&pp->lock);
      pp->writer = 0;
      ioq_close(&pp->q);
      if (pp->reader == 0) {
         destroy = true;
      }
      lock_release(&pp->lock);
   }
   if (destroy) {
      release_pipe(pp);
   }
   return 0;
}

static int32_t anon_pipe_lseek(struct file *filp, int32_t offset, int32_t whence) {
   return -1;
}

static int32_t no_pipe_read(struct file *filp, char *buffer, uint32_t size) {
   return -1;
}

static int32_t no_pipe_write(struct file *filp, const char *content, uint32_t size) {
   return -1;
}
