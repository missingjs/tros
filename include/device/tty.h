#ifndef __DEVICE_TTY_H
#define __DEVICE_TTY_H
#include "stdint.h"

struct file;
struct tty_struct;

struct tty_ldisc_ops {
    char *name;
    int     (*open)(struct tty_struct *tty);
    void    (*close)(struct tty_struct *tty);
    int32_t (*read)(struct tty_struct *tty, struct file *file,
                    unsigned char *buf, uint32_t nr);
    int32_t (*write)(struct tty_struct *tty, struct file *file,
                     const unsigned char *buf, uint32_t nr);
    void    (*receive_buf)(struct tty_struct *tty, const unsigned char *buf, uint32_t count);
};

struct tty_ldisc {
    struct tty_ldisc_ops *ops;
    struct tty_struct *tty;
};

struct tty_struct {
    struct tty_ldisc *ldisc;
    void *disc_data;
};

void tty_init(void);
void tty_data_arrived(const unsigned char *buf, uint32_t count);

#endif
