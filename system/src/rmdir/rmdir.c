#include "filesys.h"
#include "stdio.h"
#include "string.h"
#include "user/syscall.h"

int main(int argc, char *argv[]) {
    char final_path[512] = {0};
    int32_t ret = -1;
    if (argc != 2) {
        printf("rmdir: only support 1 argument!\n");
    } else {
        make_clear_abs_path(argv[1], final_path);
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