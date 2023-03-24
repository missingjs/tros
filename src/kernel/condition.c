#include "kernel/condition.h"
#include "kernel/debug.h"
#include "kernel/interrupt.h"
#include "kernel/stdio-kernel.h"

static bool has_locked(struct condition_variable *cv) {
    return cv->plock->holder == running_thread();
}

void cv_init(struct condition_variable *cv, struct lock *plock) {
    ASSERT(cv != NULL && plock != NULL);
    cv->plock = plock;
    list_init(&cv->waiters);
}

void cv_wait(struct condition_variable *cv) {
    struct task_struct *self = running_thread();

    // current thread must be the lock holder
    ASSERT(has_locked(cv));

    // current thread must NOT be in the waiter list
    ASSERT(!elem_find(&cv->waiters, &self->general_tag));

    enum intr_status status = intr_disable();

    list_append(&cv->waiters, &self->general_tag);

    lock_release(cv->plock);

    thread_block(TASK_BLOCKED);

    intr_set_status(status);

    // awaken by other thread
    ASSERT(self->status == TASK_RUNNING);
    ASSERT(!has_locked(cv));
    ASSERT(!elem_find(&cv->waiters, &self->general_tag));

    lock_acquire(cv->plock);
}

static void _cv_notify(struct condition_variable *cv, int n) {
    int i = 0;
    struct task_struct *self = running_thread();
    ASSERT(has_locked(cv));
    ASSERT(!elem_find(&cv->waiters, &self->general_tag));
    for (i = 0; i < n && !list_empty(&cv->waiters); ++i) {
        struct list_elem *elem = list_pop(&cv->waiters);
        struct task_struct *waiter = elem2entry(struct task_struct, general_tag, elem);
        ASSERT(waiter->status == TASK_BLOCKED);
        thread_unblock(waiter);
    }
}

void cv_notify_one(struct condition_variable *cv) {
    _cv_notify(cv, 1);
}

void cv_notify_all(struct condition_variable *cv) {
    _cv_notify(cv, 2147483647);
}
