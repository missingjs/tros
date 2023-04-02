#include "device/ioqueue.h"
#include "kernel/debug.h"
#include "kernel/global.h"
#include "kernel/interrupt.h"

/* 初始化io队列ioq */
void ioqueue_init(struct ioqueue* ioq) {
   lock_init(&ioq->lock);     // 初始化io队列的锁
   cv_init(&ioq->cond_space_avail, &ioq->lock);
   cv_init(&ioq->cond_data_avail, &ioq->lock);
   ioq->head = ioq->tail = 0; // 队列的首尾指针指向缓冲区数组第0个位置
   ioq->closed = false;
}

/* 返回pos在缓冲区中的下一个位置值 */
static int32_t next_pos(int32_t pos) {
   return (pos + 1) % bufsize;
}

/* 判断队列是否已满 */
static bool is_ioq_full(struct ioqueue* ioq) {
   return next_pos(ioq->head) == ioq->tail;
}

/* 判断队列是否已空 */
static bool is_ioq_empty(struct ioqueue* ioq) {
   return ioq->head == ioq->tail;
}

bool ioq_full(struct ioqueue* ioq) {
    bool res;
    lock_acquire(&ioq->lock);
    res = is_ioq_full(ioq);
    lock_release(&ioq->lock);
    return res;
}

/* 消费者从ioq队列中获取一个字符 */
int ioq_getchar(struct ioqueue* ioq) {
   lock_acquire(&ioq->lock);

   while (!ioq->closed && is_ioq_empty(ioq)) {
      cv_wait(&ioq->cond_data_avail);
   }

   if (ioq->closed && is_ioq_empty(ioq)) {
      lock_release(&ioq->lock);
      return -1;
   }

   char byte = ioq->buf[ioq->tail];	  // 从缓冲区中取出
   ioq->tail = next_pos(ioq->tail);	  // 把读游标移到下一位置

   cv_notify_one(&ioq->cond_space_avail);
   lock_release(&ioq->lock);
   return (unsigned char) byte;
}

/* 生产者往ioq队列中写入一个字符byte */
bool ioq_putchar(struct ioqueue* ioq, char byte) {
   lock_acquire(&ioq->lock);

   while (!ioq->closed && is_ioq_full(ioq)) {
       cv_wait(&ioq->cond_space_avail);
   }

   if (ioq->closed) {
      lock_release(&ioq->lock);
      return false;
   }

   ioq->buf[ioq->head] = byte;      // 把字节放入缓冲区中
   ioq->head = next_pos(ioq->head); // 把写游标移到下一位置

   cv_notify_one(&ioq->cond_data_avail);
   lock_release(&ioq->lock);
   return true;
}

/* 返回环形缓冲区中的数据长度 */
uint32_t ioq_length(struct ioqueue* ioq) {
   uint32_t len = 0;
   lock_acquire(&ioq->lock);
   if (ioq->head >= ioq->tail) {
      len = ioq->head - ioq->tail;
   } else {
      len = bufsize - (ioq->tail - ioq->head);
   }
   lock_release(&ioq->lock);
   return len;
}

void ioq_close(struct ioqueue* ioq) {
   lock_acquire(&ioq->lock);
   if (!ioq->closed) {
      ioq->closed = true;
      cv_broadcast(&ioq->cond_data_avail);
      cv_broadcast(&ioq->cond_space_avail);
   }
   lock_release(&ioq->lock);
}

bool ioq_is_closed(struct ioqueue* ioq) {
   bool res;
   lock_acquire(&ioq->lock);
   res = ioq->closed;
   lock_release(&ioq->lock);
   return res;
}
