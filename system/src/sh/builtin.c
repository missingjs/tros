#include "fs/dir.h"
#include "stdio.h"
#include "string.h"
#include "user/assert.h"
#include "user/syscall.h"

#include "builtin.h"
#include "filesys.h"
#include "stdlib.h"

extern char final_path[];

#define MAX_FILE_NAME_LEN 16

/* pwd命令的内建函数 */
void buildin_pwd(uint32_t argc, char** argv UNUSED) {
   if (argc != 1) {
      printf("pwd: no argument support!\n");
      return;
   } else {
      if (NULL != getcwd(final_path, MAX_PATH_LEN)) {
    printf("%s\n", final_path);
      } else {
    printf("pwd: get current work directory failed.\n");
      }
   }
}

/* cd命令的内建函数 */
char* buildin_cd(uint32_t argc, char** argv) {
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

/* clear命令内建函数 */
void buildin_clear(uint32_t argc, char** argv UNUSED) {
   if (argc != 1) {
      printf("clear: no argument support!\n");
      return;
   }
   clear();
}

/* mkdir命令内建函数 */
int32_t buildin_mkdir(uint32_t argc, char** argv) {
   int32_t ret = -1;
   if (argc != 2) {
      printf("mkdir: only support 1 argument!\n");
   } else {
      make_clear_abs_path(argv[1], final_path);
      /* 若创建的不是根目录 */
      if (strcmp("/", final_path)) {
    if (mkdir(final_path) == 0) {
       ret = 0;
    } else {
       printf("mkdir: create directory %s failed.\n", argv[1]);
    }
      }
   }
   return ret;
}

/* rmdir命令内建函数 */
int32_t buildin_rmdir(uint32_t argc, char** argv) {
   int32_t ret = -1;
   if (argc != 2) {
      printf("rmdir: only support 1 argument!\n");
   } else {
      make_clear_abs_path(argv[1], final_path);
   /* 若删除的不是根目录 */
      if (strcmp("/", final_path)) {
    if (rmdir(final_path) == 0) {
       ret = 0;
    } else {
       printf("rmdir: remove %s failed.\n", argv[1]);
    }
      }
   }
   return ret;
}

/* rm命令内建函数 */
int32_t buildin_rm(uint32_t argc, char** argv) {
   int32_t ret = -1;
   if (argc != 2) {
      printf("rm: only support 1 argument!\n");
   } else {
      make_clear_abs_path(argv[1], final_path);
   /* 若删除的不是根目录 */
      if (strcmp("/", final_path)) {
    if (unlink(final_path) == 0) {
       ret = 0;
    } else {
       printf("rm: delete %s failed.\n", argv[1]);
    }

      }
   }
   return ret;
}

/* 显示内建命令列表 */
void buildin_help(uint32_t argc UNUSED, char** argv UNUSED) {
   help();
}
