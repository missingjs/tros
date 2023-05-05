#include "stdio.h"
#include "user/syscall.h"

int main(void)
{
    int count = 0;
    while (1) {
        msleep(5000);
        printf("count: %d\n", count++);
    }
    return 0;
}