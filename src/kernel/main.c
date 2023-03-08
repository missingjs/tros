#include "lib/kernel/print.h"
#include "kernel/init.h"
int main(void) {
   put_str("I am kernel\n");
   init_all();
   asm volatile("sti");    // 为演示中断处理,在此临时开中断
   while(1);
   return 0;
}
