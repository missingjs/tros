#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "user/assert.h"
#include "user/syscall.h"

#include "builtin.h"
#include "filesys.h"

#define MAX_ARG_NR 16       // 加上命令名外,最多支持15个参数
#define STDIN_FILENO 0

/* 存储输入的命令 */
static char cmd_line[MAX_PATH_LEN] = {0};
extern char final_path[];

/* 用来记录当前目录,是当前目录的缓存,每次执行cd命令时会更新此内容 */
char cwd_cache[MAX_PATH_LEN] = {0};

extern char **environ;

static void execute_piped_commands(char *command_line);

static void print_prompt(void) {
   printf("[rabbit@localhost %s]$ ", cwd_cache);
}

static int readline(char *buf, uint32_t count) {
    assert(buf != NULL && count > 0);
    int r;
    if ((r = read(STDIN_FILENO, buf, count-1)) >= 0) {
        if (r > 0 && buf[r-1] == '\n') {
            --r;
        }
        buf[r] = 0;
    } else {
        // TODO: handle read error
        buf[0] = 0;
    }
    return r;
}

/* 分析字符串cmd_str中以token为分隔符的单词,将各单词的指针存入argv数组 */
static int32_t cmd_parse(char* cmd_str, char** argv, char token) {
   assert(cmd_str != NULL);
   int32_t arg_idx = 0;
   while(arg_idx < MAX_ARG_NR) {
      argv[arg_idx] = NULL;
      arg_idx++;
   }
   char* next = cmd_str;
   int32_t argc = 0;
   /* 外层循环处理整个命令行 */
   while(*next) {
      /* 去除命令字或参数之间的空格 */
      while(*next == token) {
     next++;
      }
      /* 处理最后一个参数后接空格的情况,如"ls dir2 " */
      if (*next == 0) {
     break;
      }
      argv[argc] = next;

     /* 内层循环处理命令行中的每个命令字及参数 */
      while (*next && *next != token) {      // 在字符串结束前找单词分隔符
     next++;
      }

      /* 如果未结束(是token字符),使tocken变成0 */
      if (*next) {
     *next++ = 0;    // 将token字符替换为字符串结束符0,做为一个单词的结束,并将字符指针next指向下一个字符
      }

      /* 避免argv数组访问越界,参数过多则返回0 */
      if (argc > MAX_ARG_NR) {
     return -1;
      }
      argc++;
   }
   return argc;
}

/* 执行命令 */
static void cmd_execute(uint32_t argc, char **argv)
{
   if (!strcmp("cd", argv[0]))
   {
      char final_path[MAX_PATH_LEN] = {0};
      if (buildin_cd(argc, argv, final_path) != NULL)
      {
         memset(cwd_cache, 0, MAX_PATH_LEN);
         strcpy(cwd_cache, final_path);
      }
   }
   else if (!strcmp("pwd", argv[0]))
   {
      buildin_pwd(argc, argv);
   }
   else if (!strcmp("ps", argv[0]))
   {
      buildin_ps(argc, argv);
   }
   else if (!strcmp("clear", argv[0]))
   {
      buildin_clear(argc, argv);
   }
   else if (!strcmp("mkdir", argv[0]))
   {
      buildin_mkdir(argc, argv);
   }
   else if (!strcmp("rmdir", argv[0]))
   {
      buildin_rmdir(argc, argv);
   }
   else if (!strcmp("rm", argv[0]))
   {
      buildin_rm(argc, argv);
   }
   else if (!strcmp("help", argv[0]))
   {
      buildin_help(argc, argv);
   }
   else
   { // 如果是外部命令,需要从磁盘上加载
      int32_t pid = fork();
      if (pid) {
         set_fg_pid(pid);
         int32_t status;
         int32_t child_pid = wait(&status); // 此时子进程若没有执行exit,my_shell会被阻塞,不再响应键入的命令
         if (child_pid == -1)
         { // 按理说程序正确的话不会执行到这句,fork出的进程便是shell子进程
            panic("my_shell: no child\n");
         }
         set_fg_pid(getpid());
      } else {
         char _final_path[MAX_PATH_LEN];
         if (!build_absolute_path(argv[0], _final_path)) {
            printf("my_shell: cannot access %s: No such file or directory\n", argv[0]);
            exit(-1);
         }
         char *const *current_envp = environ;
         execve(_final_path, argv, current_envp);
      }
   }
}

int main(void) {
   set_fg_pid(getpid());

   char* argv[MAX_ARG_NR] = {NULL};
   int32_t argc = -1;

   cwd_cache[0] = '/';
   while (1) {
      print_prompt();
      memset(final_path, 0, MAX_PATH_LEN);
      memset(cmd_line, 0, MAX_PATH_LEN);
      int r;
      if ((r = readline(cmd_line, MAX_PATH_LEN)) < 0) {
         printf("[shell] failed to readline, return value is %d\n", r);
         break;
      }
      if (cmd_line[0] == 0) { // 若只键入了一个回车
         continue;
      }

      /* 针对管道的处理 */
      char *pipe_symbol = strchr(cmd_line, '|');
      if (pipe_symbol) {
         execute_piped_commands(cmd_line);
      } else { // 一般无管道操作的命令
         argc = -1;
         argc = cmd_parse(cmd_line, argv, ' ');
         if (argc == -1) {
         printf("num of arguments exceed %d\n", MAX_ARG_NR);
         continue;
         }
         cmd_execute(argc, argv);
      }
   }
   panic("my_shell: should not be here");
   return 0;
}

static void execute_piped_commands(char *command_line) {
   int fd1[2], fd2[2];
   int *prev = fd1, *next = fd2, *tmp;
   pid_t pid;

   int cmd_index = 0;
   char *each_cmd = command_line;
   char *pipe_symbol;
   char cleared_path[MAX_PATH_LEN];
   char* argv[MAX_ARG_NR] = {NULL};
   char *const *current_envp = environ;

   while ((pipe_symbol = strchr(each_cmd, '|'))) {
      *pipe_symbol = 0;

      cmd_parse(each_cmd, argv, ' ');
      if (!build_absolute_path(argv[0], cleared_path)) {
         printf("my_shell: cannot access %s: No such file or directory\n", argv[0]);
         exit(-1);
      }

      pipe(next);
      if ((pid = fork()) == 0) {
         if (cmd_index > 0) {
            fd_redirect(0, prev[0]);
            close(prev[0]);
            close(prev[1]);
         }
         fd_redirect(1, next[1]);
         close(next[0]);
         close(next[1]);
         execve(cleared_path, argv, current_envp);
         panic("should not be here");
      } else if (cmd_index > 0) {
         close(prev[0]);
         close(prev[1]);
      } else {
         set_fg_pid(pid);
      }
      tmp = prev;
      prev = next;
      next = tmp;
      ++cmd_index;
      each_cmd = pipe_symbol + 1;
   }

   assert(cmd_index > 0);
   cmd_parse(each_cmd, argv, ' ');
   if (!build_absolute_path(argv[0], cleared_path)) {
      printf("my_shell: cannot access %s: No such file or directory\n", argv[0]);
      exit(-1);
   }

   if ((pid = fork()) == 0) {
      fd_redirect(0, prev[0]);
      close(prev[0]);
      close(prev[1]);
      execve(cleared_path, argv, current_envp);
      panic("should not be here");
   } else {
      close(prev[0]);
      close(prev[1]);
   }

   int32_t st;
   while ((pid = wait(&st)) != -1)
      ;;

   set_fg_pid(getpid());
}
