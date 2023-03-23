#ifndef __KERNEL_CONDITION_H
#define __KERNEL_CONDITION_H
#include "kernel/list.h"
#include "stdint.h"
#include "thread/sync.h"
#include "thread/thread.h"

struct condition_variable {
    struct lock *plock;
    struct list waiters;
};

void cv_init(struct condition_variable *cv, struct lock *plock);
void cv_wait(struct condition_variable *cv);
void cv_notify_one(struct condition_variable *cv);
void cv_notify_all(struct condition_variable *cv);

#endif
