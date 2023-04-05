#include "atomic.h"
#include "kernel/global.h"
#include "kernel/interrupt.h"

void atomic_init(struct atomic_t* v) {
    v->counter = 0;
}

int atomic_read(const struct atomic_t* v) {
    enum intr_status status;
    int res;

    interrupt_disable(&status);
    res = v->counter;
    interrupt_restore(status);

    return res;
}

// return old value
int atomic_set(struct atomic_t* v, int x) {
    int old;
    while (true) {
        old = atomic_read(v);
        if (atomic_cmpxchg(v, old, x) == old) {
            break;
        }
    }
    return old;
}

// return old value if success, or return the current value of v->counter
int atomic_cmpxchg(struct atomic_t* v, int old, int _new) {
    enum intr_status status;
    int res;

    interrupt_disable(&status);
    res = v->counter;
    if (v->counter == old) {
        v->counter = _new;
    }
    interrupt_restore(status);
    return res;
}

int atomic_inc(struct atomic_t* v) {
    int old, _new;
    while (true) {
        old = atomic_read(v);
        _new = old + 1;
        if (atomic_cmpxchg(v, old, _new) == old) {
            break;
        }
    }
    return _new;
}

int atomic_dec(struct atomic_t* v) {
    int old, _new;
    while (true) {
        old = atomic_read(v);
        _new = old - 1;
        if (atomic_cmpxchg(v, old, _new) == old) {
            break;
        }
    }
    return _new;
}