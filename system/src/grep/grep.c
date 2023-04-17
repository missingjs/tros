#include "stdio.h"
#include "string.h"
#include "user/syscall.h"

#define BUF_SIZE 1024

char buf1[BUF_SIZE];
char buf2[BUF_SIZE];

static int str_find(const char *str, const char *sub) {
    const char *p = str;
    while (*p) {
        const char *a = p;
        const char *b = sub;
        while (*a && *b && *a == *b) {
            ++a;
            ++b;
        }
        if (!*b) {
            return 1;
        }
        ++p;
    }
    return 0;
}

static void do_grep(const char *str, const char *key) {
    if (str_find(str, key)) {
        printf("%s\n", str);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: grep <keyword>");
        exit(1);
    }

    const char *key = argv[1];
    char *curr = buf1, *next = buf2;
    int rpos = 0;

    while (1) {
        int n = read(0, curr + rpos, BUF_SIZE - rpos);
        if (n == 0) {
            break;
        }
        char *p = curr, *q = p, *end = p + n;
        while (1) {
            while (p != end && *p != '\n') {
                ++p;
            }
            if (p == end) {
                break;
            }
            *p = 0;
            do_grep(q, key);
            q = p + 1;  // skip '\n'
        }
        memcpy(next, q, end - q);
        rpos = end - q;
        char *tmp = curr;
        curr = next;
        next = tmp;
    }
    return 0;
}