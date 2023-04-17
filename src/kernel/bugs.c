#include "device/console.h"
#include "kernel/interrupt.h"
#include "kernel/memory.h"
#include "kernel/stdio-kernel.h"
#include "userprog/process.h"

void bugs_page_table(void);
void bugs_page_table_2(void);

void bugs_page_table(void)
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

void bugs_page_table_2(void) {
   intr_disable();

   void *p1 = sys_malloc(32);
   uint32_t pg1 = 0xfffff000 & (uint32_t)p1;
   printk("[a] v:%x p:%x\n", pg1, addr_v2p(pg1));

   void *p2 = sys_malloc(64);
   uint32_t pg2 = 0xfffff000 & (uint32_t)p2;
   printk("[b] v:%x p:%x\n", pg2, addr_v2p(pg2));

   sys_free(p1);

   void *p3 = sys_malloc(8192);
   uint32_t pg3 = 0xfffff000 & (uint32_t)p3;
   printk("[c] v:%x p:%x\n", pg3, addr_v2p(pg3));

   void *p4 = sys_malloc(256);
   uint32_t pg4 = 0xfffff000 & (uint32_t)p4;
   printk("[d] v:%x p:%x\n", pg4, addr_v2p(pg4));

   uint32_t *xp = (uint32_t*)pg4;
   printk("%x %x %x\n", xp[0], xp[1], xp[2]);

   page_dir_activate(running_thread());
   
   printk("%x %x %x\n", xp[0], xp[1], xp[2]);

   while(1);
}
