#include "filesys.h"
#include "fs/dir.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "user/syscall.h"

int main(int argc, char *argv[]) {
    char* pathname = NULL;
    struct stat file_stat;
    memset(&file_stat, 0, sizeof(struct stat));
    bool long_info = false;
    uint32_t arg_path_nr = 0;
    int arg_idx = 1;   // 跨过argv[0],argv[0]是字符串“ls”
    char final_path[MAX_PATH_LEN] = {0};
    while (arg_idx < argc) {
        if (argv[arg_idx][0] == '-') {     // 如果是选项,单词的首字符是-
            if (!strcmp("-l", argv[arg_idx])) {         // 如果是参数-l
                long_info = true;
            } else if (!strcmp("-h", argv[arg_idx])) {   // 参数-h
                printf("usage: -l list all infomation about the file.\n-h for help\nlist all files in the current dirctory if no option\n");
                return 1;
            } else {   // 只支持-h -l两个选项
                printf("ls: invalid option %s\nTry `ls -h' for more information.\n", argv[arg_idx]);
                return 1;
            }
        } else {        // ls的路径参数
            if (arg_path_nr == 0) {
                pathname = argv[arg_idx];
                arg_path_nr = 1;
            } else {
                printf("ls: only support one path\n");
                return 1;
            }
        }
        arg_idx++;
    }

    if (pathname == NULL) {    // 若只输入了ls 或 ls -l,没有输入操作路径,默认以当前路径的绝对路径为参数.
        if (NULL != getcwd(final_path, MAX_PATH_LEN)) {
            pathname = final_path;
        } else {
            printf("ls: getcwd for default path failed\n");
            return 1;
        }
    } else {
        make_clear_abs_path(pathname, final_path);
        pathname = final_path;
    }

    if (stat(pathname, &file_stat) == -1) {
        printf("ls: cannot access %s: No such file or directory\n", pathname);
        return 2;
    }
    if (file_stat.st_filetype == FT_DIRECTORY) {
        struct dir* dir = opendir(pathname);
        struct dir_entry* dir_e = NULL;
        char sub_pathname[MAX_PATH_LEN] = {0};
        uint32_t pathname_len = strlen(pathname);
        uint32_t last_char_idx = pathname_len - 1;
        memcpy(sub_pathname, pathname, pathname_len);
        if (sub_pathname[last_char_idx] != '/') {
            sub_pathname[pathname_len] = '/';
            pathname_len++;
        }
        rewinddir(dir);
        if (long_info) {
            char ftype;
            printf("total: %d\n", file_stat.st_size);
            while((dir_e = readdir(dir))) {
                ftype = 'd';
                if (dir_e->f_type == FT_REGULAR) {
                    ftype = '-';
                }
                sub_pathname[pathname_len] = 0;
                strcat(sub_pathname, dir_e->filename);
                memset(&file_stat, 0, sizeof(struct stat));
                if (stat(sub_pathname, &file_stat) == -1) {
                    printf("ls: cannot access %s: No such file or directory\n", dir_e->filename);
                    return 2;
                }
                printf("%c  %d  %d  %s\n", ftype, dir_e->i_no, file_stat.st_size, dir_e->filename);
            }
        } else {
            while((dir_e = readdir(dir))) {
                printf("%s ", dir_e->filename);
            }
            printf("\n");
        }
        closedir(dir);
    } else {
        if (long_info) {
            printf("-  %d  %d  %s\n", file_stat.st_ino, file_stat.st_size, pathname);
        } else {
            printf("%s\n", pathname);
        }
    }
    return 0;
}