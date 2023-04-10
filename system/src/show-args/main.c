#include "stdio.h"

int main(int argc, char *argv[])
{
    printf("&argc = %x\n", &argc);
    printf("&argv = %x\n", &argv);
    for (int i = 0; i < argc; ++i) {
        printf("argv[%d] - %x, %s\n", i, argv[i], argv[i]);
    }
    return 0;
}