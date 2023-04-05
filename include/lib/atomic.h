#ifndef __LIB_ATOMIC_H
#define __LIB_ATOMIC_H

#include "kernel/global.h"
#include "kernel/interrupt.h"

struct atomic_t {
    volatile int counter;
};

void atomic_init(struct atomic_t* v);
int atomic_read(const struct atomic_t* v);

// return old value
int atomic_set(struct atomic_t* v, int x);

// return old value if success, or return the current value of v->counter
int atomic_cmpxchg(struct atomic_t* v, int old, int _new);

int atomic_inc(struct atomic_t* v);
int atomic_dec(struct atomic_t* v);

#endif