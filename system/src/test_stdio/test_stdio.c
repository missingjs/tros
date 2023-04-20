#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main(void)
{
    char buffer[1024];
    char *s;
    while ((s = fgets(buffer, 1023, stdin)) != NULL) {
        uint32_t size = strlen(buffer);
        if (size > 0 && buffer[size-1] == '\n') {
            printf("content (%d) with LF: %s", size, s);
        } else {
            printf("content (%d) without LF: %s\n", size, s);
        }
    }
    return 0;
}