#include "kernel/debug.h"
#include "kernel/global.h"
#include "kernel/interrupt.h"
#include "kernel/list.h"
#include "kernel/memory.h"
#include "thread/sync.h"

enum waiter_status {
   WAITER_CREATED,
   WAITER_QUEUED,
   WAITER_BLOCKED,
   WAITER_NOTIFIED
};

struct waiter_node {
   struct task_struct *pthread;
   enum waiter_status status;
   struct list_elem link;
};

static struct waiter_node* create_waiter_node(struct task_struct *pthread) {
   struct waiter_node* node = (struct waiter_node*) sys_malloc(sizeof(struct waiter_node));
   node->pthread = pthread;
   node->status = WAITER_CREATED;
   node->link.prev = node->link.next = NULL;
   return node;
}

static void release_waiter_node(struct waiter_node* pnode) {
   sys_free(pnode);
}

/* 初始化信号量 */
void sema_init(struct semaphore* psema, uint8_t value) {
   psema->value = value;       // 为信号量赋初值
   list_init(&psema->waiters); //初始化信号量的等待队列
}

/* 初始化锁plock */
void lock_init(struct lock* plock) {
   plock->holder = NULL;
   plock->holder_repeat_nr = 0;
   sema_init(&plock->semaphore, 1);  // 信号量初值为1
}

/* 信号量down操作 */
void sema_down(struct semaphore* psema) {
/* 关中断来保证原子操作 */
   enum intr_status old_status = intr_disable();
   while(psema->value == 0) {	// 若value为0,表示已经被别人持有
      ASSERT(!elem_find(&psema->waiters, &running_thread()->general_tag));
      /* 当前线程不应该已在信号量的waiters队列中 */
      if (elem_find(&psema->waiters, &running_thread()->general_tag)) {
	 PANIC("sema_down: thread blocked has been in waiters_list\n");
      }
/* 若信号量的值等于0,则当前线程把自己加入该锁的等待队列,然后阻塞自己 */
      list_append(&psema->waiters, &running_thread()->general_tag); 
      thread_block(TASK_BLOCKED);    // 阻塞线程,直到被唤醒
   }
/* 若value为1或被唤醒后,会执行下面的代码,也就是获得了锁。*/
   psema->value--;
   ASSERT(psema->value == 0);	    
/* 恢复之前的中断状态 */
   intr_set_status(old_status);
}

/* 信号量的up操作 */
void sema_up(struct semaphore* psema) {
/* 关中断,保证原子操作 */
   enum intr_status old_status = intr_disable();
   ASSERT(psema->value == 0);	    
   if (!list_empty(&psema->waiters)) {
      struct task_struct* thread_blocked = elem2entry(struct task_struct, general_tag, list_pop(&psema->waiters));
      thread_unblock(thread_blocked);
   }
   psema->value++;
   ASSERT(psema->value == 1);	    
/* 恢复之前的中断状态 */
   intr_set_status(old_status);
}

/* 获取锁plock */
void lock_acquire(struct lock* plock) {
/* 排除曾经自己已经持有锁但还未将其释放的情况。*/
   if (plock->holder != running_thread()) { 
      sema_down(&plock->semaphore);    // 对信号量P操作,原子操作
      plock->holder = running_thread();
      ASSERT(plock->holder_repeat_nr == 0);
      plock->holder_repeat_nr = 1;
   } else {
      plock->holder_repeat_nr++;
   }
}

/* 释放锁plock */
void lock_release(struct lock* plock) {
   ASSERT(plock->holder == running_thread());
   if (plock->holder_repeat_nr > 1) {
      plock->holder_repeat_nr--;
      return;
   }
   ASSERT(plock->holder_repeat_nr == 1);

   plock->holder = NULL;	   // 把锁的持有者置空放在V操作之前
   plock->holder_repeat_nr = 0;
   sema_up(&plock->semaphore);	   // 信号量的V操作,也是原子操作
}

static bool has_locked(struct condition_variable *cv) {
    return cv->plock->holder == running_thread();
}

void cv_init(struct condition_variable *cv, struct lock *plock) {
   ASSERT(cv != NULL && plock != NULL);
   cv->plock = plock;
   list_init(&cv->waiters);
}

void cv_wait(struct condition_variable *cv) {
   // current thread must be the lock holder
   ASSERT(has_locked(cv));

   struct task_struct *self = running_thread();
   struct waiter_node* wnode = create_waiter_node(self);
   list_append(&cv->waiters, &wnode->link);
   wnode->status = WAITER_QUEUED;

   // release lock before get into blocked
   lock_release(cv->plock);

   while (wnode->status != WAITER_NOTIFIED) {
      enum intr_status old_status = intr_disable();
      if (wnode->status != WAITER_NOTIFIED) {
         wnode->status = WAITER_BLOCKED;
         thread_block(TASK_BLOCKED);
      }
      intr_set_status(old_status);
   }

   // re-acquire lock
   lock_acquire(cv->plock);

   release_waiter_node(wnode);
}

static void _cv_notify(struct condition_variable *cv, int n) {
   // enum intr_status old_status = intr_disable();
   int i = 0;
   ASSERT(has_locked(cv));
   for (i = 0; i < n && !list_empty(&cv->waiters); ++i) {
      struct list_elem *elem = list_pop(&cv->waiters);
      struct waiter_node *wnode = elem2entry(struct waiter_node, link, elem);
      enum intr_status old_status = intr_disable();
      if (wnode->status == WAITER_BLOCKED) {
         thread_unblock(wnode->pthread);
      }
      wnode->status = WAITER_NOTIFIED;
      intr_set_status(old_status);
   }
}

void cv_notify_one(struct condition_variable *cv) {
   _cv_notify(cv, 1);
}

void cv_broadcast(struct condition_variable *cv) {
   _cv_notify(cv, 2147483647);
}
