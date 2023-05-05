#include "stdio.h"

extern char **environ;

int main(int argc, char *argv[])
{
    printf("&argc = %x\n", &argc);
    printf("&argv = %x\n", &argv);
    for (int i = 0; i < argc; ++i) {
        printf("argv[%d] - %x, %s\n", i, argv[i], argv[i]);
    }
    printf("start address of environ pointers array: %x\n", environ);
    char **ep = environ;
    while (*ep) {
        printf("%s\n", *ep);
        ++ep;
    }
    return 0;
}