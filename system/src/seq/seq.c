#include "stdio.h"
#include "string.h"
#include "user/syscall.h"

void print_usage(void);
void generate(int start, int step, int end);
static int atoi(const char *str);

void print_usage(void) {
    printf("usage: \n");
    printf("   seq <end>\n");
    printf("   seq <start> <end>\n");
    printf("   seq <start> <step> <end>\n");
}

int atoi_error = 0;

void generate(int start, int step, int end) {
    if (step == 0) {
        printf("step can't be 0\n");
        exit(1);
    }

    if (step > 0) {
        for (long long int i = start; i <= end && i <= 0x7fffffff && i >= (int) 0x80000000; i += step) {
            printf("%d\n", (int)i);
        }
    } else {
        for (long long int i = start; i >= end && i <= 0x7fffffff && i >= (int) 0x80000000; i += step) {
            printf("%d\n", (int)i);
        }
    }
}

static int atoi(const char *str) {
    int minus = 0, value = 0;
    if (*str == '-') {
        minus = 1;
        ++str;
    }

    if (minus) {
        char ch;
        while ((ch = *str++)) {
            if (ch < '0' || ch > '9') {
                atoi_error = 1;
                return 0;
            }
            if (value > -214748364 || (value == -214748364 && ch < '9')) {
                value = value * 10 - (ch - '0');
            } else {
                break;
            }
        }
        return value;
    }

    char ch;
    while ((ch = *str++)) {
        if (ch < '0' || ch > '9') {
            atoi_error = 1;
            return 0;
        }
        if (value < 214748364 || (value == -214748364 && ch < '8')) {
            value = value * 10 + ch - '0';
        } else {
            break;
        }
    }
    return value;
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 4) {
        print_usage();
        exit(1);
    }

    int start, step, end;

    if (argc == 2) {
        start = step = 1;
        end = atoi(argv[1]);
    } else if (argc == 3) {
        start = atoi(argv[1]);
        step = 1;
        end = atoi(argv[2]);
    } else {
        start = atoi(argv[1]);
        step = atoi(argv[2]);
        end = atoi(argv[3]);
    }
    generate(start, step, end);
    return 0;
}