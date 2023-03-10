#include "kernel/init.h"
#include "lib/kernel/print.h"
#include "thread/thread.h"

void k_thread_a(void*);

int main(void) {
    put_str("I am kernel\n");
    init_all();

    thread_start("k_thread_a", 31, k_thread_a, "K-thread-a-argA ");

    while(1);
    return 0;
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg) {
    /* 用void*来通用表示参数,被调用的函数知道自己需要什么类型的参数,自己转换再用 */
    char* para = arg;
    int32_t number = 0;
    while(1) {
        put_str(para);
        put_char(' ');
        put_int(number++);
        put_char('\n');
    }
}
