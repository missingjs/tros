#ifndef __KERNEL_SHELL_H
#define __KERNEL_SHELL_H
#include "fs/fs.h"
void print_prompt(void);
void my_shell(void);
extern char final_path[MAX_PATH_LEN];

#define MAX_ARG_NR 16       // 加上命令名外,最多支持15个参数

#endif
