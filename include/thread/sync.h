#ifndef __THREAD_SYNC_H
#define __THREAD_SYNC_H
#include "kernel/list.h"
#include "stdint.h"
#include "thread/thread.h"

/* 信号量结构 */
struct semaphore {
   uint8_t  value;
   struct   list waiters;
};

/* 锁结构 */
struct lock {
   struct   task_struct* holder;	    // 锁的持有者
   struct   semaphore semaphore;	    // 用二元信号量实现锁
   uint32_t holder_repeat_nr;		    // 锁的持有者重复申请锁的次数
};

struct condition_variable {
    struct lock *plock;
    struct list waiters;
};

void sema_init(struct semaphore* psema, uint8_t value); 
void sema_down(struct semaphore* psema);
void sema_up(struct semaphore* psema);
void lock_init(struct lock* plock);
void lock_acquire(struct lock* plock);
void lock_release(struct lock* plock);
void cv_init(struct condition_variable *cv, struct lock *plock);
void cv_wait(struct condition_variable *cv);
void cv_notify_one(struct condition_variable *cv);
void cv_broadcast(struct condition_variable *cv);
#endif
