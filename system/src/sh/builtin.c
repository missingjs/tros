#include "fs/dir.h"
#include "stdio.h"
#include "string.h"
#include "user/assert.h"
#include "user/syscall.h"

#include "builtin.h"
#include "filesys.h"
#include "stdlib.h"

/* cd命令的内建函数 */
char* buildin_cd(uint32_t argc, char** argv, char *final_path) {
   if (argc > 2) {
      printf("cd: only support 1 argument!\n");
      return NULL;
   }

   /* 若是只键入cd而无参数,直接返回到根目录. */
   if (argc == 1) {
      final_path[0] = '/';
      final_path[1] = 0;
   } else {
      make_clear_abs_path(argv[1], final_path);
   }

   if (chdir(final_path) == -1) {
      printf("cd: no such directory %s\n", final_path);
      return NULL;
   }
   return final_path;
}

/* ps命令内建函数 */
void buildin_ps(uint32_t argc, char** argv UNUSED) {
   if (argc != 1) {
      printf("ps: no argument support!\n");
      return;
   }
   ps();
}

/* 显示内建命令列表 */
void buildin_help(uint32_t argc UNUSED, char** argv UNUSED) {
   printf("\
 buildin commands:\n\
       ls: show directory or file information\n\
       cd: change current work directory\n\
       mkdir: create a directory\n\
       rmdir: remove a empty directory\n\
       rm: remove a regular file\n\
       pwd: show current work directory\n\
       ps: show process information\n\
       clear: clear screen\n\
 shortcut key:\n\
       ctrl+l: clear screen\n\
       ctrl+u: clear input\n\n");
}
