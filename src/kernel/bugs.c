#include "device/console.h"
#include "kernel/memory.h"

void page_table(void);

void page_table(void)
{
    // set cpu's ips larger than 22,500,000 will trigger this bug
    //
    console_put_str("[bugs] page_table begin\n");
    void *p1, *p2, *p3, *p4;
    int i;
    for (i = 0; i < 200; ++i) {
        p1 = sys_malloc(128);
        p2 = sys_malloc(256);
        p3 = sys_malloc(512);

        sys_free(p1);
        sys_free(p3);

        p4 = sys_malloc(8192);
        sys_free(p4);

        sys_free(p2);
    }
    console_put_str("[bugs] page_table end\n");
    while(1);
}
