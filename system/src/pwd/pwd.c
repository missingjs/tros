#include "stdio.h"
#include "user/syscall.h"

int main(int argc, char *argv[] __attribute__ ((unused))) {
    char final_path[512] = {0};
    if (argc != 1) {
        printf("pwd: no argument support!\n");
        return 1;
    }
    if (!getcwd(final_path, MAX_PATH_LEN)) {
        printf("pwd: get current work directory failed.\n");
        return 1;
    }
    printf("%s\n", final_path);
    return 0;
}