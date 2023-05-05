#include "stdio.h"
#include "string.h"
#include "user/syscall.h"

char *lines[1024];
char line[1024];

static void do_sort(int n) {
    for (int i = 1; i < n; ++i) {
        char *s = lines[i];
        int j = i;
        for (; j > 0 && strcmp(lines[j-1], s) >= 0; --j) {
            lines[j] = lines[j-1];
        }
        lines[j] = s;
    }
}

int main(void) {
    int count = 0;
    while (1) {
        char *s = fgets(line, 1023, stdin);
        if (!s) {
            break;
        }
        int r = strlen(line) - 1;
        if (r == 0) {
            continue;
        }
        line[r] = 0;  // override '\n'
        char *new_line = (char*)malloc(r + 1);
        strcpy(new_line, line);
        lines[count++] = new_line;
    }

    do_sort(count);

    for (int i = 0; i < count; ++i) {
        printf("%s\n", lines[i]);
        free(lines[i]);
    }
    return 0;
}