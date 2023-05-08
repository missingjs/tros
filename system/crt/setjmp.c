#include "setjmp.h"

int setjmp(jmp_buf env) {
    struct __jmp_buf *buf = env;

    uint32_t ebp, ebx, edi, esi;
    asm ("mov %%ebp, %0" : "=g" (ebp));
    asm ("mov %%ebx, %0" : "=g" (ebx));
    asm ("mov %%edi, %0" : "=g" (edi));
    asm ("mov %%esi, %0" : "=g" (esi));

    buf->esp = ebp + 4;
    buf->ebp = *(uint32_t*)ebp;
    buf->ret_addr = *(uint32_t*)(ebp + 4);
    buf->ebx = ebx;
    buf->edi = edi;
    buf->esi = esi;

    return 0;
}

extern void _long_jump(void);

void longjmp(jmp_buf env, int status) {
    struct __jmp_buf *buf = env;

    uint32_t new_esp = buf->esp;
    *(uint32_t*)new_esp = buf->ret_addr;

    asm volatile ("push %0" :: "r" (buf->esp));
    asm volatile ("push %0" :: "r" (status));
    asm volatile ("push %0" :: "r" (buf->ebp));
    asm volatile ("push %0" :: "r" (buf->ebx));
    asm volatile ("push %0" :: "r" (buf->edi));
    asm volatile ("push %0" :: "r" (buf->esi));

    asm volatile ("pop %%esi" ::);
    asm volatile ("pop %%edi" ::);
    asm volatile ("pop %%ebx" ::);
    asm volatile ("pop %%ebp" ::);
    asm volatile ("pop %%eax" ::);
    asm volatile ("pop %%esp" ::);
    asm volatile ("ret");
}