#ifndef TROS_CRT_SETJMP_H
#define TROS_CRT_SETJMP_H

#include "stdint.h"

struct __jmp_buf {
    uint32_t esp;
    uint32_t ebp;
    uint32_t ret_addr;
    uint32_t ebx;
    uint32_t esi;
    uint32_t edi;
};

typedef struct __jmp_buf jmp_buf[1];

int setjmp(jmp_buf env);

void longjmp(jmp_buf env, int status);

#endif