#include "stdio.h"
#include "string.h"
#include "user/syscall.h"

int main(void) {
    char filename[] = "/test-file.txt";
    unlink(filename);
    FILE *fp = fopen(filename, "w");
    char name[] = "ant-man";
    int episode = 3;
    fprintf(fp, "name: %s, episode: %d\n", name, episode);
    fclose(fp);

    fp = fopen(filename, "r");
    char buf[64];
    fgets(buf, 63, fp);
    fclose(fp);
    printf("content read: %s", buf);
    return 0;
}