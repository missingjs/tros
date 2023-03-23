#include "kernel/condition.h"
#include "kernel/debug.h"
// #include "kernel/interrupt.h"

static bool has_locked(struct condition_variable *cv) {
    return cv->plock->holder == running_thread();
}

static bool is_in_ready_list(struct task_struct *thread) {
    return elem_find(&thread_ready_list, &thread->general_tag);
}

void cv_init(struct condition_variable *cv, struct lock *plock) {
    ASSERT(cv != NULL && plock != NULL);
    cv->plock = plock;
    list_init(&cv->waiters);
}

void cv_wait(struct condition_variable *cv) {
    struct task_struct *self = running_thread();

    // current thread must not in ready list, as it has been popped from ready list
    ASSERT(!is_in_ready_list(self));

    // current thread must be the lock holder
    ASSERT(has_locked(cv));

    // current thread must NOT be in the waiter list
    ASSERT(!elem_find(&cv->waiters, &self->general_tag));

    list_append(&cv->waiters, &self->general_tag);
    ASSERT(elem_find(&cv->waiters, &self->general_tag));

//    enum intr_status status = intr_disable();

    lock_release(cv->plock);

    thread_block(TASK_BLOCKED);

//    intr_set_status(status);

    // awaken by other thread
    ASSERT(self->status == TASK_RUNNING);
    ASSERT(!is_in_ready_list(self));
    ASSERT(!has_locked(cv));
    ASSERT(!elem_find(&cv->waiters, &self->general_tag));

    lock_acquire(cv->plock);
}

void cv_notify_one(struct condition_variable *cv) {
    struct task_struct *self = running_thread();
    ASSERT(has_locked(cv));
    ASSERT(!elem_find(&cv->waiters, &self->general_tag));
    struct list_elem *p = cv->waiters.head.next;
    while (p != &cv->waiters.tail) {
        struct task_struct *waiter = elem2entry(struct task_struct, general_tag, p);
        if (waiter->status == TASK_BLOCKED) {
            ASSERT(elem_find(&cv->waiters, p));
            list_remove(p);
            ASSERT(!elem_find(&cv->waiters, p));
            thread_unblock(waiter);
            break;
        }
        p = p->next;
    }
    // if (!list_empty(&cv->waiters)) {
    //     uint32_t length = list_len(&cv->waiters);
    //     struct list_elem *elem = list_pop(&cv->waiters);
    //     struct task_struct *waiter = elem2entry(struct task_struct, general_tag, elem);
    //     ASSERT(list_len(&cv->waiters) == length - 1);
    //     ASSERT(waiter->status == TASK_BLOCKED);
    //     ASSERT(!elem_find(&cv->waiters, elem));
    //     thread_unblock(waiter);
    // }
    ASSERT(has_locked(cv));
}

void cv_notify_all(struct condition_variable *cv) {
    ASSERT(has_locked(cv));
}
