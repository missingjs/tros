#ifndef __DEVICE_IOQUEUE_H
#define __DEVICE_IOQUEUE_H
#include "stdint.h"
#include "thread/sync.h"
#include "thread/thread.h"

#define bufsize  2048	// 兼容管道的一页大小的struct ioqueue

/* 环形队列 */
struct ioqueue {
    struct lock lock;
    struct condition_variable cond_space_avail;
    struct condition_variable cond_data_avail;
    char buf[bufsize];			    // 缓冲区大小
    int32_t head;			    // 队首,数据往队首处写入
    int32_t tail;			    // 队尾,数据从队尾处读出
    bool closed;
};

void ioqueue_init(struct ioqueue* ioq);
bool ioq_full(struct ioqueue* ioq);
int ioq_getchar(struct ioqueue* ioq);
bool ioq_putchar(struct ioqueue* ioq, char byte);
uint32_t ioq_length(struct ioqueue* ioq);
void ioq_close(struct ioqueue* ioq);
bool ioq_is_closed(struct ioqueue* ioq);
#endif
