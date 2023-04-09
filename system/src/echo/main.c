#include "stdio.h"

int main(int argc, char **argv)
{
    int i;

    if (argc < 2) {
        printf("\n");
        return 0;
    }

    printf("%s", argv[1]);
    for (i = 2; i < argc; ++i) {
        printf(" %s", argv[i]);
    }
    printf("\n");
    return 0;
}
