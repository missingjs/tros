#ifndef __FS_FILE_H
#define __FS_FILE_H
#include "atomic.h"
#include "device/ide.h"
#include "fs/dir.h"
#include "kernel/global.h"
#include "stdint.h"
#include "thread/sync.h"

struct file;
struct inode;

struct file_operations {
   int32_t (* llseek)(struct file *, int32_t, int32_t);   // (filp, offset, whence)
   int32_t (* read)(struct file *, char *, uint32_t);         // (filp, buffer, size, filepos)
   int32_t (* write)(struct file *, const char *, uint32_t);  // (filp, buffer, size, filepos)
   int32_t (* release)(struct file *);
};

/* 文件结构 */
struct file {
   // uint32_t fd_pos;      // 记录当前文件操作的偏移地址,以0为起始,最大为文件大小-1
   int32_t fd_pos;
   uint32_t fd_flag;
   struct inode* fd_inode;
   struct atomic_t count;
   struct file_operations* op;
   void *private_data;
};

/* 标准输入输出描述符 */
enum std_fd {
   stdin_no,   // 0 标准输入
   stdout_no,  // 1 标准输出
   stderr_no   // 2 标准错误
};

/* 位图类型 */
enum bitmap_type {
   INODE_BITMAP,     // inode位图
   BLOCK_BITMAP	     // 块位图
};

#define MAX_FILE_OPEN 32    // 系统可打开的最大文件数

extern struct file file_table[MAX_FILE_OPEN];
extern struct lock file_table_lock;

int32_t inode_bitmap_alloc(struct partition* part);
int32_t block_bitmap_alloc(struct partition* part);
int32_t file_create(struct dir* parent_dir, char* filename, uint8_t flag);
void bitmap_sync(struct partition* part, uint32_t bit_idx, uint8_t btmp);
int32_t get_free_slot_in_global(void);
int32_t pcb_fd_install(int32_t globa_fd_idx);
int32_t file_open(uint32_t inode_no, uint8_t flag);
void init_file_struct(struct file *filp);
void finalize_file_struct(struct file *filp);
void release_free_slot_in_global(int32_t fd);
// void initialize_stdio(void);

void file_table_init(void);

int32_t no_read_fn(struct file *, char *, uint32_t);
int32_t no_write_fn(struct file *, const char *, uint32_t);
int32_t no_llseek_fn(struct file *, int32_t, int32_t);
int32_t no_release_fn(struct file *);
#endif
