#include "stdio.h"
#include "string.h"
#include "user/syscall.h"

#define BUF_SIZE 512

struct read_buffer {
    char b1[BUF_SIZE];
    char b2[BUF_SIZE];
    char *curr;
    char *next;
    char *ptr;
    char *end;
};

static void init_read_buffer(struct read_buffer *buf) {
    buf->curr = &buf->b2[0];
    buf->next = &buf->b1[0];
    buf->ptr = buf->curr + BUF_SIZE;
    buf->end = buf->ptr;
}

char *lines[1024];
char line[1024];

static int read_line(struct read_buffer *buf, char *out) {
    int total = 0, r;
    char *start = out;

    while (1) {
        while (buf->ptr != buf->end && *buf->ptr != '\n') {
            *out++ = *buf->ptr++;
        }
        if (buf->ptr == buf->end) {
            int expected_size = BUF_SIZE;
            if (buf->end == buf->curr + BUF_SIZE) {
                char *tmp = buf->curr;
                buf->curr = buf->next;
                buf->next = tmp;
                buf->ptr = buf->curr;
            } else {
                expected_size = BUF_SIZE - (buf->ptr - buf->curr);
            }
            
            r = read(0, buf->ptr, expected_size);
            buf->end = buf->ptr + r;
            total += r;
            if (r == 0) {
                *out = 0;
                return total > 0 || out != start ? out - start : -1;
            }
        } else {
            ++buf->ptr;
            *out = 0;
            return out - start;
        }
    }
}

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
    struct read_buffer buf;
    init_read_buffer(&buf);

    int r, count = 0;
    while (1) {
        r = read_line(&buf, line);
        if (r == 0) {
            continue;
        } else if (r < 0) {
            break;
        }
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