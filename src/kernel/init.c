#include "device/console.h"
#include "device/ide.h"
#include "device/keyboard.h"
#include "device/timer.h"
#include "device/tty.h"
#include "fs/file.h"
#include "fs/fs.h"
#include "kernel/init.h"
#include "kernel/interrupt.h"
#include "kernel/memory.h"
#include "kernel/print.h"
#include "thread/thread.h"
#include "userprog/syscall-init.h"
#include "userprog/tss.h"

/*负责初始化所有模块 */
void init_all() {
   put_str("init_all\n");
   idt_init();	     // 初始化中断
   mem_init();	     // 初始化内存管理系统
   thread_init();    // 初始化线程相关结构
   timer_init();     // 初始化PIT
   console_init();   // 控制台初始化最好放在开中断之前
   keyboard_init();  // 键盘初始化
   tss_init();       // tss初始化
   syscall_init();   // 初始化系统调用
   file_table_init();   // init kernel file table
   tty_init();
   intr_enable();    // 后面的ide_init需要打开中断
   ide_init();	     // 初始化硬盘
   filesys_init();   // 初始化文件系统
   put_str("init_all done\n");
}
